QT += core gui widgets opengl openglwidgets

TARGET = cube-imu
TEMPLATE = app

SOURCES += main.cpp

SOURCES += \
    mainwidget.cpp \
    geometryengine.cpp \
    mpu6050.c

HEADERS += \
    mainwidget.h \
    geometryengine.h \
    mpu6050.h

RESOURCES += \
    shaders.qrc \
    textures.qrc

