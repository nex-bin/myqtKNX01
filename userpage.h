#ifndef USERPAGE_H
#define USERPAGE_H

#include <QWidget>

namespace Ui {
class userpage;
}

class userpage : public QWidget
{
    Q_OBJECT

public:
    explicit userpage(QWidget *parent = nullptr);
    ~userpage();
    void init();
    void sendwurao();
    void sendqingjie();
    void sendopenlighting();
    void sendcloselighting();
    void sendmingliangmode();
    void sendrouhemode();
    void sendrichangmode();
    void sendshuimianmode();


signals:
    void turnToMainpage();
    void wurao(QString ,QString value);
    void qingjie(QString ,QString value);
    void openlighting(QString ,QString value);
    void closelighting(QString ,QString value);
    void mingliangmode(QString ,QString value);
    void rouhemode(QString ,QString value);
    void richangmode(QString ,QString value);
    void shuimianmode(QString ,QString value);
    void KNXValueSend(QString address,QString value);

private:
    Ui::userpage *ui;
};

#endif // USERPAGE_H
