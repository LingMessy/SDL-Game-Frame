#include "sgf.h"
#include "sgftools.h"

//游戏初始化
int game_init(Sgf_interface* sgf_intf)
{
	// GBKToUTF8("没事多造轮子", -1, sgf_intf->title, TITLE_MAX_LEN);

	return 0;
}

//加载游戏资源
int game_load_resources(Sgf_interface* sgf_intf)
{

	return 0;
}


// 游戏事件处理
void game_event_processing(SDL_Event* evt)
{
}

// 游戏更新
int game_update(Sgf_interface* sgf_intf, float ms)
{
	return 0;
}


// 游戏渲染
void game_render(Sgf_interface* sgf_intf, float lag)
{

}


// 退出游戏前进行反初始化
void game_quit(Sgf_interface* sgf_intf)
{

}
