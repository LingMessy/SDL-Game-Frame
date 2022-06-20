#ifndef SGF_GAME_H
#define SGF_GAME_H

#include "sgf.h"

//初始化
Sgf_interface* sgf_interface_init();

//运行
void game_run(Sgf_interface* interface);



#endif