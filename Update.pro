CODECFORTR = UTF-8
CODECFORSRC = UTF-8

QMAKE_CXXFLAGS += -Wno-unused-local-typedefs

#Resolve macros.
#QMAKE_CXXFLAGS += -E

#TRANSLATIONS = lang_us.ts

RC_FILE = Update/UpdateProperties.rc

DEFINES += QT_NO_CAST_FROM_ASCII

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#For app name containing "update", "install" or "setup" Windows need admin rights...
#Workaround it using different name.
TARGET = VersionCheck
TEMPLATE = app

SOURCES += Update/Update.cpp \
    Update/main.cpp \
    Shared/Application.cpp \
    Shared/Networking.cpp \
    Shared/Logger.cpp

HEADERS  += Update/Update.h \
    Shared/Application.h \
    Update/UpdateProperties.h \
    Shared/CommonProperties.h \
    Shared/Networking.h \
    Shared/Logger.h \
    Shared/Debugging.h

FORMS += \
    Update/Update.ui

OTHER_FILES += \
    Update/UpdateProperties.rc \
    Update/Update.ico










