#include "blockrecord.h"

BlockRecord::BlockRecord()
{

}

void BlockRecord::setBlock(int _row, int _col, int _x, int _y, BlockRecord::PROPS prop)
{
    this->row=_row;
    this->col=_col;
    this->pixel_x=_x;
    this->pixel_y=_y;
    this->prop=prop;
}

int BlockRecord::x()
{
    return pixel_x;
}

int BlockRecord::y()
{
    return pixel_y;
}

int BlockRecord::getRow()
{
    return row;
}

int BlockRecord::getCol()
{
    return col;
}

BlockRecord::PROPS BlockRecord::getProp()
{
    return prop;
}

void BlockRecord::setProp(BlockRecord::PROPS prop)
{
    this->prop=prop;
}

