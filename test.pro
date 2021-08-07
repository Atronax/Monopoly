TARGET = Monopoly
QT += core gui widgets xml

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11 c++14 c++17

SOURCES += \
    main.cpp \
    cards/card.cpp \
    cards/deck.cpp \
    helper/description.cpp \
    nodes/node.cpp \
    nodes/nodeeditor.cpp \
    nodes/tokens/actiontoken.cpp \
    nodes/tokens/ownershiptoken.cpp \
    nodes/tokens/token.cpp \
    player/hand.cpp \
    player/player.cpp \
    table.cpp \
    ui/die.cpp \
    ui/dieview.cpp \
    ui/historylabel.cpp \
    ui/menu.cpp \
    ui/details.cpp \
    ui/uielement.cpp \
    ui/uielementfactory.cpp \
    ui/view.cpp

HEADERS += \
    cards/card.h \
    cards/deck.h \
    helper/description.h \
    nodes/node.h \
    nodes/nodeeditor.h \
    nodes/tokens/actiontoken.h \
    nodes/tokens/ownershiptoken.h \
    nodes/tokens/token.h \
    player/hand.h \
    player/player.h \
    table.h \
    ui/die.h \
    ui/dieview.h \
    ui/historylabel.h \
    ui/menu.h \
    ui/details.h \
    ui/uielement.h \
    ui/uielementfactory.h \
    ui/view.h

# LIBS += -LC:/Libraries/OpenCV-4.5.1/build2/install/x64/vc16/lib -lopencv_core451 -lopencv_videoio451 -lopencv_imgcodecs451 -lopencv_imgproc451

# INCLUDEPATH += "C:/Libraries/OpenCV-4.5.1/build2/install/include"
# DEPENDPATH += "c:/Libraries/OpenCV-4.5.1/build2/install/x64/vc16/bin"
