#ifndef BLOCKRECORD_H
#define BLOCKRECORD_H

/*方格信息-可作为基类*/
class BlockRecord
{
public:
    BlockRecord();

    enum  PROPS//道具
    {
        WALL,//墙0
        FLOOR,//地板1
        BOX,//箱子2
        AIM,//目标3
        MAN,//小人4
        HIT,//箱子推到里目的地
        BACK,//背景5
        GAMEOVER,
        MONSTER,
        SUM,//6
        GAMEFILURE
    };
    void setBlock(int _row,int _col,int _x,int _y,PROPS  prop);  /*初始化信息*/
    int x();      /*得到x坐标*/
    int y();      /*得到y坐标*/
    int getRow();  /*得到行坐标*/
    int getCol();  /*得到列坐标*/

    PROPS getProp();      /*得到道具*/
    void setProp(PROPS prop);
protected:
    int row;    //行坐标  做判断的时候用
    int col;    //列坐标
    int pixel_x;    //像素x坐标  贴图用
    int pixel_y;    //像素y坐标
    PROPS  prop;        /*道具*/

};

#endif // BLOCKRECORD_H
