TEMPLATE = lib
TARGET = SplineAlgorithm

win32:CONFIG(release, debug|release): DESTDIR = ../Win32/Release
else:win32:CONFIG(debug, debug|release): DESTDIR = ../Win32/Debug

QT += core gui
DEFINES += QT_LARGEFILE_SUPPORT QT_DLL SPLINEALGORITHM_LIB
INCLUDEPATH += ./GeneratedFiles \
    . \
    ./../Eigen \
    ./../Globals \
    ./../Interfaces
PRECOMPILED_HEADER = StdAfx.h
DEPENDPATH += .

win32:CONFIG(release, debug|release): MOC_DIR += ./GeneratedFiles/release
else:win32:CONFIG(debug, debug|release): MOC_DIR += ./GeneratedFiles/debug

win32:CONFIG(release, debug|release): OBJECTS_DIR += ./release
else:win32:CONFIG(debug, debug|release): OBJECTS_DIR += ./debug

UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(SplineAlgorithm.pri)
TRANSLATIONS += splinealgorithm_hy.ts


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Win32/Release/ -lInterfaces
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Win32/Debug/ -lInterfaces
else:unix: LIBS += -L$$OUT_PWD/../Interfaces/ -lInterfaces

INCLUDEPATH += $$PWD/../Interfaces
DEPENDPATH += $$PWD/../Interfaces

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Win32/Release/Interfaces.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Win32/Debug/Interfaces.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Interfaces/libInterfaces.ac
