/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "GmslConnection.h"
#include "Constants.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

const struct RegValue GmslConnection::serializerSetting[] = {
	
	// This script is validated on:
	// MAX96717
	// MAX96716B
	// Please refer to the Errata sheet for each device.
	// ---------------------------------------------------------------------------------
	
	//
	// CSIConfigurationTool
	//
	// GMSL-A / Serializer: MAX96717 (Pixel Mode) / Mode: 1x4 / Device Address: 0x84 / Multiple-VC Case: Single VC / Pipe Sharing: Separate Pipes
	// PipeZ:
	// Input Stream: VC0 RAW10 PortB (D-PHY) (Doubled)

	// Deserializer: MAX96716B / Mode: 2 (1x4) / Device Address: 0x50
	// PipeY:
	// GMSL-A Input Stream: VC0 RAW10 PortB - Output Stream: VC0 RAW10 PortA (D-PHY)

	// Video Transmit Configuration for Serializer(s)
 	//{0x84,0x0002,0x03}, // DEV : REG2 | VID_TX_EN_Z (VID_TX_EN_Z): Disabled
	//
	// INSTRUCTIONS FOR GMSL-A SERIALIZER MAX96717
	//
	// MIPI D-PHY Configuration
 	{0x84,0x0330,0x00}, // MIPI_RX : MIPI_RX0 | (Default) RSVD (Port Configuration): 1x4
 	{0x84,0x0383,0x80}, // MIPI_RX_EXT : EXT11 | Tun_Mode (Tunnel Mode): Enabled
 	{0x84,0x0331,0x10}, // MIPI_RX : MIPI_RX1 | ctrl1_num_lanes (Port B - Lane Count): 2
 	{0x84,0x0332,0xE0}, // MIPI_RX : MIPI_RX2 | (Default) phy1_lane_map (Lane Map - PHY1 D0): Lane 2 | (Default) phy1_lane_map (Lane Map - PHY1 D1): Lane 3
 	{0x84,0x0333,0x04}, // MIPI_RX : MIPI_RX3 | (Default) phy2_lane_map (Lane Map - PHY2 D0): Lane 0 | (Default) phy2_lane_map (Lane Map - PHY2 D1): Lane 1
 	{0x84,0x0334,0x00}, // MIPI_RX : MIPI_RX4 | (Default) phy1_pol_map (Polarity - PHY1 Lane 0): Normal | (Default) phy1_pol_map (Polarity - PHY1 Lane 1): Normal
 	{0x84,0x0335,0x00}, // MIPI_RX : MIPI_RX5 | (Default) phy2_pol_map (Polarity - PHY2 Lane 0): Normal | (Default) phy2_pol_map (Polarity - PHY2 Lane 1): Normal | (Default) phy2_pol_map (Polarity - PHY2 Clock Lane): Normal
	// Controller to Pipe Mapping Configuration
 	{0x84,0x0308,0x64}, // FRONTTOP : FRONTTOP_0 | (Default) RSVD (CLK_SELZ): Port B | (Default) START_PORTB (START_PORTB): Enabled
 	{0x84,0x0311,0x40}, // FRONTTOP : FRONTTOP_9 | (Default) START_PORTBZ (START_PORTBZ): Start Video
 	{0x84,0x0318,0x6B}, // FRONTTOP : FRONTTOP_16 | mem_dt1_selz (mem_dt1_selz): 0x6B
	// Double Mode Configuration
 	{0x84,0x0313,0x04}, // FRONTTOP : FRONTTOP_11 | bpp10dblz (bpp10dblz): Send 10-bit pixels as 20-bit
 	{0x84,0x031E,0x34}, // FRONTTOP : FRONTTOP_22 | soft_bppz (soft_bppz): 0x14 | soft_bppz_en (soft_bppz_en): Software override enabled
	// Pipe Configuration
 	{0x84,0x005B,0x00}, // CFGV__VIDEO_Z : TX3 | TX_STR_SEL (TX_STR_SEL Pipe Z): 0x0
};

const struct RegValue GmslConnection::deserializerSetting[] = {
	//
	// INSTRUCTIONS FOR DESERIALIZER MAX96716B
	//
	// Pipe to Controller Mapping Configuration
 	{0x50,0x044B,0x07}, // MIPI_TX__1 : MIPI_TX11 | MAP_EN_L (MAP_EN_L Pipe Y): 0x7
 	{0x50,0x044C,0x00}, // MIPI_TX__1 : MIPI_TX12 | (Default) MAP_EN_H (MAP_EN_H Pipe Y): 0x0
 	{0x50,0x044D,0x2B}, // MIPI_TX__1 : MIPI_TX13 | MAP_SRC_0 (MAP_SRC_0 Pipe Y DT): 0x2B | (Default) MAP_SRC_0 (MAP_SRC_0 Pipe Y VC): 0x0
 	{0x50,0x044E,0x2B}, // MIPI_TX__1 : MIPI_TX14 | MAP_DST_0 (MAP_DST_0 Pipe Y DT): 0x2B | (Default) MAP_DST_0 (MAP_DST_0 Pipe Y VC): 0x0
 	{0x50,0x044F,0x00}, // MIPI_TX__1 : MIPI_TX15 | (Default) MAP_SRC_1 (MAP_SRC_1 Pipe Y DT): 0x0 | (Default) MAP_SRC_1 (MAP_SRC_1 Pipe Y VC): 0x0
 	{0x50,0x0450,0x00}, // MIPI_TX__1 : MIPI_TX16 | (Default) MAP_DST_1 (MAP_DST_1 Pipe Y DT): 0x0 | (Default) MAP_DST_1 (MAP_DST_1 Pipe Y VC): 0x0
 	{0x50,0x0451,0x01}, // MIPI_TX__1 : MIPI_TX17 | MAP_SRC_2 (MAP_SRC_2 Pipe Y DT): 0x1 | (Default) MAP_SRC_2 (MAP_SRC_2 Pipe Y VC): 0x0
 	{0x50,0x0452,0x01}, // MIPI_TX__1 : MIPI_TX18 | MAP_DST_2 (MAP_DST_2 Pipe Y DT): 0x1 | (Default) MAP_DST_2 (MAP_DST_2 Pipe Y VC): 0x0
 	{0x50,0x046D,0x15}, // MIPI_TX__1 : MIPI_TX45 | MAP_DPHY_DEST_0 (MAP_DPHY_DST_0 Pipe Y): 0x1 | MAP_DPHY_DEST_1 (MAP_DPHY_DST_1 Pipe Y): 0x1 | MAP_DPHY_DEST_2 (MAP_DPHY_DST_2 Pipe Y): 0x1
	// Double Mode Configuration
 	{0x50,0x0473,0x04}, // MIPI_TX__1 : MIPI_TX51 | ALT_MEM_MAP10 (ALT_MEM_MAP10 CTRL1): Alternate memory map enabled
	// MIPI D-PHY Configuration
 	{0x50,0x0330,0x04}, // MIPI_PHY : MIPI_PHY0 | (Default) phy_4x2 (Port Configuration): 2 (1x4)
 	{0x50,0x044A,0x50}, // MIPI_TX__1 : MIPI_TX10 | CSI2_LANE_CNT (Port A - Lane Count): 2
 	{0x50,0x0333,0x4E}, // MIPI_PHY : MIPI_PHY3 | (Default) phy0_lane_map (Lane Map - PHY0 D0): Lane 2 | (Default) phy0_lane_map (Lane Map - PHY0 D1): Lane 3 | (Default) phy1_lane_map (Lane Map - PHY1 D0): Lane 0 | (Default) phy1_lane_map (Lane Map - PHY1 D1): Lane 1
 	{0x50,0x0335,0x00}, // MIPI_PHY : MIPI_PHY5 | (Default) phy0_pol_map (Polarity - PHY0 Lane 0): Normal | (Default) phy0_pol_map (Polarity - PHY0 Lane 1): Normal | (Default) phy1_pol_map (Polarity - PHY1 Lane 0): Normal | (Default) phy1_pol_map (Polarity - PHY1 Lane 1): Normal | (Default) phy1_pol_map (Polarity - PHY1 Clock Lane): Normal
 	{0x50,0x1D00,0xF4}, // DPLL__CSI2 : DPLL_0 | config_soft_rst_n (config_soft_rst_n - PHY1): 0x0
	// This is to set predefined (coarse) CSI output frequency
	// CSI Phy 1 is 1000 Mbps/lane.
 	{0x50,0x1D00,0xF4}, // DPLL__CSI2 : DPLL_0 | (Default)
 	{0x50,0x0320,0x2A},
 	{0x50,0x1D00,0xF5}, // DPLL__CSI2 : DPLL_0 |  | (Default) config_soft_rst_n (config_soft_rst_n - PHY1): 0x1
 	{0x50,0x0332,0x00}, // MIPI_PHY : MIPI_PHY2 | phy_Stdby_n (phy_Stdby_2): Put PHY2 in standby mode | phy_Stdby_n (phy_Stdby_3): Put PHY3 in standby mode
    {0x50,0x0474,0x09}, // Tunnel mode Link A: enabled
    {0x50,0x04B4,0x09}, // Tunnel mode Link B: enabled
 	//{0x50,0x0313,0x02}, // BACKTOP : BACKTOP12 | CSI_OUT_EN (CSI_OUT_EN): CSI output enabled
	// Video Transmit Configuration for Serializer(s)
 	//{0x84,0x0002,0x43}, // DEV : REG2 | VID_TX_EN_Z (VID_TX_EN_Z): Enabled
};

const struct RegValue GmslConnection::deserializerTestPattern[] = {
    {0x50,0x0240,0x13},
    {0x50,0x0242,0x00},
    {0x50,0x0243,0x00},
    {0x50,0x0244,0x00},
    {0x50,0x0245,0x00},
    {0x50,0x0246,0x50},
    {0x50,0x0247,0x78},
    {0x50,0x0248,0x8A},
    {0x50,0x0249,0x4E},
    {0x50,0x024A,0x40},
    {0x50,0x024B,0x00},
    {0x50,0x024C,0x00},
    {0x50,0x024D,0x00},
    {0x50,0x024E,0x00},
    {0x50,0x024F,0x2C},
    {0x50,0x0250,0x0F},
    {0x50,0x0251,0xEC},
    {0x50,0x0252,0x08},
    {0x50,0x0253,0x9D},
    {0x50,0x0254,0x02},
    {0x50,0x0255,0x94},
    {0x50,0x0256,0x98},
    {0x50,0x0257,0x0F},
    {0x50,0x0258,0x00},
    {0x50,0x0259,0x01},
    {0x50,0x025A,0x18},
    {0x50,0x025B,0x08},
    {0x50,0x025C,0x70},
    {0x50,0x0241,0x20},
    {0x50,0x025D,0x04},
    {0x50,0x01FC,0x00},
    {0x50,0x0038,0x01},
    {0x50,0x0240,0Xf3},
};

const struct RegValue GmslConnection::cameraSetting[] = {
    {0x6c,0x0302,0x1b},
    {0x6c,0x030d,0x1b},
    {0x6c,0x030e,0x02},
    {0x6c,0x0312,0x01},
    {0x6c,0x3000,0x00},
    {0x6c,0x3018,0x32},
    {0x6c,0x3031,0x0a}, //10-bit mode
    {0x6c,0x3080,0x08},
    {0x6c,0x3083,0xB4},
    {0x6c,0x3103,0x00}, //not used?
    {0x6c,0x3104,0x01}, //not used?
    {0x6c,0x3106,0x01}, //SRB host input dis
    //{0x6c,0x3500,0x03}, //exposure
    //{0x6c,0x3501,0x0D}, //exposure
    //{0x6c,0x3502,0x40}, //exposure
    {0x6c,0x3503,0x88}, //AEC control, gain/exposure settings
    {0x6c,0x3507,0x00}, //AEC gain shift
    //{0x6c,0x3508,0x0D}, //gain
    //{0x6c,0x3509,0xAC}, //gain
    //{0x6c,0x350c,0x00}, //short gain
    //{0x6c,0x350d,0x80}, //short gain
    //{0x6c,0x3510,0x00}, //short exposure
    //{0x6c,0x3511,0x00}, //short exposure
    //{0x6c,0x3512,0x20}, //short exposure
    {0x6c,0x3632,0x00}, //Analog control registers from 3600 to 366F
    {0x6c,0x3633,0x10},
    {0x6c,0x3634,0x10},
    {0x6c,0x3635,0x10},
    {0x6c,0x3645,0x13},
    {0x6c,0x3646,0x81},
    {0x6c,0x3636,0x10},
    {0x6c,0x3651,0x0a},
    {0x6c,0x3656,0x02},
    {0x6c,0x3659,0x04},
    {0x6c,0x365a,0xda},
    {0x6c,0x365b,0xa2},
    {0x6c,0x365c,0x04},
    {0x6c,0x365d,0x1d},
    {0x6c,0x365e,0x1a},
    {0x6c,0x3662,0xd7},
    {0x6c,0x3667,0x78},
    {0x6c,0x3669,0x0a},
    {0x6c,0x366a,0x92},
    {0x6c,0x3700,0x54}, //Sensor control registers from 3700 to 37CF
    {0x6c,0x3702,0x10},
    {0x6c,0x3706,0x42},
    {0x6c,0x3709,0x30},
    {0x6c,0x370b,0xc2},
    {0x6c,0x3714,0x63},
    {0x6c,0x3715,0x01},
    {0x6c,0x3716,0x00},
    {0x6c,0x371a,0x3e},
    {0x6c,0x3732,0x0e},
    {0x6c,0x3733,0x10},
    {0x6c,0x375f,0x0e},
    {0x6c,0x3768,0x30},
    {0x6c,0x3769,0x44},
    {0x6c,0x376a,0x22},
    {0x6c,0x377b,0x20},
    {0x6c,0x377c,0x00},
    {0x6c,0x377d,0x0c},
    {0x6c,0x3798,0x00},
    {0x6c,0x37a1,0x55},
    {0x6c,0x37a8,0x6d},
    {0x6c,0x37c2,0x04},
    {0x6c,0x37c5,0x00},
    {0x6c,0x37c8,0x00},
    {0x6c,0x3800,0x00}, //Timing control from 3800 to 382B
    {0x6c,0x3801,0x00},
    {0x6c,0x3802,0x00},
    {0x6c,0x3803,0x04},
    {0x6c,0x3804,0x07},
    {0x6c,0x3805,0x8f},
    {0x6c,0x3806,0x04},
    {0x6c,0x3807,0x43},
    {0x6c,0x3808,0x07},
    {0x6c,0x3809,0x80},
    {0x6c,0x380a,0x04},
    {0x6c,0x380b,0x38},
    {0x6c,0x380c,0x04},
    {0x6c,0x380d,0x38},
    {0x6c,0x380e,0x04},
    {0x6c,0x380f,0x60},
    {0x6c,0x3810,0x00},
    {0x6c,0x3811,0x08},
    {0x6c,0x3812,0x00},
    {0x6c,0x3813,0x04},
    {0x6c,0x3814,0x01},
    {0x6c,0x3815,0x01},
    {0x6c,0x3820,0x80},
    {0x6c,0x3821,0x40},
    {0x6c,0x3822,0x84},
    {0x6c,0x3829,0x00},
    {0x6c,0x382a,0x01},
    {0x6c,0x382b,0x01},
    {0x6c,0x3830,0x04},
    {0x6c,0x3836,0x01},
    {0x6c,0x3837,0x08},
    {0x6c,0x3839,0x01},
    {0x6c,0x383a,0x00},
    {0x6c,0x383b,0x08},
    {0x6c,0x383c,0x00},
    {0x6c,0x3f0b,0x00},
    {0x6c,0x4001,0x20}, //Black level settings
    {0x6c,0x4009,0x07}, //Black line end, default=0x0B
    {0x6c,0x4003,0x10}, //Black level target
    {0x6c,0x4010,0xe0}, //Black level settings, default=0x60
    {0x6c,0x4016,0x00}, //Offset trigger threshold
    {0x6c,0x4017,0x10}, //Offset trigger threshold, default=0x00
    {0x6c,0x4044,0x02}, //r_md_gain_th
    {0x6c,0x4304,0x08},
    {0x6c,0x4307,0x30},
    {0x6c,0x4320,0x80}, //test pattern control
    {0x6c,0x4322,0x00}, //solid color B
    {0x6c,0x4323,0x00}, //solid color B
    {0x6c,0x4324,0x00}, //solid color GB
    {0x6c,0x4325,0x00}, //solid color GB
    {0x6c,0x4326,0x00}, //solid color R
    {0x6c,0x4327,0x00}, //solid color R
    {0x6c,0x4328,0x00}, //solid color GR
    {0x6c,0x4329,0x00}, //solid color GR
    {0x6c,0x432c,0x03},
    {0x6c,0x432d,0x81},
    {0x6c,0x4501,0x84},
    {0x6c,0x4502,0x40},
    {0x6c,0x4503,0x18},
    {0x6c,0x4504,0x04},
    {0x6c,0x4508,0x02},
    {0x6c,0x4601,0x10},
    {0x6c,0x4800,0x00}, //MIPI control, default=0x4c
    {0x6c,0x4816,0x52}, //embedded line data
    {0x6c,0x4837,0x15}, //PCLK period, default=0x1A
    {0x6c,0x5000,0x7f}, //ISP control00, default=0xBF
    {0x6c,0x5001,0x00}, //ISP control01, avg_size_man_en, default=0x01
    {0x6c,0x5005,0x38}, //DSP control, contains Pre_awb average enable, DCBLC enable, long/short reverse for AWB gain, default=0x28
    {0x6c,0x501e,0x0d}, //AWB gain bias manual (for AWB before DPC), default=0x10
    {0x6c,0x5040,0x00}, //Test pattern settings
    {0x6c,0x5901,0x00}, //VAP control, Hskip, Vskip
    //{0x6c,0x0100,0x00}, //Stream mode
};

GmslConnection::GmslConnection() : connected(false), selectedCamera(0), pauseReconnection(false) {
}

void GmslConnection::initCamera() {
	CameraRegisterAccess::getInstance().writeTable(cameraSetting, ARRAY_SIZE(cameraSetting));
}

void GmslConnection::checkConnection() {
    if (pauseReconnection) {
        return;
    }
    
    u8 read_value;
    int ret = CameraRegisterAccess::getInstance().readRegister(0x302, &read_value);

    if (ret == RET_FAIL) {
        connected = false;
        return;
    }

    if (!checkVideoLock(false)) {
        connected = false;
        return;
    }

    connected = true;
}

bool GmslConnection::connect() {
    const int defaultSleepTime = 20;
    int ret = RET_FAIL;
    int retries = 0;

    while (retries < Constants::MaximumNumberOfRetries) {
        if (retries % 5 == 0) {
            printf("Trying to connect camera...\n");
        }

        retries++;

        if (!pauseReconnection) {
            std::this_thread::sleep_for(Constants::ReconnectionWaitTime);
        }

        stopStream();
        disableCsiOutput();
        disableVideoTx();

        ret = CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50,0x031E,0x34);
        if(RET_FAIL == ret){
            continue;
        }

        if (!activateCamera()) {
            continue;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(defaultSleepTime));

        ret = CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50,0x0011,0x0F);
        if(RET_FAIL == ret){
            continue;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(defaultSleepTime));

        if (!resetLink()) {
            continue;
        }

        if (!waitForGmslLink()) {
            continue;
        }

        setVideoPipeSelection();
        CameraRegisterAccess::getInstance().writeTable(deserializerSetting, ARRAY_SIZE(deserializerSetting));

        std::this_thread::sleep_for(std::chrono::milliseconds(defaultSleepTime));

        ret = CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x84,0x0001,0x04);
        if(RET_FAIL == ret){
            printf("0x84 not active\n");
            continue;
        }
        ret = CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x84,0x0011,0x03);
        if(RET_FAIL == ret){
            continue;
        }
        ret = CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x84,0x02BE,0x80);
        if(RET_FAIL == ret){
            continue;
        }
        ret = CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x84,0x0003,0x03);
        if(RET_FAIL == ret){
            continue;
        }
        ret = CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x84,0x0006,0xb1);
        if(RET_FAIL == ret){
            continue;
        }
        ret = CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x84,0x03f0,0x51);
        if(RET_FAIL == ret){
            continue;
        }
        ret = CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x84,0x0570,0x0c);
        if(RET_FAIL == ret){
            continue;
        }

        CameraRegisterAccess::getInstance().writeTable(serializerSetting, ARRAY_SIZE(serializerSetting));

        std::this_thread::sleep_for(std::chrono::milliseconds(defaultSleepTime));
        ret = CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x84,0x02be,0x90);
        if(RET_FAIL == ret){
            continue;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(defaultSleepTime));
        
        initCamera();
        enableVideoTx();
        startStream();
        enableCsiOutput();
        
        u8 read_value;
        ret = CameraRegisterAccess::getInstance().readRegister(0x302,&read_value);
        if(RET_FAIL == ret){
            continue;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(defaultSleepTime));

        if (!checkVideoLock()) {
            continue;
        }

        printf("connection finished\n");
        return true;
    }

    printf( "Error: unable to connect camera\n");
    return false;
}

void GmslConnection::disconnect() {
    pauseReconnection = true;
    stopStream();
    disableCsiOutput();
    disableVideoTx();
    pauseReconnection = false;
}

void GmslConnection::keepConnection() {
    while (keepRunning) {
        std::this_thread::sleep_for(Constants::ConnectionCheckInterval);

        if (pauseReconnection) {
            continue;
        }

        if (!connected) {
            bool success = connect();

            if (!success) {
                pause();
            }
        }

        checkConnection();
    }
}

void GmslConnection::startThread() {
    #ifdef SIMULATE_ALL_HARDWARE
    return;
    #endif
    
    keepRunning = true;
    reconnectionThread = std::thread(&GmslConnection::keepConnection, this);
}

void GmslConnection::stopThread() {
    #ifdef SIMULATE_ALL_HARDWARE
    return;
    #endif

    printf("Stopping connection thread...\n");
    keepRunning = false;

    if (reconnectionThread.joinable()) {
        reconnectionThread.join();
    }

    printf("Connection thread was stopped.\n");
}

bool GmslConnection::isConnected() {
    #ifdef SIMULATE_ALL_HARDWARE
    return true;
    #endif

    checkConnection();

    return connected;
}

void GmslConnection::setVideoPipeSelection() {
    if (selectedCamera == 1) {
        CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50, 0x0161, 0x30); // Pipe Y - Link A stream 0
    } else if (selectedCamera == 2) {
        CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50, 0x0161, 0x34); // Pipe Y - Link B stream 0
    }
}

bool GmslConnection::resetLink() {
    int ret;

    if (selectedCamera == 1) {
        ret = CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50, 0x0010, 0xE1);
    } else {
        ret = CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50, 0x0010, 0xE2);
    }

    return ret == RET_OK;
}

bool GmslConnection::activateCamera() {
    if (selectedCamera == 1) {
        if (CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50, 0x0003, 0x00) == RET_FAIL) { // Disable Link B
            return false;
        }
        if (CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50, 0x0001, 0x01) == RET_FAIL) { // Enable Link A
            return false;
        }
        if (CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50, 0x0F00, 0x01) == RET_FAIL) { // Select Link A only
            return false;
        }
    } else {
        if (CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50, 0x0001, 0x00) == RET_FAIL) { // Disable Link A
            return false;
        }
        if (CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50, 0x0003, 0x01) == RET_FAIL) { // Enable Link B
            return false;
        }
        if (CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50, 0x0F00, 0x10) == RET_FAIL) { // Select Link B only
            return false;
        }
    }

    return true;
}

void GmslConnection::switchCamera(int newCameraId) {
    pauseReconnection = true;
    selectedCamera = newCameraId;
    connect();
    pauseReconnection = false;
}

void GmslConnection::resetCamera() {
    pauseReconnection = true;
    connect();
    pauseReconnection = false;
}

void GmslConnection::enableVideoTx() {
    CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x84, 0x0002, 0x43);
}

void GmslConnection::disableVideoTx() {
    CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x84, 0x0002, 0x03);
}

void GmslConnection::startStream() {
    enableMipiPhyLanes();
    CameraRegisterAccess::getInstance().writeRegister(0x0100, 0x01);
}

void GmslConnection::stopStream() {
    CameraRegisterAccess::getInstance().writeRegister(0x0100, 0x00);
    disableMipiPhyLanes();
}

void GmslConnection::enableMipiPhyLanes() {
    CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50, 0x0332, 0x30);
}

void GmslConnection::disableMipiPhyLanes() {
    CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50, 0x0332, 0x00);
}

void GmslConnection::enableCsiOutput() {
    CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50,0x0313,0x02);
}

void GmslConnection::disableCsiOutput() {
    CameraRegisterAccess::getInstance().writeRegWithSlaveAddr(0x50,0x0313,0x00);
}

bool GmslConnection::waitForGmslLink() {
    u8 valueA;
    u8 valueB;
    int retA = RET_FAIL;
    int retB = RET_FAIL;
    bool linkA = false;
    bool linkB = false;

    for (int i = 0; i < 20; i++) {
        retA = CameraRegisterAccess::getInstance().readRegisterWithSlaveAddr(0x50, 0x0013, &valueA);
        
        if (retA == RET_OK) {
            if (valueA & 0x08) {
                linkA = true;
                break;
            }
        }

        retB = CameraRegisterAccess::getInstance().readRegisterWithSlaveAddr(0x50, 0x5009, &valueB);
        
        if (retB == RET_OK) {
            if (valueB & 0x08) {
                linkB = true;
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    if (linkA) {
        printf("GMSL link A is active\n");
        return true;
    }
    
    if (linkB) {
        printf("GMSL link B is active\n");
        return true;
    }

    bool active = linkA || linkB;
    
    if (!active) {
        printf("GMSL link not active\n");
    }

    return active;
}

bool GmslConnection::checkVideoLock(bool print) {
    u8 value;
    int retY = RET_FAIL;
    int retZ = RET_FAIL;
    bool pipeY = false;
    bool pipeZ = false;

    retY = CameraRegisterAccess::getInstance().readRegisterWithSlaveAddr(0x50, 0x01FC, &value);
        
    if (retY == RET_OK) {
        if (value & 0x01) {
            pipeY = true;

            if (print) {
                printf("Pipe Y is locked\n");
            }
        }
    }

    retZ = CameraRegisterAccess::getInstance().readRegisterWithSlaveAddr(0x50, 0x021C, &value);
    
    if (retZ == RET_OK) {
        if (value & 0x01) {
            pipeZ = true;

            if (print) {
                printf("Pipe Z is locked\n");
            }
        }
    }

    bool active = pipeY || pipeZ;
    
    if (!active && print) {
        printf("video pipe locked\n");
    }

    return active;
}

void GmslConnection::enableCameraTestPattern() {
    CameraRegisterAccess::getInstance().writeRegister(0x5040, 0x80);
}

void GmslConnection::defaultAllCameraRegisters() {
    CameraRegisterAccess::getInstance().writeRegister(0x0103, 0x01);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int GmslConnection::getSelectedCamera() const {
    return selectedCamera;
}

void GmslConnection::setSelectedCamera(int cameraId) {
    selectedCamera = cameraId;
}

void GmslConnection::pause() {
    if (!pauseReconnection) {
        printf("Connection::pause\n");
        pauseReconnection = true;
    }
}

void GmslConnection::resume() {
    if (pauseReconnection) {
        printf("Connection::resume\n");
        pauseReconnection = false;
    }
}
