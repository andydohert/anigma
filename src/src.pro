TEMPLATE = app
TARGET = anigma
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

CONFIG		+= qt warn_on debug
#unix:QMAKE_POST_LINK=strip $(TARGET)
#CONFIG		+= qt warn_on release
#DEFINES	= DEMO_VERSION

RESOURCES     = resources.qrc

sound.files = ../sounds/*
sound.path = /usr/share/anigma/sounds

mac {
    ICON = anigma.icns
    QMAKE_INFO_PLIST = Info_mac.plist
    TARGET = Anigma
    sound.path = ../Anigma.app/Contents/MacOS/sounds/
}

INSTALLS += sound

!mac:unix {
    target.path=/usr/bin
    INSTALLS += target

    desktop.files = ../anigma.desktop ../anigma.png
    desktop.path = /usr/share/applications/games/

    desktopicon.files = ../anigma.png
    desktopicon.path = /usr/share/icons/hicolor/16x16/

    INSTALLS += target desktop desktopicon
}

# Input
HEADERS += aboutdialog.h \
           demo.h \
           filedialog.h \
           gamedialog.h \
           imagerepository.h \
           infobar.h \
           menubutton.h \
           optionsdialog.h \
           playblock.h \
           playground.h \
           puzzle.h \
           puzzlewindow.h \
           soundrepository.h \
           levels.dat
SOURCES += aboutdialog.cpp \
           demo.cpp \
           filedialog.cpp \
           gamedialog.cpp \
           imagerepository.cpp \
           infobar.cpp \
           main.cpp \
           menubutton.cpp \
           optionsdialog.cpp \
           playblock.cpp \
           playground.cpp \
           puzzle.cpp \
           puzzlewindow.cpp \
           soundrepository.cpp
