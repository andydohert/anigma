TEMPLATE = app
TARGET = puzz-le
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR         = ../
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

CONFIG		+= qt warn_on debug
#CONFIG		+= qt warn_on release
#DEFINES		= DEMO_VERSION
#QT +=  qt3support

RESOURCES     = resources.qrc

mac {
    ICON = puzz-le.icns
    QMAKE_INFO_PLIST = Info_mac.plist
    TARGET = Puzz-le
}

#target.path=$$[QT_INSTALL_BINS]
#INSTALLS += target

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
