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
    void wurao();
    void qingjie();
    void openlighting();
    void closelighting();
    void mingliangmode();
    void rouhemode();
    void richangmode();
    void shuimianmode();

private:
    Ui::userpage *ui;
};

#endif // USERPAGE_H
