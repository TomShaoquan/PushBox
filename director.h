#ifndef DIRECTOR_H
#define DIRECTOR_H
#include <QWidget>
#include<QPushButton>
#include<QLabel>
#include<QKeyEvent>
#include<QPainter>
#include<QTimer>
#include"gamemenu.h"
#include"illustrate.h"
#include"blockrecord.h"
#include"monster.h"
#include"common.h"
class Director : public QWidget
{
    Q_OBJECT
public:
    explicit Director(int check=0,QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);  /*绘制场景*/

    void keyPressEvent(QKeyEvent* ev);    /*获取键盘输入*/


public:
    void initProps();                   /*初始化道具*/
    void initScene();

    void showScene(QPainter& painter);                   /*显示场景*/
    void moveActor(QPainter& painter,MOVE_DIR dir);     /*移动角色*/

    void ActorMoveToUp(QPainter& painter);
    void ActorMoveToDown(QPainter& painter);
    void ActorMoveToLeft(QPainter& painter);
    void ActorMoveToRight(QPainter& painter);

    bool isCanMoveMonster(MOVE_DIR dir);
    void moveMonster(QPainter& painter);

    void putAimageOnPos(QPainter& painter,BlockRecord desView,BlockRecord::PROPS prop);
    bool isSuccess();     /*判断游戏是否胜利*/
    bool isFilaure();     /*判断游戏是否失败*/
    void pause();           /*暂停游戏*/

public slots:
    void setCheck(int check);
    void onMenuBtnClicked(bool);
    void onHelpBtnClicked(bool);
    void onTimeOut();
private:
    QPushButton* m_menu;
    QPushButton* m_help;
    GameMenu* m_game;
    int m_curCheck;      /*当前关卡*/
    QTimer* m_timer;

    Illustrate m_illustrate;
    MOVE_DIR m_dir;         /*移动方向*/
private:
    QPixmap img[BlockRecord::GAMEFILURE+1];   /*图片数组*/
    BlockRecord blockView[ROW][COL];
    BlockRecord m_actor;            /*用来记录角色的位置*/
    Monster m_monster;            /*用来记录怪物的位置*/
    bool isTimeout;
private:
    int map[CHECK_SUM][ROW][COL] =
    {
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//0
            { 0, 1 ,0, 1, 1, 1, 1, 1, 1, 8, 0 ,0 },//1
            { 0 ,1 ,2, 1, 0, 3, 1, 0, 3, 1, 0, 0 },//2
            { 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0 },//3
            { 0, 1, 0 ,3, 0, 1 ,1, 2, 1, 1, 1, 0 },//4
            { 0, 1, 1, 1, 0, 4, 1, 1, 1, 2, 1, 0 },//5
            { 0, 1, 3, 1, 1, 2, 1, 1, 1, 1, 1, 0 },//6
            { 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0 },//7
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }//8
        },
        {     //9表示什么都没有
            { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 },//0
            { 9, 0 ,0, 0, 0, 0, 0, 0, 0, 0, 9 ,9 },//1
            { 9 ,0 ,1, 1, 1, 4, 1, 1, 1, 0, 9, 9 },//2
            { 9, 0, 1, 1, 2, 3, 2, 1, 1, 0, 9, 9 },//3
            { 9, 0, 0 ,3, 0, 2 ,0, 3, 0, 0, 9, 9 },//4
            { 9, 0, 1, 1, 2, 3, 1, 1, 1, 0, 9, 9 },//5
            { 9, 0, 1, 1, 1, 0, 1, 1, 8, 0, 9, 9 },//6
            { 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9 },//7
            { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 }//8
        },
        {
            { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 },
            { 9, 9 ,9, 0, 0, 0, 0, 0, 0, 9, 9 ,9 },
            { 9 ,9 ,0, 0, 1, 3, 3, 3, 0, 9, 9, 9 },
            { 9, 9, 0, 1, 2, 1, 2, 1, 0, 9, 9, 9 },
            { 9, 9, 0 ,1, 2, 4 ,2, 1, 0, 9, 9, 9 },
            { 9, 9, 0, 1, 2, 1, 2, 8, 0, 9, 9, 9 },
            { 9, 9, 0, 3, 3, 3, 1, 0, 0, 9, 9, 9 },
            { 9, 9, 0, 0, 0, 0, 0, 0, 9, 9, 9, 9 },
            { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 }
        },
        {
            { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 },
            { 9, 9 ,0, 0, 0, 0, 0, 0, 0, 0, 9 ,9 },//1
            { 9 ,9 ,0, 1, 1, 1, 1, 1, 8, 0, 9, 9 },//2
            { 9, 9, 0, 1, 2, 2, 2, 1, 1, 0, 9, 9 },//3
            { 9, 9, 0 ,1, 3, 3 ,3, 0, 0, 0, 9, 9 },//4
            { 9, 9, 0, 0, 0, 3, 3, 3, 1, 0, 9, 9 },//5
            { 9, 9, 0, 1, 4, 2, 2, 2, 1, 0, 9, 9 },//6
            { 9, 9, 0, 1, 1, 1, 1, 1, 1, 0, 9, 9 },//7
            { 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9 }//8
        }
    };

};



#endif // DIRECTOR_H
