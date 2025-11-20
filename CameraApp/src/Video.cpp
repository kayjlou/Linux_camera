/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef __APPLE__
    #ifdef USE_GLES
        #include <gles2.h>
    #else
        #include <gl.h>
    #endif
#endif

#include "Video.h"
#include <cstdlib>
#include "MessageHandler.h"
#include "FileUtils.h"
#include "Utils.h"
#include <thread>
#include "ErrorHandler.h"
#include "Constants.h"

#ifndef SIMULATE_ALL_HARDWARE
#include <opencv2/opencv.hpp>
#endif

// Vertex data for a quad
const GLfloat vertices[] = {
    // Positions       // Texture Coordinates
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,

     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f
};

const GLfloat vertices_hflip[] = {
    // Positions       // Flipped Texture Coordinates (U flipped)
    -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 0.0f, 1.0f,

     1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 1.0f, 0.0f
};

const GLfloat vertices_vflip[] = {
    // Positions       // Texture Coordinates (V flipped)
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 0.0f, 0.0f,

     1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f
};

GstFlowReturn Video::newSample(GstAppSink *appsink, gpointer user_data) {
    Video *self = static_cast<Video *>(user_data);
    GstSample *sample = gst_app_sink_pull_sample(appsink);
    if (!sample) return GST_FLOW_ERROR;

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if (!gst_buffer_map(buffer, &map, GST_MAP_READ)) {
        gst_sample_unref(sample);
        return GST_FLOW_ERROR;
    }

    {
        std::lock_guard<std::mutex> lock(self->textureMutex);

        // Separate Y and UV planes from the incoming map data
        uint8_t* yPlaneData = map.data;
        uint8_t* uvPlaneData = map.data + self->ySize;

        self->yTextureData.assign(yPlaneData, yPlaneData + self->ySize);
        self->uvTextureData.assign(uvPlaneData, uvPlaneData + self->uvSize);

        if (self->whiteBalance->isFrameCollectionActive()) {
            if (self->whiteBalanceSkipFramesCounter >= Constants::WhiteBalanceCalibrationSkipFrames) {
                self->whiteBalance->addImageData(self->yTextureData, self->uvTextureData);
                self->whiteBalanceSkipFramesCounter = 0;
            } else {
                self->whiteBalanceSkipFramesCounter++;
            }
        }
    }

    if (self->snapshotRequest) {
        self->saveFrame(map);
        self->snapshotRequest = false;
    }

    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);

    // Check for frozen frames
    GstClockTime pts = GST_BUFFER_PTS(buffer);

    if (pts == self->lastPts) {
        self->ptsEqualCount++;

        if (self->ptsEqualCount > Constants::PtsEqualCountThreshold) {
            self->ptsUnchanged = true;
        }
    } else {
        self->ptsEqualCount = 0;
        self->ptsUnchanged = false;
        self->lastPts = pts;
    }

    self->sampleRenderCount = 0;

    if (self->recordingState == RecordingState::SwitchingBackToNormalStream) {
        self->recordingState = RecordingState::Idle;
    }

    return GST_FLOW_OK;
}

void Video::saveFrame(const GstMapInfo& map) const {
    #ifndef SIMULATE_ALL_HARDWARE
    if (snapshotFilename.empty()) {
        return;
    }

    // Convert NV12 to BGR using OpenCV
    cv::Mat nv12(Constants::CameraImageHeight * 3 / 2, Constants::CameraImageWidth, CV_8UC1, map.data);
    cv::Mat bgr;
    cv::cvtColor(nv12, bgr, cv::COLOR_YUV2BGR_NV12);

    cv::imwrite(snapshotFilename, bgr);
    #endif
}

Video::Video()
    : pipeline(nullptr), appsink(nullptr), yTextureID(0), uvTextureID(0), quadVAO(0), quadVBO(0), glResourcesInitialized(false) {
    ySize = Constants::CameraImageWidth * Constants::CameraImageHeight;
    uvSize = ySize / 2;
    whiteBalance = &whiteBalance1;
}

Video::~Video() {
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
    }
    if (yTextureID) glDeleteTextures(1, &yTextureID);
    if (uvTextureID) glDeleteTextures(1, &uvTextureID);
    if (quadVAO) glDeleteVertexArrays(1, &quadVAO);
    if (quadVBO) glDeleteBuffers(1, &quadVBO);
}

void Video::init() {
    initOpenGLResources();
    shaderProgram = createShaderProgram();
    initWhiteBalanceFactors();
    initGstPipeline();
}

void Video::initGstPipeline() {
    printf("initGstPipeline\n");
    gst_init(nullptr, nullptr);

#ifdef SIMULATE_ALL_HARDWARE
    std::string gstCommand = "filesrc location=../data/testData/omni_exp100000.png ! pngdec ! imagefreeze ! videoconvert ! "
    "video/x-raw,format=NV12,width=" + std::to_string(Constants::CameraImageWidth) + ",height=" + std::to_string(Constants::CameraImageHeight) + " ! appsink name=sink";
#else
    // for Yocto with OpenGL ES 2
    std::string gstCommand = "v4l2src device=/dev/video3 ! video/x-raw,format=NV12,width=" + std::to_string(Constants::CameraImageWidth) + ",height=" + std::to_string(Constants::CameraImageHeight) +
                " ! tee name=t t. ! queue ! appsink name=sink";
#endif

    pipeline = gst_parse_launch(gstCommand.c_str(), nullptr);
    if (!pipeline) {
        std::cerr << "Failed to create GStreamer pipeline" << std::endl;
        return;
    }

    appsink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
    if (!appsink) {
        std::cerr << "Failed to get appsink from pipeline" << std::endl;
        return;
    }

    GstAppSinkCallbacks callbacks = {};
    callbacks.new_sample = newSample;
    gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &callbacks, this, nullptr);
}

bool Video::linkRecordingBranchToGstPipeline() {
    // Get the tee element from the pipeline
    GstElement* tee = gst_bin_get_by_name(GST_BIN(pipeline), "t");

    // Create recording elements
    rec.queue = gst_element_factory_make("queue", nullptr);
    rec.encoder = gst_element_factory_make("v4l2h264enc", nullptr);
    rec.parser = gst_element_factory_make("h264parse", nullptr);
    rec.muxer = gst_element_factory_make("mp4mux", nullptr);
    rec.filesink = gst_element_factory_make("filesink", nullptr);
    g_object_set(rec.filesink, "location", videoFilename.c_str(), NULL);

    if (!rec.queue || !rec.encoder || !rec.parser || !rec.muxer || !rec.filesink || !tee) {
        printf("Failed to create one or more recording elements.\n");
        return false;
    }

    // Add to pipeline
    gst_bin_add_many(GST_BIN(pipeline), rec.queue, rec.encoder, rec.parser, rec.muxer, rec.filesink, NULL);

    // Link elements in the recording branch
    if (!gst_element_link_many(rec.queue, rec.encoder, rec.parser, rec.muxer, rec.filesink, NULL)) {
        printf("Failed to link recording elements.\n");
        return false;
    }

    // Sync state with parent (must be done after adding to pipeline)
    gst_element_sync_state_with_parent(rec.queue);
    gst_element_sync_state_with_parent(rec.encoder);
    gst_element_sync_state_with_parent(rec.parser);
    gst_element_sync_state_with_parent(rec.muxer);
    gst_element_sync_state_with_parent(rec.filesink);

    // Request a new src pad from the tee and link it to the recording queue
    rec.tee_src_pad = gst_element_request_pad_simple(tee, "src_%u");
    GstPad* queue_sink_pad = gst_element_get_static_pad(rec.queue, "sink");

    bool success = true;

    if (gst_pad_link(rec.tee_src_pad, queue_sink_pad) != GST_PAD_LINK_OK) {
        printf("Failed to link tee to recording queue.\n");
        success = false;
    }

    gst_object_unref(queue_sink_pad);
    gst_object_unref(tee);

    return success;
}

bool Video::stopRecordingBranchOfGstPipeline() {
    printf("stopRecordingBranchFromGstPipeline\n");

    if (!rec.queue) {
        printf("Recording queue is null.\n");
        return false;
    }

    gst_element_send_event(rec.queue, gst_event_new_eos());

    GstBus* bus = gst_element_get_bus(pipeline);
    bool eosReceived = false;
    bool success = false;

    while (!eosReceived) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        GstMessage* msg = gst_bus_timed_pop_filtered(
            bus,
            GST_CLOCK_TIME_NONE,
            static_cast<GstMessageType>(GST_MESSAGE_EOS | GST_MESSAGE_ERROR)
        );

        if (msg) {
            const gchar* src_name = GST_OBJECT_NAME(GST_MESSAGE_SRC(msg));

            switch (GST_MESSAGE_TYPE(msg)) {
                case GST_MESSAGE_EOS:
                    printf("EOS received from: %s\n", src_name);
                    eosReceived = true;
                    success = true;
                    break;
                case GST_MESSAGE_ERROR:
                    GError* err;
                    gchar* debug;
                    gst_message_parse_error(msg, &err, &debug);
                    printf("Error during stop: %s\n", err->message);
                    g_error_free(err);
                    g_free(debug);
                    eosReceived = true; // Exit on error
                    break;
                default:
                    break;
            }
            gst_message_unref(msg);
        }
    }
    gst_object_unref(bus);

    unlinkRecordingBranchFromGstPipeline();
    restartPipeline();
    printf("stopRecordingBranchFromGstPipeline END\n");
    return success;
}

void Video::unlinkRecordingBranchFromGstPipeline() {
    printf("unlinkRecordingBranchFromGstPipeline\n");

    // Unlink tee -> queue
    GstPad* queue_sink_pad = gst_element_get_static_pad(rec.queue, "sink");
    gst_pad_unlink(rec.tee_src_pad, queue_sink_pad);
    gst_object_unref(queue_sink_pad);

    // Set all elements to NULL state
    gst_element_set_state(rec.filesink, GST_STATE_NULL);
    gst_element_set_state(rec.muxer, GST_STATE_NULL);
    gst_element_set_state(rec.parser, GST_STATE_NULL);
    gst_element_set_state(rec.encoder, GST_STATE_NULL);
    gst_element_set_state(rec.queue, GST_STATE_NULL);

    // Remove from pipeline
    gst_bin_remove_many(GST_BIN(pipeline), rec.queue, rec.encoder, rec.parser, rec.muxer, rec.filesink, NULL);

    // Release tee src pad
    GstElement* tee = gst_bin_get_by_name(GST_BIN(pipeline), "t");
    gst_element_release_request_pad(tee, rec.tee_src_pad);
    gst_object_unref(rec.tee_src_pad);
    gst_object_unref(tee);

    // Reset the rec struct
    rec = {};
}

void Video::startGstPipeline() {
    printf("startGstPipeline\n");
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    resetFrozenCheck();
}

void Video::stopGstPipeline() const {
    printf("stopGstPipeline\n");
    gst_element_send_event(pipeline, gst_event_new_eos());

    GstBus* bus = gst_element_get_bus(pipeline);
    GstMessage* msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));

    if (msg) {
        gst_message_unref(msg);
    }

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
}

void Video::restartPipeline() const {
    gst_element_set_state(pipeline, GST_STATE_NULL);

    gst_element_seek_simple(
        pipeline,
        GST_FORMAT_TIME,
        (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
        0
    );

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void Video::initOpenGLResources() {
    // --- Create Y texture (luminance plane) ---
    glGenTextures(1, &yTextureID);
    glBindTexture(GL_TEXTURE_2D, yTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Constants::CameraImageWidth, Constants::CameraImageHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

    // --- Create UV texture (chrominance plane) ---
    glGenTextures(1, &uvTextureID);
    glBindTexture(GL_TEXTURE_2D, uvTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, Constants::CameraImageWidth / 2, Constants::CameraImageHeight / 2, 0, GL_RG, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);

    // --- Create VAO/VBO for rendering quad ---
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    // Assume 'vertices' contains interleaved position (3 floats) and tex coords (2 floats)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_vflip), vertices_vflip, GL_STATIC_DRAW);

    // Attribute 0: position (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Attribute 1: tex coords (vec2)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glResourcesInitialized = true;
}



#ifdef USE_GLES
const char *vertexShaderSource = R"(
#version 300 es
precision mediump float;
in vec3 position;
in vec2 texCoord;
out vec2 fragTexCoord;
void main() {
    gl_Position = vec4(position, 1.0);
    fragTexCoord = texCoord;
})";
const char *fragmentShaderSource = R"(
#version 300 es
precision mediump float;

in vec2 fragTexCoord;
out vec4 color;

uniform sampler2D textureY;
uniform sampler2D textureUV;

uniform float whiteBalanceR;
uniform float whiteBalanceG;
uniform float whiteBalanceB;

void main() {
    // Sample Y (luminance) texture
    float Y = texture(textureY, fragTexCoord).r;
    
    // Sample UV (chrominance) texture
    vec2 UV = texture(textureUV, fragTexCoord).rg;
    
    // Decode UV values from the [0, 1] range to the [-0.5, 0.5] range
    float U = UV.r - 0.5;
    float V = UV.g - 0.5;
    
    // Convert YUV to RGB using the standard formula
    float R = Y + 1.402 * V;
    float G = Y - 0.344136 * U - 0.714136 * V;
    float B = Y + 1.772 * U;

    // Apply white balance
    R *= whiteBalanceR;
    G *= whiteBalanceG;
    B *= whiteBalanceB;
    
    // Output the final RGB color
    color = vec4(R, G, B, 1.0);
}
)";
#else
const char *vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
out vec2 fragTexCoord;
void main() {
    gl_Position = vec4(position, 1.0);
    fragTexCoord = texCoord;
}
)";

const char *fragmentShaderSource = R"(
#version 330 core
in vec2 fragTexCoord;
out vec4 color;

uniform sampler2D textureY;
uniform sampler2D textureUV;

uniform float whiteBalanceR;
uniform float whiteBalanceG;
uniform float whiteBalanceB;

void main() {
    float Y = texture(textureY, fragTexCoord).r;
    vec2 UV = texture(textureUV, fragTexCoord).rg;
    
    float U = UV.r - 0.5;
    float V = UV.g - 0.5;

    float R = Y + 1.402 * V;
    float G = Y - 0.344136 * U - 0.714136 * V;
    float B = Y + 1.772 * U;

    R *= whiteBalanceR;
    G *= whiteBalanceG;
    B *= whiteBalanceB;

    color = vec4(R, G, B, 1.0);
}
)";
#endif

unsigned int Video::createShader(unsigned int type, const char *source) const {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader Compilation Error: " << infoLog << std::endl;
    }

    return shader;
}

unsigned int Video::createShaderProgram() const {
    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader Program Linking Error: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void Video::initWhiteBalanceFactors() {
    glUseProgram(shaderProgram);
    locR = glGetUniformLocation(shaderProgram, "whiteBalanceR");
    locG = glGetUniformLocation(shaderProgram, "whiteBalanceG");
    locB = glGetUniformLocation(shaderProgram, "whiteBalanceB");
    glUniform1f(locR, 1.05);
    glUniform1f(locG, 0.95);
    glUniform1f(locB, 1.05);
}

void Video::render() {
    if (yTextureData.size() == 0) {
        return;
    }

    sampleRenderCount++;

    if (checkFrozen()) {
        whiteBalanceUpdateRequest = false;
    }

    if (!glResourcesInitialized) {
        return;
    }
    
    if (whiteBalanceUpdateRequest) {
        applyWhiteBalance();
        whiteBalanceUpdateRequest = false;
    }

    std::lock_guard<std::mutex> lock(textureMutex);

    glBindTexture(GL_TEXTURE_2D, yTextureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Constants::CameraImageWidth, Constants::CameraImageHeight, GL_RED, GL_UNSIGNED_BYTE, yTextureData.data());
    
    glBindTexture(GL_TEXTURE_2D, uvTextureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Constants::CameraImageWidth / 2, Constants::CameraImageHeight / 2, GL_RG, GL_UNSIGNED_BYTE, uvTextureData.data());

    glUseProgram(shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, yTextureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureY"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, uvTextureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureUV"), 1);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Video::snapshot() {
    #ifdef SIMULATE_ALL_HARDWARE
    return true;
    #else

    snapshotFilename = FileUtils::getInstance().getTimestampedFilename("frame_cam" + std::to_string(activeCameraId), "png");

    if (snapshotFilename.empty()) {
        MessageHandler::getInstance().set(Message::MessageType::NoUsbStorage);
        ErrorHandler::getInstance().addError(ErrorCodes::NoUsbStorage); //TODO: for testing
        return false;
    }

    snapshotRequest = true;
    return true;
    #endif
}

bool Video::prepareRecording() {
    printf("prepareRecording\n");
    videoFilename = FileUtils::getInstance().getTimestampedFilename("video_cam" + std::to_string(activeCameraId), "mp4");

    if (videoFilename.empty()) {
        MessageHandler::getInstance().set(Message::MessageType::NoUsbStorage);
        ErrorHandler::getInstance().addError(ErrorCodes::NoUsbStorage); //TODO: for testing
        return false;
    }

    return true;
}

bool Video::startRecording() {
    printf("startRecording\n");
    recordingState = RecordingState::Recording;
    bool success = linkRecordingBranchToGstPipeline();
    return success;
}

void Video::stopRecording() {
    printf("stopRecording\n");
    recordingState = RecordingState::Finishing;
}

bool Video::finishRecording() {
    bool success = true;

    if (stopRecordingBranchOfGstPipeline()) {
        FileUtils::getInstance().flushFile(videoFilename);
    } else {
        printf("Failed to stop recording branch of GStreamer pipeline.\n");
        success = false;
    }

    recordingState = RecordingState::SwitchingBackToNormalStream;
    return success;
}

bool Video::isRecording() const {
    return recordingState != RecordingState::Idle;
}

bool Video::checkFrozen() {
    #ifdef SIMULATE_ALL_HARDWARE
    return false;
    #endif

    #ifdef DEBUG
    if (simulateVideoFreeze) {
        setFrozen(true);
        return true;
    }
    #endif

    if (ptsUnchanged) {
        printf("Video frozen: ptsUnchanged\n");
        setFrozen(true);
        return true;
    }

    if (!isRecording()) { // TODO: Somehow this check does not work while recording video
        if (sampleRenderCount > Constants::SampleRenderCountThreshold) {
            printf("Video frozen: sampleRenderCount > sampleRenderCountThreshold\n");
            setFrozen(true);
            return true;
        }
    }

    setFrozen(false);
    return false;
}

void Video::setFrozen(bool frozen) {
    if (videoFrozen == frozen) {
        return;
    }

    if (frozen) {
        MessageHandler::getInstance().set(Message::MessageType::FrozenFrame);
        ErrorHandler::getInstance().addError(ErrorCodes::VideoFreeze);
        videoFrozen = true;
        frozenTime = std::chrono::high_resolution_clock::now();
    } else {
        printf("Video back to normal\n");
        // Remove frozen message when rendering is back to normal
        MessageHandler::getInstance().remove(Message::MessageType::FrozenFrame);
        ErrorHandler::getInstance().removeError(ErrorCodes::VideoFreeze);
        videoFrozen = false;
    }
}

void Video::startWhiteBalance() {
    whiteBalance->start();
    whiteBalanceSkipFramesCounter = 0;
    whiteBalanceUpdateRequest = true;
}

void Video::applyWhiteBalance() {
    glUseProgram(shaderProgram);
    glUniform1f(locR, whiteBalance->getRed());
    glUniform1f(locG, whiteBalance->getGreen());
    glUniform1f(locB, whiteBalance->getBlue());
}

bool Video::frozenForLong() {
    if (videoFrozen) {
        if (Utils::timePassedSince(frozenTime) > Constants::FrozenTimeThreshold) {
            frozenTime = std::chrono::high_resolution_clock::now(); // Reset frozen time so it won't be triggered again immediately

            #ifdef DEBUG
            simulateVideoFreeze = false;
            #endif

            return true;
        }
    }

    return false;
}

void Video::resetFrozenCheck() {
    ptsEqualCount = 0;
    ptsUnchanged = false;
    sampleRenderCount = 0;
    videoFrozen = false;
}

void Video::switchCamera(int newCameraId) {
    setActiveCamera(newCameraId);

    if (newCameraId == 1) {
        whiteBalance = &whiteBalance1;
    } else {
        whiteBalance = &whiteBalance2;
    }

    applyWhiteBalance();
}

void Video::setActiveCamera(int cameraId) {
    activeCameraId = cameraId;
}

#ifdef DEBUG
void Video::toggleSimulateVideoFreeze() {
    simulateVideoFreeze = !simulateVideoFreeze;
    printf("Simulate Video Freeze: %s\n", simulateVideoFreeze ? "ON" : "OFF");
}
#endif
