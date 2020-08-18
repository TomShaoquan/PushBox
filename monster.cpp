#include "monster.h"
#include<time.h>
#include<math.h>

Monster::Monster()
{
    srand((unsigned int)time(NULL));
    m_dir=(MOVE_DIR)(rand()%4);

}

void Monster::changeDir()
{
    m_dir=(MOVE_DIR)(rand()%4);
}

void Monster::changeDir(MOVE_DIR dir)
{
    m_dir=dir;
}

MOVE_DIR Monster::getDir()
{
    return m_dir;
}
