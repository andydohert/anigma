TEMPLATE = app
TARGET = puzz-le
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

#CONFIG		+= qt warn_on debug
CONFIG		+= qt warn_on release
#DEFINES	= DEMO_VERSION

RESOURCES     = resources.qrc

sound.files = ../sounds/*
sound.path = /usr/share/puzzle/sounds

mac {
    ICON = puzzle.icns
    QMAKE_INFO_PLIST = Info_mac.plist
    TARGET = Puzz-le
    sound.path = ../Puzz-le.app/Contents/MacOS/sounds/
}


unix:QMAKE_POST_LINK=strip $(TARGET)

INSTALLS += sound

unix && !mac {
    target.path=/usr/bin
    INSTALLS += target
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
