#include "gamemenu.h"
#include "ui_gamemenu.h"
#include<QPainter>
#include<QPixmap>

GameMenu::GameMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameMenu)
{
    ui->setupUi(this);
    setFixedSize(600,400);

    connect(this,SIGNAL(checkChange(int)),this->parent(),SLOT(setCheck(int)));

}

GameMenu::~GameMenu()
{
    delete ui;
}

void GameMenu::paintEvent(QPaintEvent *event)
{
    QPixmap pix;
    pix.load(":/res/timg.jpg");
    QPainter painter(this);
    painter.drawPixmap(0,0,width(),height(),pix);
    painter.end();
}

void GameMenu::on_pushButtonA_clicked()
{
    emit checkChange(0);
    hide();
    ((QWidget*)parent())->show();
}

void GameMenu::on_pushButtonB_clicked()
{
    emit checkChange(1);
    hide();
    ((QWidget*)parent())->show();
}

void GameMenu::on_pushButtonC_clicked()
{
    emit checkChange(2);
    hide();
    ((QWidget*)parent())->show();
}

void GameMenu::on_pushButtonD_clicked()
{
    emit checkChange(3);
    hide();
    ((QWidget*)parent())->show();
}
