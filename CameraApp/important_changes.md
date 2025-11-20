### 1. switch mit define für ImGUI mit Opengl ES

- siehe Readme.md und git history

### 2. Video.cpp und Video.h geändert um Texturen schneller zu rendern

- glTexImage2D → glTexSubImage2D (Upload schneller, kein Speicher neu alloziert)
- VAO/VBO nicht jedes Mal erstellen & löschen → nur einmal in init() anlegen, im render() einfach verwenden
- siehe und git history


### 3. Start Logo in Yocto Image geändert psplash-poky.png

-  in dir /media/onexip/1fca541d-9a09-4a96-9e1d-25b611db41dd/var-fsl-yocto/sources/meta-variscite-sdk-imx/recipes-core/psplash/files
- psplash-poky.png ausgetauscht
- in diesen file  0001-psplash-Change-colors-for-the-Variscite-Yocto-logo.patch 2 Farben geändert von Weiss auf 0x45,0x96,0xcb (Hellblau) 


### 4. Hintergrund Image  vom Yocto Image geändert

Direkt in der Variscite-Layer ändern

    🔧 Schneller, aber nicht updatesicher (geht bei Yocto-Updates evtl. verloren)

Schritt 1: Datei weston.in bearbeiten

Pfad:

YOCTO_FOLDER/sources/meta-variscite-*/recipes-graphics/wayland/weston-init/imx*/weston.in

👉 In dieser Datei suchst du den [shell]-Block, und fügst folgendes ein oder änderst vorhandene Zeilen:

```
[shell]
background-color=0xff0000ff
background-image=/etc/xdg/weston/wallpaper.png
background-type=centered
```

Beispiel: Blau als Hintergrundfarbe + zentriertes Bild
Schritt 2: Bild hinzufügen (wallpaper.png)

Platziere dein Bild in diesem Pfad:

YOCTO_FOLDER/sources/meta-variscite-*/recipes-graphics/wayland/weston-init/

Schritt 3: weston-init.bbappend anpassen

Pfad:

YOCTO_FOLDER/sources/meta-variscite-*/recipes-graphics/wayland/weston-init.bbappend

Füge folgendes hinzu:

```
SRC_URI:append = " file://wallpaper.png"

do_install:append() {
    install -D -p -m0644 ${WORKDIR}/wallpaper.png ${D}${sysconfdir}/xdg/weston/
}
```

### 5. Für die Gst Pipeline um ein Videoconvert über die Grafikkarte zu machen mit dem Modul imxvideoconvert_g2d

um diese Pipeline verwenden zu können in Video.cpp

```
std::string gstCommand =
    "v4l2src device=/dev/video3 ! "
    "video/x-raw,width=" + std::to_string(width) + ",height=" + std::to_string(height) + " ! "
    "imxvideoconvert_g2d ! "
    "video/x-raw,format=RGB ! "
    "queue ! "
    "appsink name=sink";
```


Öffne deine local.conf oder dein Image-Recipe (z. B. core-image-weston.bbappend oder eigenes Image .bb):

Füge folgendes hinzu:
Variante A: in conf/local.conf

```
IMAGE_INSTALL:append = " gstreamer1.0-plugins-imx"

```

### 6. gststreamer pipeline angepasst 

---

### ✅ **Unterstützte Formate (OpenGL ES 2.0)**

- `GL_RGB`
- `GL_RGBA`
- `GL_LUMINANCE`
- `GL_LUMINANCE_ALPHA`
- `GL_ALPHA`

---

### ❌ **Nicht unterstützte Formate**

- `GL_BGR`
- `GL_BGRA`
- `GL_RED`
- `GL_RG`
- `GL_RGB16F`, `GL_RGBA16F`, `GL_FLOAT` (nur ES 3.0+ oder mit Extensions)

---

1. v4l2src liefert YUY2, ein häufig genutztes YUV-Format (YUV422).

2. imxvideoconvert_g2d nutzt die GPU-Beschleunigung zur Umwandlung → spart CPU.

3. videoconvert behebt ggf. Inkompatibilitäten (z. B. Farbraum, Alignment-Probleme), die imxvideoconvert_g2d allein nicht komplett löst.

4. RGB Format passt gut zu OpenGLs GL_RGB.


```
std::string gstCommand = "v4l2src device=/dev/video3 ! video/x-raw,width=" + std::to_string(width) + ",height=" + std::to_string(height) + " pixelformat=YUY2 ! imxvideoconvert_g2d ! videoconvert ! video/x-raw,format=RGB ! queue max-size-buffers=1 ! appsink name=sink";
```