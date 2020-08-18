#ifndef GAMEMENU_H
#define GAMEMENU_H

#include <QWidget>

namespace Ui {
class GameMenu;
}

class GameMenu : public QWidget
{
    Q_OBJECT

public:
    explicit GameMenu(QWidget *parent = nullptr);
    ~GameMenu();
    void paintEvent(QPaintEvent *event);

signals:
    void checkChange(int check);
private slots:
    void on_pushButtonA_clicked();

    void on_pushButtonB_clicked();

    void on_pushButtonC_clicked();

    void on_pushButtonD_clicked();

private:
    Ui::GameMenu *ui;
};

#endif // GAMEMENU_H
