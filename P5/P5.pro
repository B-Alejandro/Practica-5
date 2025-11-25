TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++20

SOURCES += \
        colision.cpp \
        colisionmanager.cpp \
        main.cpp \
        obstaculo.cpp \
        particula.cpp \
        simulador.cpp \
        vector.cpp

HEADERS += \
    colision.h \
    colisionmanager.h \
    obstaculo.h \
    particula.h \
    simulador.h \
    vector.h
