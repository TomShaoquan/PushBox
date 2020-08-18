#include "director.h"

#include<QDebug>
#include<QApplication>
#include<QTime>

Director::Director(int check,QWidget *parent) : QWidget(parent),
  m_curCheck(check)
{
    setWindowTitle(QString("推箱子-第%1关").arg(m_curCheck+1));
    setFixedSize(WIDETH,HEIGHT);

    /*菜单按钮*/
    m_menu=new QPushButton(this);
    m_menu->setParent(this);
    m_menu->setGeometry(QRect(300,3,200,28));
    connect(m_menu,SIGNAL(clicked(bool)),this,SLOT(onMenuBtnClicked(bool)));
    m_menu->setText("游戏菜单");

    /*说明按钮*/
    m_help=new QPushButton(this);
    m_help->setParent(this);
    connect(m_help,SIGNAL(clicked(bool)),this,SLOT(onHelpBtnClicked(bool)));
    m_help->setGeometry(QRect(500,3,200,28));
    m_help->setText("帮助");

    m_dir=OTHER;
    m_game=new GameMenu(this);
    m_game->hide();
    m_illustrate.setParent(this);
    m_illustrate.hide();

    initProps();
    initScene();

    isTimeout=false;
    m_timer=new QTimer(this);
    m_timer->setInterval(500);  /*每个一秒发射一次信号*/
    m_timer->start();
    connect(m_timer,SIGNAL(timeout()),this,SLOT(onTimeOut()));
}

void Director::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    showScene(painter);
    painter.drawPixmap(m_actor.x(),m_actor.y(),SIDE,SIDE,img[BlockRecord::MAN]);
    painter.drawPixmap(m_monster.x(),m_monster.y(),SIDE,SIDE,img[BlockRecord::MONSTER]);
    moveActor(painter,m_dir);

    //移动怪物
    moveMonster(painter);
}

void Director::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_W:
        m_dir=UP;
        break;
    case Qt::Key_S:
        m_dir=DOWN;
        break;
    case Qt::Key_A:
        m_dir=LEFT;
        break;
    case Qt::Key_D:
        m_dir=RIGHT;
        break;
    default:
        break;
    }
    this->update();

}

void Director::initScene()
{
    for(int i=0;i<ROW;i++)
    {
        for(int j=0;j<COL;j++)
        {
            if(map[m_curCheck][i][j]==BlockRecord::MAN)
            {
                m_actor.setBlock(i,j,j*SIDE+START_X,i*SIDE+START_Y,BlockRecord::MAN);
                /*小人下面为地板*/
                blockView[i][j].setBlock(i,j,j*SIDE+START_X,i*SIDE+START_Y,BlockRecord::FLOOR);
                continue;
            }
            if(map[m_curCheck][i][j]==BlockRecord::MONSTER)
            {
                m_monster.setBlock(i,j,j*SIDE+START_X,i*SIDE+START_Y,BlockRecord::MONSTER);
                /*小人下面为地板*/
                blockView[i][j].setBlock(i,j,j*SIDE+START_X,i*SIDE+START_Y,BlockRecord::FLOOR);
                continue;
            }
            blockView[i][j].setBlock(i,j,j*SIDE+START_X,i*SIDE+START_Y,(BlockRecord::PROPS)map[m_curCheck][i][j]);

        }
    }
}

void Director::showScene(QPainter& painter)
{
    painter.drawPixmap(0,0,WIDETH,HEIGHT,img[BlockRecord::BACK]);
    for(int i=0;i<ROW;i++)
    {
        for(int j=0;j<COL;j++)
        {
            if(blockView[i][j].getProp()==BlockRecord::SUM) continue;  //不是游戏区域直接返回

            painter.drawPixmap(blockView[i][j].x(),
                    blockView[i][j].y(),SIDE,SIDE,img[blockView[i][j].getProp()]);
        }
    }

}

void Director::moveActor(QPainter &painter, MOVE_DIR dir)
{
    /*所在位置-目标*/
    switch (dir)
    {
    case UP:
        ActorMoveToUp(painter);
        break;
    case DOWN:
        ActorMoveToDown(painter);
        break;
    case LEFT:
        ActorMoveToLeft(painter);
        break;
    case RIGHT:
        ActorMoveToRight(painter);
        break;
        default:
            break;
    }
    m_dir=OTHER;

    if(isSuccess())
    {
        painter.drawPixmap(170,180,img[BlockRecord::GAMEOVER].width(),img[BlockRecord::GAMEOVER].height(),
                img[BlockRecord::GAMEOVER]);
    }


}

void Director::ActorMoveToUp(QPainter& painter)
{
    if(blockView[m_actor.getRow()-1][m_actor.getCol()].getProp()==BlockRecord::SUM) return;   /*前面是非游戏区*/
    if(blockView[m_actor.getRow()-1][m_actor.getCol()].getProp()==BlockRecord::WALL) return;  /*前面是墙*/

    /*地板-地板*/
    if(blockView[m_actor.getRow()-1][m_actor.getCol()].getProp()==BlockRecord::FLOOR
       &&blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR)

    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()-1][m_actor.getCol()],BlockRecord::MAN);
    }
    /*地板-目标*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
            &&blockView[m_actor.getRow()-1][m_actor.getCol()].getProp()==BlockRecord::AIM)
    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()-1][m_actor.getCol()],BlockRecord::MAN);
    }
    /*地板-击中*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
            &&blockView[m_actor.getRow()-1][m_actor.getCol()].getProp()==BlockRecord::HIT)
    {
        if(blockView[m_actor.getRow()-2][m_actor.getCol()].getProp()==BlockRecord::WALL) return;
        if(blockView[m_actor.getRow()-2][m_actor.getCol()].getProp()==BlockRecord::BOX) return;
        if(blockView[m_actor.getRow()-2][m_actor.getCol()].getProp()==BlockRecord::HIT) return;


        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()-1][m_actor.getCol()],BlockRecord::MAN);
        putAimageOnPos(painter,blockView[m_actor.getRow()-2][m_actor.getCol()],BlockRecord::BOX);

        blockView[m_actor.getRow()-1][m_actor.getCol()].setProp(BlockRecord::AIM);
        blockView[m_actor.getRow()-2][m_actor.getCol()].setProp(BlockRecord::BOX);

    }
    /*地板-箱子*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
            &&blockView[m_actor.getRow()-1][m_actor.getCol()].getProp()==BlockRecord::BOX)
    {

        if(blockView[m_actor.getRow()-2][m_actor.getCol()].getProp()==BlockRecord::WALL) return;
        if(blockView[m_actor.getRow()-2][m_actor.getCol()].getProp()==BlockRecord::BOX) return;
        if(blockView[m_actor.getRow()-2][m_actor.getCol()].getProp()==BlockRecord::HIT) return;

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()-1][m_actor.getCol()],BlockRecord::MAN);
        putAimageOnPos(painter,blockView[m_actor.getRow()-2][m_actor.getCol()],BlockRecord::BOX);

        //更新所箱子在道具
        blockView[m_actor.getRow()-1][m_actor.getCol()].setProp(BlockRecord::FLOOR);

        /*箱子->目标*/
        if(blockView[m_actor.getRow()-2][m_actor.getCol()].getProp()==BlockRecord::AIM)
        {
            blockView[m_actor.getRow()-2][m_actor.getCol()].setProp(BlockRecord::HIT);
        }
        else
        {
            //更新箱子的道具
            blockView[m_actor.getRow()-2][m_actor.getCol()].setProp(BlockRecord::BOX);
        }

    }
    /*目标-地板*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()-1][m_actor.getCol()].getProp()==BlockRecord::FLOOR)
    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()-1][m_actor.getCol()],BlockRecord::MAN);
    }
    /*目标-目标*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()-1][m_actor.getCol()].getProp()==BlockRecord::AIM)
    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()-1][m_actor.getCol()],BlockRecord::MAN);
    }
    /*目标->箱子*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()-1][m_actor.getCol()].getProp()==BlockRecord::BOX)
    {
        /*箱子->墙*/
        if(blockView[m_actor.getRow()-2][m_actor.getCol()].getProp()==BlockRecord::WALL) return;
        /*箱子->箱子*/
        if(blockView[m_actor.getRow()-2][m_actor.getCol()].getProp()==BlockRecord::BOX) return;
        if(blockView[m_actor.getRow()-2][m_actor.getCol()].getProp()==BlockRecord::HIT) return;

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()-1][m_actor.getCol()],BlockRecord::MAN);
        putAimageOnPos(painter,blockView[m_actor.getRow()-2][m_actor.getCol()],BlockRecord::BOX);

        blockView[m_actor.getRow()-1][m_actor.getCol()].setProp(BlockRecord::FLOOR);
        blockView[m_actor.getRow()-2][m_actor.getCol()].setProp(BlockRecord::BOX);

    }
     m_actor.setBlock(m_actor.getRow()-1,m_actor.getCol(),m_actor.x(),m_actor.y()-SIDE,BlockRecord::MAN);


}

void Director::ActorMoveToDown(QPainter& painter)
{
    if(blockView[m_actor.getRow()+1][m_actor.getCol()].getProp()==BlockRecord::SUM) return;   /*前面是非游戏区*/
    if(blockView[m_actor.getRow()+1][m_actor.getCol()].getProp()==BlockRecord::WALL) return;  /*前面是墙*/

    /*地板-地板*/
    if(blockView[m_actor.getRow()+1][m_actor.getCol()].getProp()==BlockRecord::FLOOR
       &&blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR)

    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()+1][m_actor.getCol()],BlockRecord::MAN);
    }
    /*地板-目标*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
            &&blockView[m_actor.getRow()+1][m_actor.getCol()].getProp()==BlockRecord::AIM)
    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()+1][m_actor.getCol()],BlockRecord::MAN);
    }
    /*地板-击中*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
            &&blockView[m_actor.getRow()+1][m_actor.getCol()].getProp()==BlockRecord::HIT)
    {

        if(blockView[m_actor.getRow()+2][m_actor.getCol()].getProp()==BlockRecord::WALL) return;
        if(blockView[m_actor.getRow()+2][m_actor.getCol()].getProp()==BlockRecord::BOX) return;
        if(blockView[m_actor.getRow()+2][m_actor.getCol()].getProp()==BlockRecord::HIT) return;

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()+1][m_actor.getCol()],BlockRecord::MAN);
        putAimageOnPos(painter,blockView[m_actor.getRow()+2][m_actor.getCol()],BlockRecord::BOX);

        blockView[m_actor.getRow()+1][m_actor.getCol()].setProp(BlockRecord::AIM);
        blockView[m_actor.getRow()+2][m_actor.getCol()].setProp(BlockRecord::BOX);
    }
    /*地板-箱子*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
            &&blockView[m_actor.getRow()+1][m_actor.getCol()].getProp()==BlockRecord::BOX)
    {
        if(blockView[m_actor.getRow()+2][m_actor.getCol()].getProp()==BlockRecord::WALL) return;
        if(blockView[m_actor.getRow()+2][m_actor.getCol()].getProp()==BlockRecord::BOX) return;
        if(blockView[m_actor.getRow()+2][m_actor.getCol()].getProp()==BlockRecord::HIT) return;


        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()+1][m_actor.getCol()],BlockRecord::MAN);
        putAimageOnPos(painter,blockView[m_actor.getRow()+2][m_actor.getCol()],BlockRecord::BOX);

        //更新所箱子在道具
        blockView[m_actor.getRow()+1][m_actor.getCol()].setProp(BlockRecord::FLOOR);

        /*箱子->目标*/
        if(blockView[m_actor.getRow()+2][m_actor.getCol()].getProp()==BlockRecord::AIM)
        {
            blockView[m_actor.getRow()+2][m_actor.getCol()].setProp(BlockRecord::HIT);
        }
        else
        {
            //更新箱子的道具
            blockView[m_actor.getRow()+2][m_actor.getCol()].setProp(BlockRecord::BOX);
        }
    }
    /*目标-地板*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()+1][m_actor.getCol()].getProp()==BlockRecord::FLOOR)
    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()+1][m_actor.getCol()],BlockRecord::MAN);
    }
    /*目标-目标*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()+1][m_actor.getCol()].getProp()==BlockRecord::AIM)
    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()+1][m_actor.getCol()],BlockRecord::MAN);
    }
    /*目标->箱子*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()+1][m_actor.getCol()].getProp()==BlockRecord::BOX)
    {
        /*箱子->墙*/
        if(blockView[m_actor.getRow()+2][m_actor.getCol()].getProp()==BlockRecord::WALL) return;
        /*箱子->箱子*/
        if(blockView[m_actor.getRow()+2][m_actor.getCol()].getProp()==BlockRecord::BOX) return;

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()+1][m_actor.getCol()],BlockRecord::MAN);


        blockView[m_actor.getRow()+1][m_actor.getCol()].setProp(BlockRecord::FLOOR);
        blockView[m_actor.getRow()+2][m_actor.getCol()].setProp(BlockRecord::BOX);

        putAimageOnPos(painter,blockView[m_actor.getRow()+2][m_actor.getCol()],BlockRecord::BOX);
    }
     m_actor.setBlock(m_actor.getRow()+1,m_actor.getCol(),m_actor.x(),m_actor.y()+SIDE,BlockRecord::MAN);

}

void Director::ActorMoveToLeft(QPainter& painter)
{
    if(blockView[m_actor.getRow()][m_actor.getCol()-1].getProp()==BlockRecord::SUM) return;   /*前面是非游戏区*/
    if(blockView[m_actor.getRow()][m_actor.getCol()-1].getProp()==BlockRecord::WALL) return;  /*前面是墙*/

    /*地板-地板*/
    if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
       &&blockView[m_actor.getRow()][m_actor.getCol()-1].getProp()==BlockRecord::FLOOR)

    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()-1],BlockRecord::MAN);
    }
    /*地板-目标*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
            &&blockView[m_actor.getRow()][m_actor.getCol()-1].getProp()==BlockRecord::AIM)
    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()-1],BlockRecord::MAN);
    }
    /*地板-击中*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
            &&blockView[m_actor.getRow()][m_actor.getCol()-1].getProp()==BlockRecord::HIT)
    {
        if(blockView[m_actor.getRow()][m_actor.getCol()-2].getProp()==BlockRecord::BOX) return;
        if(blockView[m_actor.getRow()][m_actor.getCol()-2].getProp()==BlockRecord::WALL) return;
        if(blockView[m_actor.getRow()][m_actor.getCol()-2].getProp()==BlockRecord::HIT) return;

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()-1],BlockRecord::MAN);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()-2],BlockRecord::BOX);

        blockView[m_actor.getRow()][m_actor.getCol()-1].setProp(BlockRecord::AIM);
        blockView[m_actor.getRow()][m_actor.getCol()-2].setProp(BlockRecord::BOX);
    }
    /*地板-箱子*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
            &&blockView[m_actor.getRow()][m_actor.getCol()-1].getProp()==BlockRecord::BOX)
    {
        /*箱子->墙*/
        if(blockView[m_actor.getRow()][m_actor.getCol()-2].getProp()==BlockRecord::WALL) return;
        /*箱子->箱子*/
        if(blockView[m_actor.getRow()][m_actor.getCol()-2].getProp()==BlockRecord::BOX) return;

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()-1],BlockRecord::MAN);
        //更新所箱子在道具
        blockView[m_actor.getRow()][m_actor.getCol()-1].setProp(BlockRecord::FLOOR);

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()-2],BlockRecord::BOX);

        /*箱子->目标*/
        if(blockView[m_actor.getRow()][m_actor.getCol()-2].getProp()==BlockRecord::AIM)
        {
            blockView[m_actor.getRow()][m_actor.getCol()-2].setProp(BlockRecord::HIT);
        }
        else
        {
            //更新箱子的道具
            blockView[m_actor.getRow()][m_actor.getCol()-2].setProp(BlockRecord::BOX);
        }

    }
    /*目标-地板*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()][m_actor.getCol()-1].getProp()==BlockRecord::FLOOR)
    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()-1],BlockRecord::MAN);
    }
    /*目标-目标*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()][m_actor.getCol()-1].getProp()==BlockRecord::AIM)
    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()-1],BlockRecord::MAN);
    }
    /*目标-击中*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()][m_actor.getCol()-1].getProp()==BlockRecord::HIT)
    {
        if(blockView[m_actor.getRow()][m_actor.getCol()-2].getProp()==BlockRecord::BOX) return;
        if(blockView[m_actor.getRow()][m_actor.getCol()-2].getProp()==BlockRecord::WALL) return;
        if(blockView[m_actor.getRow()][m_actor.getCol()-2].getProp()==BlockRecord::HIT) return;

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()-1],BlockRecord::MAN);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()-2],BlockRecord::BOX);

        blockView[m_actor.getRow()][m_actor.getCol()-1].setProp(BlockRecord::AIM);
        if(blockView[m_actor.getRow()][m_actor.getCol()-2].getProp()==BlockRecord::AIM)
        {
            blockView[m_actor.getRow()][m_actor.getCol()-2].setProp(BlockRecord::HIT);
        }
        blockView[m_actor.getRow()][m_actor.getCol()-2].setProp(BlockRecord::BOX);

    }
    /*目标->箱子*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()][m_actor.getCol()-1].getProp()==BlockRecord::BOX)
    {
        /*箱子->墙*/
        if(blockView[m_actor.getRow()][m_actor.getCol()-2].getProp()==BlockRecord::WALL) return;
        /*箱子->箱子*/
        if(blockView[m_actor.getRow()][m_actor.getCol()-2].getProp()==BlockRecord::BOX) return;

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()-1],BlockRecord::MAN);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()-2],BlockRecord::BOX);

        blockView[m_actor.getRow()][m_actor.getCol()-1].setProp(BlockRecord::FLOOR);
        blockView[m_actor.getRow()][m_actor.getCol()-2].setProp(BlockRecord::BOX);

    }
     m_actor.setBlock(m_actor.getRow(),m_actor.getCol()-1,m_actor.x()-SIDE,m_actor.y(),BlockRecord::MAN);

}

void Director::ActorMoveToRight(QPainter& painter)
{
    if(blockView[m_actor.getRow()][m_actor.getCol()+1].getProp()==BlockRecord::SUM) return;   /*前面是非游戏区*/
    if(blockView[m_actor.getRow()][m_actor.getCol()+1].getProp()==BlockRecord::WALL) return;  /*前面是墙*/

    /*地板-地板*/
    if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
       &&blockView[m_actor.getRow()][m_actor.getCol()+1].getProp()==BlockRecord::FLOOR)

    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()+1],BlockRecord::MAN);
    }
    /*地板-目标*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
            &&blockView[m_actor.getRow()][m_actor.getCol()+1].getProp()==BlockRecord::AIM)
    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()+1],BlockRecord::MAN);

    }
    /*地板-箱子*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
            &&blockView[m_actor.getRow()][m_actor.getCol()+1].getProp()==BlockRecord::BOX)
    {

        if(blockView[m_actor.getRow()][m_actor.getCol()+2].getProp()==BlockRecord::WALL) return;
        if(blockView[m_actor.getRow()][m_actor.getCol()+2].getProp()==BlockRecord::BOX) return;
        if(blockView[m_actor.getRow()][m_actor.getCol()+2].getProp()==BlockRecord::HIT) return;

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()+1],BlockRecord::MAN);

        //更新所箱子在道具
        blockView[m_actor.getRow()][m_actor.getCol()+1].setProp(BlockRecord::FLOOR);

        //更新箱子的道具
        blockView[m_actor.getRow()][m_actor.getCol()+2].setProp(BlockRecord::BOX);

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()+2],BlockRecord::BOX);
    }
    /*地板-击中*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::FLOOR
            &&blockView[m_actor.getRow()][m_actor.getCol()+1].getProp()==BlockRecord::HIT)
    {
        if(blockView[m_actor.getRow()][m_actor.getCol()+2].getProp()==BlockRecord::BOX) return;
        if(blockView[m_actor.getRow()][m_actor.getCol()+2].getProp()==BlockRecord::WALL) return;
        if(blockView[m_actor.getRow()][m_actor.getCol()+2].getProp()==BlockRecord::HIT) return;

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::FLOOR);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()+1],BlockRecord::MAN);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()+2],BlockRecord::BOX);

        blockView[m_actor.getRow()][m_actor.getCol()+1].setProp(BlockRecord::AIM);
        blockView[m_actor.getRow()][m_actor.getCol()+2].setProp(BlockRecord::BOX);
    }
    /*目标-地板*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()][m_actor.getCol()+1].getProp()==BlockRecord::FLOOR)
    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()+1],BlockRecord::MAN);

    }
    /*目标-目标*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()][m_actor.getCol()+1].getProp()==BlockRecord::AIM)
    {
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()+1],BlockRecord::MAN);

    }
    /*目标->箱子*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()][m_actor.getCol()+1].getProp()==BlockRecord::BOX)
    {
        /*箱子->墙*/
        if(blockView[m_actor.getRow()][m_actor.getCol()+2].getProp()==BlockRecord::WALL) return;
        /*箱子->箱子*/
        if(blockView[m_actor.getRow()][m_actor.getCol()+2].getProp()==BlockRecord::BOX) return;

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()+1],BlockRecord::MAN);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()+2],BlockRecord::BOX);

        blockView[m_actor.getRow()][m_actor.getCol()+1].setProp(BlockRecord::FLOOR);
        blockView[m_actor.getRow()][m_actor.getCol()+2].setProp(BlockRecord::BOX);

    }
    /*目标-击中*/
    else if(blockView[m_actor.getRow()][m_actor.getCol()].getProp()==BlockRecord::AIM
            &&blockView[m_actor.getRow()][m_actor.getCol()+1].getProp()==BlockRecord::HIT)
    {
        if(blockView[m_actor.getRow()][m_actor.getCol()+2].getProp()==BlockRecord::BOX) return;
        if(blockView[m_actor.getRow()][m_actor.getCol()+2].getProp()==BlockRecord::WALL) return;
        if(blockView[m_actor.getRow()][m_actor.getCol()+2].getProp()==BlockRecord::HIT) return;

        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()],BlockRecord::AIM);
        putAimageOnPos(painter,blockView[m_actor.getRow()][m_actor.getCol()+1],BlockRecord::MAN);
    }
    m_actor.setBlock(m_actor.getRow(),m_actor.getCol()+1,m_actor.x()+SIDE,m_actor.y(),BlockRecord::MAN);

}

bool Director::isCanMoveMonster(MOVE_DIR dir)
{
    switch (dir)
    {
    case UP:
        if(blockView[m_monster.getRow()-1][m_monster.getCol()].getProp()==BlockRecord::FLOOR
            &&isTimeout)
        {
            return true;
        }
        else {
            return false;
        }
        break;
    case DOWN:
        if(/*blockView[m_monster.getRow()+1][m_monster.getCol()].getProp()==BlockRecord::AIM*/
            blockView[m_monster.getRow()+1][m_monster.getCol()].getProp()==BlockRecord::FLOOR&&isTimeout)
        {
            return true;
        }
        else {
            return false;
        }
        break;
    case LEFT:
        if(/*blockView[m_monster.getRow()][m_monster.getCol()-1].getProp()==BlockRecord::AIM
           ||*/blockView[m_monster.getRow()][m_monster.getCol()-1].getProp()==BlockRecord::FLOOR&&isTimeout)
        {
            return true;
        }
        else {
            return false;
        }
        break;
    case RIGHT:
        if(/*blockView[m_monster.getRow()][m_monster.getCol()+1].getProp()==BlockRecord::AIM
           ||*/blockView[m_monster.getRow()][m_monster.getCol()+1].getProp()==BlockRecord::FLOOR&&isTimeout)
        {
            return true;
        }
        else {
            return false;
        }
        break;

    default:
        break;
    }
}

void Director::moveMonster(QPainter &painter)
{
    switch (m_monster.getDir())
    {
    case UP:
        if(isCanMoveMonster(UP))
        {
            putAimageOnPos(painter,blockView[m_monster.getRow()][m_monster.getCol()],BlockRecord::FLOOR);
            putAimageOnPos(painter,blockView[m_monster.getRow()-1][m_monster.getCol()],BlockRecord::MONSTER);
            m_monster.setBlock(m_monster.getRow()-1,m_monster.getCol(),m_monster.x(),m_monster.y()-SIDE,BlockRecord::MONSTER);
        }
        else
        {
            m_monster.changeDir(DOWN);
            isTimeout=true;
        }
        break;
    case DOWN:
        if(isCanMoveMonster(DOWN))
        {
            putAimageOnPos(painter,blockView[m_monster.getRow()][m_monster.getCol()],BlockRecord::FLOOR);
            putAimageOnPos(painter,blockView[m_monster.getRow()+1][m_monster.getCol()],BlockRecord::MONSTER);
            m_monster.setBlock(m_monster.getRow()+1,m_monster.getCol(),m_monster.x(),m_monster.y()+SIDE,BlockRecord::MONSTER);
        }
        else
        {
            m_monster.changeDir(UP);
            isTimeout=true;
        }
        break;
    case LEFT:
        if(isCanMoveMonster(LEFT))
        {
            putAimageOnPos(painter,blockView[m_monster.getRow()][m_monster.getCol()],BlockRecord::FLOOR);
            putAimageOnPos(painter,blockView[m_monster.getRow()][m_monster.getCol()-1],BlockRecord::MONSTER);
            m_monster.setBlock(m_monster.getRow(),m_monster.getCol()-1,m_monster.x()-SIDE,m_monster.y(),BlockRecord::MONSTER);
        }
        else
        {
            m_monster.changeDir(RIGHT);
            isTimeout=true;
        }
        break;
    case RIGHT:
        if(isCanMoveMonster(RIGHT))
        {
            putAimageOnPos(painter,blockView[m_monster.getRow()][m_monster.getCol()],BlockRecord::FLOOR);
            putAimageOnPos(painter,blockView[m_monster.getRow()][m_monster.getCol()+1],BlockRecord::MONSTER);
            m_monster.setBlock(m_monster.getRow(),m_monster.getCol()+1,m_monster.x()+SIDE,m_monster.y(),BlockRecord::MONSTER);
        }
        else
        {
            m_monster.changeDir(LEFT);
            isTimeout=true;
        }
        break;
    default:
        break;
    }
    if(isFilaure())
    {
        painter.drawPixmap(170,180,img[BlockRecord::GAMEFILURE].width(),img[BlockRecord::GAMEFILURE].height(),
                img[BlockRecord::GAMEFILURE]);
        isTimeout=false;
        pause();
    }
    this->update();
    isTimeout=false;

}

void Director::putAimageOnPos(QPainter &painter, BlockRecord desView, BlockRecord::PROPS prop)
{
    painter.drawPixmap(desView.x(),desView.y(),SIDE,SIDE,img[prop]);
}

bool Director::isSuccess()
{
    bool isOver=true;
    for (int row = 0; row < 9; row++)
    {
        for (int col = 0; col < 12; col++)
        {
            if(blockView[row][col].getProp() ==BlockRecord::SUM) continue;
            if (blockView[row][col].getProp() ==BlockRecord::BOX)
            {
                isOver = false;
                break;
            }
        }
    }
    return isOver;
}

bool Director::isFilaure()
{
    if(m_monster.getRow()==m_actor.getRow())
    {
        if(m_monster.getCol()==m_actor.getCol())
        {
            return true;
        }
    }
    return false;
}

void Director::pause()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
        while (QTime::currentTime() < dieTime)
            QApplication::processEvents(QEventLoop::AllEvents, 100);
}

void Director::initProps()
{
    img[BlockRecord::WALL].load(":/res/wall_right.bmp");
    img[BlockRecord::FLOOR].load(":/res/floor.bmp");
    img[BlockRecord::BOX].load(":/res/box.bmp");
    img[BlockRecord::AIM].load(":/res/des.bmp");
    img[BlockRecord::MAN].load(":/res/man.bmp");
    img[BlockRecord::HIT].load(":/res/box.bmp");

    img[BlockRecord::GAMEOVER].load(":/res/success.jpg");
    img[BlockRecord::MONSTER].load(":/res/monster.png");
    img[BlockRecord::MONSTER]=img[BlockRecord::MONSTER].scaled(SIDE,SIDE);

    img[BlockRecord::BACK].load(":/res/blackground.bmp");
    img[BlockRecord::GAMEFILURE].load(":/res/failure.jpg");
}
void Director::setCheck(int check)
{
    m_curCheck=check;
    setWindowTitle(QString("推箱子-第%1关").arg(m_curCheck+1));
    initScene();
    this->update();
}

void Director::onMenuBtnClicked(bool)
{
    m_game->show();
}

void Director::onHelpBtnClicked(bool)
{
    m_illustrate.show();
}

void Director::onTimeOut()
{
    isTimeout=true;
    m_monster.changeDir();

}
