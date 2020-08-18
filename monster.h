#ifndef MONSTER_H
#define MONSTER_H
#include"blockrecord.h"
#include"common.h"

class Monster : public BlockRecord
{
public:
    Monster();
    void changeDir();
    void changeDir(MOVE_DIR dir);
    MOVE_DIR getDir();

private:
    MOVE_DIR m_dir;
};

#endif // MONSTER_H
