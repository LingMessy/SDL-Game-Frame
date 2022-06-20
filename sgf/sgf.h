#ifndef _SGF_H
#define _SGF_H

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

// 设置断言等级 2开启，0关闭
// #define SDL_ASSERT_LEVEL 2

//标题最大长度
#define TITLE_MAX_LEN 512

/*
640*360 360p 0.5K
1280*720 720p 1K
1920*1080 1080p 1.5K
2560*1440 2K
3840*2160 4K
7680*4320 8K
15360*8640 16K
*/

typedef enum
{
    RESOLUTION_0_5K_X = 640,
    RESOLUTION_0_5K_Y = 360,
    RESOLUTION_1K_X = 1280,
    RESOLUTION_1K_Y = 720,
    RESOLUTION_1_5K_X = 1920,
    RESOLUTION_1_5K_Y = 1080,
    RESOLUTION_2K_X = 2560,
    RESOLUTION_2K_Y = 1440,
    RESOLUTION_4K_X = 3840,
    RESOLUTION_4K_Y = 2160
} Game_resolution;

typedef struct
{
    int x;
    int y;
} Resolution;


// Sgf接口
typedef struct
{
	SDL_Window* window;				//窗口指针
	SDL_Renderer* renderer;		//渲染指针
	TTF_Font* font;				//默认字体指针
	char title[TITLE_MAX_LEN];		//游戏标题
	Resolution window_resolution;	//窗口大小
	unsigned int game_update_frequency;	// 游戏更新的频率
	int show_fps;					//是否显示 FPS
} Sgf_interface;



//游戏初始化
int game_init(Sgf_interface* sgf_intf);

//加载游戏资源
int game_load_resources(Sgf_interface* sgf_intf);

// 游戏事件处理
void game_event_processing(SDL_Event* evt);

// 游戏更新
int game_update(Sgf_interface* sgf_intf, float ms);

// 游戏渲染
void game_render(Sgf_interface* sgf_intf, float lag);

// 退出游戏前进行反初始化
void game_quit(Sgf_interface* sgf_intf);

#endif