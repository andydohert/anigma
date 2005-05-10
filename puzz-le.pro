TEMPLATE	= app
CONFIG		= qt warn_on debug
#CONFIG		= qt warn_on release
HEADERS		= PuzzleWindow.h ImageRepository.h MenuButton.h Playground.h GameDialog.h FileDialog.h AboutDialog.h SoundRepository.h Puzzle.h\
		  OptionsDialog.h block.h
SOURCES		= main.cpp PuzzleWindow.cpp ImageRepository.cpp MenuButton.cpp Playground.cpp GameDialog.cpp FileDialog.cpp AboutDialog.cpp \
		  SoundRepository.cpp Puzzle.cpp OptionsDialog.cpp block.cpp
#-Wunused-function
#INCLUDEPATH	+= $(QPEDIR)/include
#DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lkdecore 
TARGET		= puzz-le
DEFINES = KDE_RELEASE
INCLUDEPATH	+= $(KDEDIR)/include
DEPENDPATH	+= $(KDEDIR)/include
#DEFINES		= DEMO_VERSION
