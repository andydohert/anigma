TEMPLATE	= app
CONFIG		= qt warn_on debug
#CONFIG		= qt warn_on release
HEADERS		= puzzlewindow.h imagerepository.h menubutton.h playground.h gamedialog.h filedialog.h aboutdialog.h soundrepository.h puzzle.h\
		  optionsdialog.h block.h
SOURCES		= puzzlewindow.cpp imagerepository.cpp menubutton.cpp playground.cpp gamedialog.cpp filedialog.cpp aboutdialog.cpp \
		  soundrepository.cpp puzzle.cpp optionsdialog.cpp block.cpp main.cpp
#-Wunused-function
#INCLUDEPATH	+= $(QPEDIR)/include
#DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lkdecore 
TARGET		= puzz-le
DEFINES = KDE_RELEASE
INCLUDEPATH	+= $(KDEDIR)/include $(KDEDIR)/compat
DEPENDPATH	+= $(KDEDIR)/include
#DEFINES		= DEMO_VERSION
