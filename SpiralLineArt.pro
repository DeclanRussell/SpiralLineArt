# This specifies the exe name
TARGET=SpiralLineArt
# where to put the .o files
OBJECTS_DIR=obj
# core Qt Libs to use add more here if needed.
QT+=gui opengl core
# as I want to support 4.8 and 5 this will set a flag for some of the mac stuff
# mainly in the types.h file for the setMacVisual which is native in Qt5
isEqual(QT_MAJOR_VERSION, 5) {
	cache()
	DEFINES +=QT5BUILD
}
# where to put moc auto generated files
MOC_DIR=moc
# on a mac we don't create a .app bundle file ( for ease of multiplatform use)
CONFIG-=app_bundle
VPATH += ./src
# and add the include dir into the search path for Qt and make
INCLUDEPATH +=./include
# Auto include all .cpp files in the project src directory (can specifiy individually if required)
SOURCES+= src/main.cpp \
        src/MainWindow.cpp  \
        src/NGLScene.cpp  \
        src/ParticleDrawer.cpp \
        src/Shader.cpp \
        src/ShaderProgram.cpp \
        src/ShaderUtils.cpp \
        src/Camera.cpp \
        src/Text.cpp \

# same for the .h files
HEADERS+=include/MainWindow.h \
        include/NGLScene.h \
        include/ParticleDrawer.h \
        include/Shader.h \
        include/ShaderProgram.h \
        include/ShaderUtils.h \
        include/Camera.h \
        include/Text.h \

# where our exe is going to live (root of project)
DESTDIR=./
# add the glsl shader files
OTHER_FILES+= shaders/*.glsl \
                README.md
# were are going to default to a console app
CONFIG += console
DEFINES += _USE_MATH_DEFINES
#in on mac define DARWIN
macx:DEFINES+=DARWIN
win32:{
    DEFINES+=WIN32
    DEFINES+=_WIN32
    DEFINES += GLEW_STATIC
    INCLUDEPATH+=C:/boost
    LIBS+= -lopengl32 -lglew32s
}
# basic compiler flags (not all appropriate for all platforms)
QMAKE_CXXFLAGS+= -msse -msse2 -msse3
# use this to suppress some warning from boost
unix*:QMAKE_CXXFLAGS_WARN_ON += "-Wno-unused-parameter"
