#ifndef ILLUSTRATE_H
#define ILLUSTRATE_H

#include <QWidget>

namespace Ui {
class Illustrate;
}

class Illustrate : public QWidget
{
    Q_OBJECT

public:
    explicit Illustrate(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);
    ~Illustrate();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Illustrate *ui;
};

#endif // ILLUSTRATE_H
