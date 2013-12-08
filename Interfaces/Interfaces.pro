TEMPLATE = lib
TARGET = Interfaces

win32:CONFIG(release, debug|release): DESTDIR = ../Win32/Release
else:win32:CONFIG(debug, debug|release): DESTDIR = ../Win32/Debug

QT += core gui
CONFIG += staticlib
DEFINES += QT_LARGEFILE_SUPPORT QT_DLL INTERFACES_LIB

INCLUDEPATH += ./GeneratedFiles \
    . \
    ./../Globals

PRECOMPILED_HEADER = StdAfx.h
DEPENDPATH += .

win32:CONFIG(release, debug|release): MOC_DIR += ./GeneratedFiles/release
else:win32:CONFIG(debug, debug|release): MOC_DIR += ./GeneratedFiles/debug

win32:CONFIG(release, debug|release): OBJECTS_DIR += ./release
else:win32:CONFIG(debug, debug|release): OBJECTS_DIR += ./debug

UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

include(Interfaces.pri)
