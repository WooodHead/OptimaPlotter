TEMPLATE = lib
TARGET = qwt

win32:CONFIG(release, debug|release): DESTDIR = ../../Win32/Release
else:win32:CONFIG(debug, debug|release): DESTDIR = ../../Win32/Debug

QT += core gui svg opengl qaxserver qaxcontainer
DEFINES += _WINDOWS QT_LARGEFILE_SUPPORT QT_DLL QWT_DLL QWT_MAKEDLL QT_DLL QT_NO_KEYWORDS QT_SVG_LIB QT_OPENGL_LIB QT_HAVE_MMX QT_HAVE_3DNOW QT_HAVE_SSE QT_HAVE_MMXEXT QT_HAVE_SSE2
INCLUDEPATH += ../../../../../../$(QTDIR)/include/QtCore \
    ../../../../../../$(QTDIR)/include/QtGui \
    ../../../../../../$(QTDIR)/include/QtOpenGL \
    ../../../../../../$(QTDIR)/include/QtSvg \
    ../../../../../../$(QTDIR)/include \
    ../../../../../../$(QTDIR)/include/ActiveQt \
    ../../../../../../moc \
    $(QTDIR)/mkspecs/default
LIBS += -lglu32 \
    -lopengl32 \
    -lgdi32 \
    -luser32
DEPENDPATH += .

win32:CONFIG(release, debug|release): MOC_DIR += ./GeneratedFiles/release
else:win32:CONFIG(debug, debug|release): MOC_DIR += ./GeneratedFiles/debug

win32:CONFIG(release, debug|release): OBJECTS_DIR += ./release
else:win32:CONFIG(debug, debug|release): OBJECTS_DIR += ./debug

UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

include(qwt.pri)
