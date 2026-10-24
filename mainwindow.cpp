#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QUdpSocket>
#include <qnewbytearray.h>

#include <QNetworkDatagram>   //需要有这个头，否则m_udpSocket->receiveDatagram();报错

#include <netipconnectionheader.h>
#include <netipframe.h>
#include <netipservicediscoveryagent_p.h>
#include <netipconnectrequest.h>
#include <netipconnectresponse.h>
#include <netipconnectionstaterequest.h>
#include <netipconnectionstateresponse.h>
#include <netipdisconnectrequest.h>
#include <netipdisconnectresponse.h>
#include <netiptunnelingrequest.h>
#include <netipserviceinfo.h>
#include <netipdeviceconfigurationrequest.h>
#include <qnewdevicemanagementframefactory.h>
#include <netipdeviceconfigurationacknowledge.h>
#include <qnewlinklayerframebuilder.h>
#include <dpt1bit.h>
#include <netipsrp.h>

#include "QPlainTextEdit"
#include <netipdescriptionrequest.h>
#include <QButtonGroup>
#include <netiptunnelingacknowledge.h>

#include <QThread>
#include <myknxnetwork.h>

#include <userpage.h>
#include <userface.h>
#include <usermainpage.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("KNXSend");

    networkThread = new QThread(this);
    worker = new myKNXnetwork();

    // 【核心步骤】将 Worker 移动到子线程
    worker->moveToThread(networkThread);


    connect(networkThread, &QThread::started, worker, &myKNXnetwork::onNetworkInfo);
    // 清理：销毁线程时回收 Worker
    connect(networkThread, &QThread::finished, worker, &QObject::deleteLater);

    networkThread->start();

    init();
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::init(){

    mainStackedWidget = new QStackedWidget(this);
    QAction * userfaceAction = ui->actionKNXnetIp;
    QAction * userpageAction = ui->actionUserpage;

    QWidget * centralWid = ui->centralwidget;

    userface * userinterfaceWidget = new userface(centralWid);
    userpage * userpageWidget = new userpage(centralWid);
    usermainpage * usermainpageWidget = new usermainpage(centralWid);

    connect(userpageWidget,&userpage::turnToMainpage,this,&MainWindow::toUserMainpageWidget);
    connect(usermainpageWidget,&usermainpage::turnTopage,this,&MainWindow::toUserpageWidget);

    connect(userinterfaceWidget,&userface::sigLocalIpaddress,worker,&myKNXnetwork::onNetworkInfo);
    connect(userinterfaceWidget,&userface::sigfleshport,worker,&myKNXnetwork::onNetworkInfo);
    connect(worker,&myKNXnetwork::sigLocalIpaddress,userinterfaceWidget,&userface::onLocalIpaddress,Qt::QueuedConnection);

    connect(userinterfaceWidget,&userface::sigsearchrequest,worker,&myKNXnetwork::onsearchrequest);
    connect(worker,&myKNXnetwork::sigfindedDeviceInfo,userinterfaceWidget,&userface::onFindedDeviceInfo);

    connect(userinterfaceWidget,&userface::sigdescriptionrequest,worker,&myKNXnetwork::ondescriptionrequest);
    connect(worker,&myKNXnetwork::sigDetailInfo,userinterfaceWidget,&userface::onDeviceDetailInfo);

    connect(userinterfaceWidget,&userface::sigconnectrequest,worker,&myKNXnetwork::onconnectrequest);
    connect(worker,&myKNXnetwork::sigconnectInfo,userinterfaceWidget,&userface::onconnectInfo);

    connect(userinterfaceWidget,&userface::sigdisconnect,worker,&myKNXnetwork::ondisconnectrequest);

    connect(worker,&myKNXnetwork::sigstate,userinterfaceWidget,&userface::onConnectState);

    connect(worker,&myKNXnetwork::sigValueSendState,userinterfaceWidget,&userface::onValueSendState);

    connect(userinterfaceWidget,&userface::sigsendvalue,worker,&myKNXnetwork::tunnelRequest_KNXWrite);

    connect(userinterfaceWidget,&userface::sigsendvalueread,worker,&myKNXnetwork::tunnelRequest_KNXRead);


    connect(userpageWidget,&userpage::KNXValueSend,worker,&myKNXnetwork::tunnelRequest_KNXWrite);


    int userfaceIndex = mainStackedWidget->addWidget(userinterfaceWidget);
    int userpageIndex = mainStackedWidget->addWidget(userpageWidget);
    int usermainpageIndex = mainStackedWidget->addWidget(usermainpageWidget);

    connect(userfaceAction,&QAction::triggered,this,&MainWindow::toUserfaceWidget);
    connect(userpageAction,&QAction::triggered,this,&MainWindow::toUserMainpageWidget);


    mainStackedWidget->setCurrentIndex(0);
    setCentralWidget(mainStackedWidget);

}
void MainWindow::toUserfaceWidget(){
    qDebug()<<"toUserfaceWidget";

    mainStackedWidget->setCurrentIndex(0);
}
void MainWindow::toUserMainpageWidget(){
    qDebug()<<"toUserpageWidget";
    mainStackedWidget->setCurrentIndex(2);
}
void MainWindow::toUserpageWidget(){
    qDebug()<<"toUserpageWidget";
    mainStackedWidget->setCurrentIndex(1);
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "networkThread:" << networkThread;
    // 假设你的 QThread 指针是 m_thread
    if (networkThread && networkThread->isRunning()) {
        qDebug()<<"正在执行退出清理程序...........";
        // 方式一：阻塞等待清理完成（推荐，确保清理干净）
        QMetaObject::invokeMethod(worker, "m_cleanup", Qt::BlockingQueuedConnection);


        // 1. 请求线程退出事件循环
        networkThread->quit();
        // 2. 请求线程中断当前阻塞的操作（如果你的 run() 里有循环）
        networkThread->requestInterruption();
        // 3. 阻塞主线程，等待子线程真正运行结束（最多等3秒）
        if (!networkThread->wait(3000)) {
            // 如果3秒后还没结束，只能强制终止（不推荐，但能兜底）
            qWarning() << "线程未能在 3 秒内退出，强制终止";
            networkThread->terminate();
            networkThread->wait();
        }
    }
    event->accept();
}
