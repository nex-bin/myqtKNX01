#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

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
    QUdpSocket * m_udpSocket;
    void KNXIPconnect();
    void onReadyRead();

    // DiscovererConfig m_config;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
