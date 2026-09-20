#ifndef USERFACE_H
#define USERFACE_H

#include <QWidget>
#include <QHostAddress>

namespace Ui {
class userface;
}

class userface : public QWidget
{
    Q_OBJECT

public:
    explicit userface(QWidget *parent = nullptr);
    ~userface();

    void init();
    void resLocalIpaddress(QList <QHostAddress> addresses);

private:
    Ui::userface *ui;
    void sendfleshport();
    void sendsearchrequest();
    void sendconnectrequest();
    void senddisconnectrequest();

signals:
    void askLocalIpaddress();
    void fleshport();
    void searchrequest();
    void connectrequest();
    void disconnectrequest();
};

#endif // USERFACE_H
