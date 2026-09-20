#ifndef USERMAINPAGE_H
#define USERMAINPAGE_H

#include <QWidget>

namespace Ui {
class usermainpage;
}

class usermainpage : public QWidget
{
    Q_OBJECT

public:
    explicit usermainpage(QWidget *parent = nullptr);
    ~usermainpage();

    void init();

signals:
    void turnTopage();
private:
    Ui::usermainpage *ui;
};

#endif // USERMAINPAGE_H
