#include "usermainpage.h"
#include "ui_usermainpage.h"

usermainpage::usermainpage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::usermainpage)
{
    ui->setupUi(this);
    init();

}

usermainpage::~usermainpage()
{
    delete ui;
}
void usermainpage::init(){
    connect(ui->pushButton,&QPushButton::clicked,this,&usermainpage::turnTopage);
}