TEMPLATE	= app
CONFIG		= qt warn_on debug
#CONFIG		= qt warn_on release
HEADERS		= puzzlewindow.h imagerepository.h menubutton.h playground.h gamedialog.h filedialog.h aboutdialog.h soundrepository.h puzzle.h optionsdialog.h playblock.h demo.h
SOURCES		= puzzlewindow.cpp imagerepository.cpp menubutton.cpp playground.cpp gamedialog.cpp filedialog.cpp aboutdialog.cpp soundrepository.cpp puzzle.cpp optionsdialog.cpp playblock.cpp main.cpp demo.cpp
TARGET		= puzz-le
#DEFINES		= DEMO_VERSION
#The following line was inserted by qt3to4
QT +=  qt3support 
