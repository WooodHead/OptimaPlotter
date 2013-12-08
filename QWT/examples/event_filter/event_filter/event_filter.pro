TEMPLATE = app
TARGET = event_filter

win32:CONFIG(release, debug|release): DESTDIR = ../../../../Win32/Release
else:win32:CONFIG(debug, debug|release): DESTDIR = ../../../../Win32/Debug

QT += core gui xmlpatterns svg opengl
#CONFIG += debug
DEFINES += QT_LARGEFILE_SUPPORT QT_DLL QT_OPENGL_LIB QT_SVG_LIB QT_XMLPATTERNS_LIB QWT_DLL

INCLUDEPATH += . \
    ./../../../src \
    ./GeneratedFiles
LIBS += -lopengl32 \
    -lglu32
DEPENDPATH += .

win32:CONFIG(release, debug|release): MOC_DIR += ./GeneratedFiles/release
else:win32:CONFIG(debug, debug|release): MOC_DIR += ./GeneratedFiles/debug

win32:CONFIG(release, debug|release): OBJECTS_DIR += ./release
else:win32:CONFIG(debug, debug|release): OBJECTS_DIR += ./debug

UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

include(event_filter.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../Win32/Release -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../Win32/Debug -lqwt
else:unix: LIBS += -L$$OUT_PWD/../../../src/ -lqwt

INCLUDEPATH += $$PWD/../../../src
DEPENDPATH += $$PWD/../../../src
