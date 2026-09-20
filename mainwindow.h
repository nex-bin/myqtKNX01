#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QStackedWidget>
#include <QAction>
#include <myKNXnetwork.h>
#include <netipservicediscoveryagent_p.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void init();
    void toUserfaceWidget();
    void toUserpageWidget();
    void toUserMainpageWidget();
    QThread *networkThread;
    QStackedWidget * mainStackedWidget;
    myKNXnetwork *worker;
protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void sendConnectRequest();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
