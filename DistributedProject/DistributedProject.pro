#-------------------------------------------------
#
# Project created by QtCreator 2018-11-11T01:59:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DistributedProject
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        login.cpp \
    client.cpp \
    UDPClientSocket.cpp \
    ErrorHandler.cpp \
    Message.cpp \
    InterfaceDefintion.cpp \
    packet.cpp \
    Converter.cpp \
    UDPEnhancedSocket.cpp \
    UDPSocket.cpp \
    LocalDirectory.cpp \
    API.cpp \
    Server.cpp \
    UDPServerSocket.cpp \
    proxyFunctions.cpp \
    image.cpp \
    profile.cpp \
    search.cpp \
    feed.cpp \
    enter_username.cpp \
    globalvar.cpp \
    offeredservices.cpp \
    requestviews.cpp \
    viewreceivedimage.cpp \
    view_image.cpp \
    Configs.cpp \
    ApplicationServer.cpp \
    logs.cpp \
    notifications.cpp \
    handle_notifications.cpp \
    view_shared_image.cpp \
    request_more_views.cpp \
    manage_shared_with.cpp \
    view_statistics.cpp

HEADERS += \
        login.h \
    client.hpp \
    UDPClientSocket.hpp \
    ErrorHandler.hpp \
    Message.hpp \
    InterfaceDefintion.h \
    packet.hpp \
    Converter.hpp \
    UDPEnhancedSocket.hpp \
    UDPSocket.hpp \
    LocalDirectory.hpp \
    API.hpp \
    Server.hpp \
    UDPServerSocket.hpp \
    proxyFunctions.hpp \
    image.h \
    profile.h \
    search.h \
    feed.h \
    enter_username.h \
    globalvar.h \
    offeredservices.h \
    requestviews.h \
    viewreceivedimage.h \
    view_image.h \
    Configs.h \
    ApplicationServer.h \
    logs.h \
    notifications.h \
    handle_notifications.h \
    view_shared_image.h \
    request_more_views.h \
    manage_shared_with.h \
    view_statistics.h \
    ourexception.h

FORMS += \
        login.ui \
    profile.ui \
    search.ui \
    feed.ui \
    enter_username.ui \
    offeredservices.ui \
    requestviews.ui \
    viewreceivedimage.ui \
    view_image.ui \
    notifications.ui \
    handle_notifications.ui \
    view_shared_image.ui \
    request_more_views.ui \
    manage_shared_with.ui \
    view_statistics.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
