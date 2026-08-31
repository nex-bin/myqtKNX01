QT += widgets network core

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dpt1bit.cpp \
    dpt1bitcontrolled.cpp \
    dpt1byte.cpp \
    dpt2bitset.cpp \
    dpt2bytefloat.cpp \
    dpt2bytesignedvalue.cpp \
    dpt2byteunsignedvalue.cpp \
    dpt32bitset.cpp \
    dpt3bitcontrolled.cpp \
    dpt4bytefloat.cpp \
    dpt4bytesignedvalue.cpp \
    dpt4byteunsignedvalue.cpp \
    dpt8bitset.cpp \
    dpt8bitsignedvalue.cpp \
    dpt8bitunsignedvalue.cpp \
    dptchar.cpp \
    dptcharstring.cpp \
    dptdatapointtype.cpp \
    dptdatapointtypefactory.cpp \
    dptdatetime.cpp \
    dptelectricalenergy.cpp \
    dptentranceaccess.cpp \
    dptscene.cpp \
    dptstatusmode3.cpp \
    dpttime.cpp \
    dptutf8string.cpp \
    dptvarstring.cpp \
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
    netipdescriptionrequest.cpp \
    netipdescriptionresponse.cpp \
    netipdeviceconfigurationacknowledge.cpp \
    netipdeviceconfigurationrequest.cpp \
    netipdevicedib.cpp \
    netipdisconnectrequest.cpp \
    netipdisconnectresponse.cpp \
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
    qnewinterfaceobjectpropertydatatype.cpp \
    qnewinterfaceobjecttype.cpp \
    qnewkeyring.cpp \
    qnewlinklayerframe.cpp \
    qnewlinklayerframebuilder.cpp \
    qnewprojectutils_p.cpp \
    qnewsecurekey.cpp \
    qnewssl_openssl.cpp \
    qnewtpdu.cpp \
    qnewtpdufactory_broadcast.cpp \
    qnewtpdufactory_multicast.cpp \
    qnewtpdufactory_p2p.cpp

HEADERS += \
    dpt1bit.h \
    dpt1bitcontrolled.h \
    dpt1byte.h \
    dpt2bitset.h \
    dpt2bytefloat.h \
    dpt2bytesignedvalue.h \
    dpt2byteunsignedvalue.h \
    dpt32bitset.h \
    dpt3bitcontrolled.h \
    dpt4bytefloat.h \
    dpt4bytesignedvalue.h \
    dpt4byteunsignedvalue.h \
    dpt8bitset.h \
    dpt8bitsignedvalue.h \
    dpt8bitunsignedvalue.h \
    dptchar.h \
    dptcharstring.h \
    dptdatapointtype.h \
    dptdatapointtype_p.h \
    dptdatapointtypefactory.h \
    dptdatetime.h \
    dptelectricalenergy.h \
    dptentranceaccess.h \
    dptscene.h \
    dptstatusmode3.h \
    dpttime.h \
    dptutf8string.h \
    dptvarstring.h \
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
    netipdescriptionrequest.h \
    netipdescriptionresponse.h \
    netipdeviceconfigurationacknowledge.h \
    netipdeviceconfigurationrequest.h \
    netipdevicedib.h \
    netipdisconnectrequest.h \
    netipdisconnectresponse.h \
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
    qnewinterfaceobjectpropertydatatype.h \
    qnewinterfaceobjecttype.h \
    qnewkeyring_p.h \
    qnewlinklayerframe.h \
    qnewlinklayerframebuilder.h \
    qnewobject_p.h \
    qnewprojectutils_p.h \
    qnewsecurekey.h \
    qnewssl_p.h \
    qnewtpdu.h \
    qnewtpdufactory_p.h \
    qnewtraits.h \
    qnewutils.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG(debug, debug|release) {
    message("Debug mode")
    # 可选：添加调试信息
    QMAKE_CXXFLAGS += -g
} else {
    message("Release mode")
}

# 在你的主项目 .pro 文件中
# LIBS += -L/path/to/mymod/lib -lQt6MyMOD
# LIBS += -LD:/programs/QT/myQTools/mymod/lib -lQt6MyMOD
# INCLUDEPATH += /path/to/mymod/src
# INCLUDEPATH += D:/programs/QT/myQTools/mymod/src