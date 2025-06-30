LICENSE = "CLOSED"

DEPENDS:append = " qtbase"

SRC_URI = "file://cube-imu.pro \ 
           file://mpu6050.c \
           file://mpu6050.h \
           file://main.cpp \
           file://mainwidget.cpp \
           file://mainwidget.h \
           file://mainwidget.h \
           file://geometryengine.cpp \
           file://geometryengine.h \
           file://shaders.qrc \
           file://textures.qrc \
           file://vshader.glsl \
           file://fshader.glsl \
           file://cube.png \
          "
S = "${WORKDIR}"

do_install:append () {
        install -d ${D}${bindir}
        install -m 0775 cube-imu ${D}${bindir}
}

FILES_${PN}:append = "${bindir}/cube-imu"

inherit qt6-qmake
