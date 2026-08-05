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
    netipconnectionstaterequest.cpp \
    netipconnectionstateresponse.cpp \
    netipconnectrequest.cpp \
    netipconnectresponse.cpp \
    netipcrd.cpp \
    netipcri.cpp \
    netipdeviceconfigurationacknowledge.cpp \
    netipdeviceconfigurationrequest.cpp \
    netipdisconnectrequest.cpp \
    netipdisconnectresponse.cpp \
    netipdivicedib.cpp \
    netipendpointconnection.cpp \
    netipextendeddevicedib.cpp \
    netipframe.cpp \
    netipframeheader.cpp \
    netiphpai.cpp \
    netipsearchrequest.cpp \
    netipsearchresponse.cpp \
    netipsecureconfiguration.cpp \
    netipsecurewrapper.cpp \
    netipserverfamiliesdib.cpp \
    netipservicediscoveryagent.cpp \
    netipservicediscoveryagent_p.cpp \
    netipserviceinfo.cpp \
    netipserviceinfo_p.cpp \
    netipsessionauthenticate.cpp \
    netipsessionrequest.cpp \
    netipsessionresponse.cpp \
    netipsessionstatus.cpp \
    netipsrp.cpp \
    netipstruct.cpp \
    netipstructheader.cpp \
    netiptimernotify.cpp \
    netiptunnelingacknowledge.cpp \
    netiptunnelingfeatureget.cpp \
    netiptunnelingfeatureinfo.cpp \
    netiptunnelingfeatureresponse.cpp \
    netiptunnelingfeatureset.cpp \
    netiptunnelinginfodib.cpp \
    netiptunnelingrequest.cpp \
    qnewaddress.cpp \
    qnewaddtionalinfo.cpp \
    qnewbytearray.cpp \
    qnewcontrolfield.cpp \
    qnewcryptographicengine.cpp \
    qnewdevicemanagementframe.cpp \
    qnewdevicemanagementframefactory.cpp \
    qnewextendedcontrolfield.cpp \
    qnewinterfaceobjectproperty.cpp \
    qnewinterfaceobjecttype.cpp \
    qnewkeyring.cpp \
    qnewlinklayerframe.cpp \
    qnewlinklayerframebuilder.cpp \
    qnewprojectutils_p.cpp \
    qnewsecurekey.cpp \
    qnewssl_openssl.cpp \
    qnewtpdu.cpp

HEADERS += \
    mainwindow.h \
    myqglobal_p.h \
    myqtnetworkglobal_p.h \
    netip01.h \
    netipconnectionheader.h \
    netipconnectionstaterequest.h \
    netipconnectionstateresponse.h \
    netipconnectrequest.h \
    netipconnectresponse.h \
    netipcrd.h \
    netipcri.h \
    netipdeviceconfigurationacknowledge.h \
    netipdeviceconfigurationrequest.h \
    netipdisconnectrequest.h \
    netipdisconnectresponse.h \
    netipdivicedib.h \
    netipendpointconnection.h \
    netipendpointconnection_p.h \
    netipextendeddevicedib.h \
    netipframe.h \
    netipframeheader.h \
    netiphpai.h \
    netipsearchrequest.h \
    netipsearchresponse.h \
    netipsecureconfiguration.h \
    netipsecureconfiguration_p.h \
    netipsecurewrapper.h \
    netipserverfamiliesdib.h \
    netipservicediscoveryagent.h \
    netipservicediscoveryagent_p.h \
    netipserviceinfo.h \
    netipserviceinfo_p.h \
    netipsessionauthenticate.h \
    netipsessionrequest.h \
    netipsessionresponse.h \
    netipsessionstatus.h \
    netipsrp.h \
    netipstruct.h \
    netipstructheader.h \
    netiptimernotify.h \
    netiptunnelingacknowledge.h \
    netiptunnelingfeatureget.h \
    netiptunnelingfeatureinfo.h \
    netiptunnelingfeatureresponse.h \
    netiptunnelingfeatureset.h \
    netiptunnelinginfodib.h \
    netiptunnelingrequest.h \
    qmyknx.h \
    qnewaddress.h \
    qnewaddtionalinfo.h \
    qnewbuilderdata_p.h \
    qnewbuildings_p.h \
    qnewbytearray.h \
    qnewcontrolfield.h \
    qnewcryptographicengine.h \
    qnewdevicemanagementframe.h \
    qnewdevicemanagementframefactory.h \
    qnewextendedcontrolfield.h \
    qnewinterfaceobjectproperty.h \
    qnewinterfaceobjecttype.h \
    qnewkeyring_p.h \
    qnewlinklayerframe.h \
    qnewlinklayerframebuilder.h \
    qnewobject_p.h \
    qnewprojectutils_p.h \
    qnewsecurekey.h \
    qnewssl_p.h \
    qnewtpdu.h \
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