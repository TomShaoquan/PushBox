#include "illustrate.h"
#include "ui_illustrate.h"
#include<QPixmap>
#include<QPainter>

Illustrate::Illustrate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Illustrate)
{
    ui->setupUi(this);
}

void Illustrate::paintEvent(QPaintEvent *event)
{
    QPixmap pix;
    pix.load(":/res/illustrate.png");
    setFixedSize(pix.width(),pix.height());
    QPainter painter(this);
    painter.drawPixmap(0,0,width(),height(),pix);
    painter.end();
}

Illustrate::~Illustrate()
{
    delete ui;
}

void Illustrate::on_pushButton_clicked()
{
    hide();
}
