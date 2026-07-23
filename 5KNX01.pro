QT += widgets network core

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    netip01.cpp \
    netipconnectionheader.cpp \
    netipconnectrequest.cpp \
    netipcri.cpp \
    netipdivicedib.cpp \
    netipextendeddevicedib.cpp \
    netipframe.cpp \
    netipframeheader.cpp \
    netiphpai.cpp \
    netipsearchrequest.cpp \
    netipsearchresponse.cpp \
    netipserverfamiliesdib.cpp \
    netipservicediscoveryagent.cpp \
    netipservicediscoveryagent_p.cpp \
    netipserviceinfo.cpp \
    netipserviceinfo_p.cpp \
    netipsrp.cpp \
    netipstruct.cpp \
    netipstructheader.cpp \
    netiptunnelinginfodib.cpp \
    qnewaddress.cpp \
    qnewbuildings_p.cpp \
    qnewbytearray.cpp \
    qnewprojectutils_p.cpp

HEADERS += \
    mainwindow.h \
    netip01.h \
    netipconnectionheader.h \
    netipconnectrequest.h \
    netipcri.h \
    netipdivicedib.h \
    netipextendeddevicedib.h \
    netipframe.h \
    netipframeheader.h \
    netiphpai.h \
    netipsearchrequest.h \
    netipsearchresponse.h \
    netipserverfamiliesdib.h \
    netipservicediscoveryagent.h \
    netipservicediscoveryagent_p.h \
    netipserviceinfo.h \
    netipserviceinfo_p.h \
    netipsrp.h \
    netipstruct.h \
    netipstructheader.h \
    netiptunnelinginfodib.h \
    qknxbuilderdata_p.h \
    qknxbuildings_p.h \
    qmyknx.h \
    qnewaddress.h \
    qnewbuildings_p.h \
    qnewbytearray.h \
    qnewcryptographicengine.h \
    qnewobject_p.h \
    qnewprojectutils_p.h \
    qnewsecurekey.h \
    qnewtraits.h \
    qnewutils.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 在你的主项目 .pro 文件中
# LIBS += -L/path/to/mymod/lib -lQt6MyMOD
# LIBS += -LD:/programs/QT/myQTools/mymod/lib -lQt6MyMOD
# INCLUDEPATH += /path/to/mymod/src
# INCLUDEPATH += D:/programs/QT/myQTools/mymod/src