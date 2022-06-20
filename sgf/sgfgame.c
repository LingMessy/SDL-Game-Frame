#include "sgfgame.h"
#include "sgftools.h"
#include <stdio.h>
#include <string.h>

#define DEFAULT_WINDOW_NAME ("SGFWin")
const int DEFAULT_GAME_UPDATE_FREQUENCY = 60;
const int DEFAULT_WINDOW_WIDTH = 1280;
const int DEFAULT_WINDOW_HEIGHT = 720;

// 清理渲染器
static void renderer_clear(Sgf_interface* sgf_intf)
{
    if (system) {
        SDL_SetRenderDrawColor(sgf_intf->renderer, 0, 0, 0, 255);
        SDL_RenderClear(sgf_intf->renderer);
    }
}


// FPS
static int render_fps(Sgf_interface* sgf_intf, float elapsed)
{
    static float _total_time = 0;
    static int _fps = 0;
    static int _fps_count = 0;

    _total_time += elapsed;

    if (_total_time >= 1000.0f) {
        _total_time -= 1000.0f;
        _fps = _fps_count;
        _fps_count = 1;
    } else {
        _fps_count++;
    }

    // 计算并设置文字大小
    char fps_text[256] = { 0 };
    snprintf(fps_text, 255, "FPS: %d", _fps);
    SDL_Rect dst = { 0, 0, 100, 21 };
    int ret = TTF_SizeText(sgf_intf->font, fps_text, &(dst.w), &(dst.h));
    if (ret) {
        SDL_Log("get text size failed: %s\n", TTF_GetError());
    }

    // 渲染文字
    SDL_Color color = { 255, 255, 255, 255 };
    if (sgf_intf != NULL) {
        SDL_Surface* surf = TTF_RenderText_Blended(sgf_intf->font, fps_text, color);
        if (surf != NULL) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(sgf_intf->renderer, surf);
            if (texture) {
                SDL_RenderCopy(sgf_intf->renderer, texture, NULL, &dst);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surf);
        }
    }

    return 0;
}



// 清理sgf接口
static void sgf_interface_clear(Sgf_interface* sgf_intf)
{
    SDL_assert(sgf_intf != NULL);

    if (sgf_intf->font != NULL) {
        TTF_CloseFont(sgf_intf->font);
        sgf_intf->font = NULL;
    }

    if (sgf_intf->renderer != NULL) {
        SDL_DestroyRenderer(sgf_intf->renderer);
        sgf_intf->renderer = NULL;
    }

    if (sgf_intf->window != NULL) {
        SDL_DestroyWindow(sgf_intf->window);
        sgf_intf->window = NULL;
    }

    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    if (sgf_intf != NULL) {
        free(sgf_intf);
    }
}

// 初始化及加载默认资源
Sgf_interface* sgf_interface_init()
{
    int ret = 0;

    // 创建接口
    Sgf_interface* sgf_intf = (Sgf_interface*)malloc(sizeof(Sgf_interface));
    if (sgf_intf == NULL) {
        SDL_Log("malloc unable to allocate memory\n");
        return NULL;
    }

    // 接口填充默认值
    memset(sgf_intf, 0, sizeof(sgf_intf));
    // strcpy_s(pModule->usr.title, strlen(APP_WIN_NAME) + 1, APP_WIN_NAME);

    strncpy(sgf_intf->title, DEFAULT_WINDOW_NAME, sizeof(sgf_intf->title));
    sgf_intf->title[sizeof(sgf_intf->title) - 1] = '\0'; // 手动截断字符串
    sgf_intf->window_resolution.x = DEFAULT_WINDOW_WIDTH;
    sgf_intf->window_resolution.y = DEFAULT_WINDOW_HEIGHT;
    sgf_intf->game_update_frequency = DEFAULT_GAME_UPDATE_FREQUENCY;
    sgf_intf->show_fps = 1;

    // SDL初始化
    ret = SDL_Init(SDL_INIT_EVERYTHING);
    if (ret != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        sgf_interface_clear(sgf_intf);
        return NULL;
    }

    // TTF字体初始化
    ret = TTF_Init();
    if (ret != 0) {
        SDL_Log("Unable to initialize TTF: %s\n", SDL_GetError());
        sgf_interface_clear(sgf_intf);
        return NULL;
    }

    //初始化 Image
    int flags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_WEBP;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) {
        SDL_Log("Failed to init required jpg and png and webp support: %s\n", IMG_GetError());
        sgf_interface_clear(sgf_intf);
        return NULL;
    }

    //初始化 mixer
    flags = MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MID | MIX_INIT_OPUS;
    initted = Mix_Init(flags);
    if ((initted & flags) != flags) {
        SDL_Log("Failed to init required flag and mod and mp3 and ogg and mid and opus support: %s\n", Mix_GetError());
        sgf_interface_clear(sgf_intf);
        return NULL;
    }

    // 打开音频
    ret = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048);
    if (ret != 0) {
        SDL_Log("Mix_OpenAudio failed: %s\n", Mix_GetError());
        sgf_interface_clear(sgf_intf);
        return NULL;
    }

    //用户初始化
    ret = game_init(sgf_intf);
    if (ret != 0) {
        sgf_interface_clear(sgf_intf);
        return NULL;
    }

    //创建窗口
    sgf_intf->window = SDL_CreateWindow(sgf_intf->title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        sgf_intf->window_resolution.x, sgf_intf->window_resolution.y, 0);
    if (sgf_intf->window == NULL) {
        SDL_Log("Could not create window: %s\n", SDL_GetError());
        sgf_interface_clear(sgf_intf);
        return NULL;
    }

    // XXX
    // 可以改成从内存加载资源
    // 设置图标
    SDL_Surface* icon = IMG_Load("./resources/icon/icon.png");
    if (icon == NULL) {
        SDL_Log("Load icon.png error: %s\n", SDL_GetError());
        sgf_interface_clear(sgf_intf);
        return NULL;
    }
    SDL_SetWindowIcon(sgf_intf->window, icon);
    SDL_FreeSurface(icon);

    //创建渲染器
    sgf_intf->renderer = SDL_CreateRenderer(sgf_intf->window, -1, SDL_RENDERER_ACCELERATED);
    if (sgf_intf->renderer == NULL) {
        SDL_Log("Could not create renderer: %s\n", SDL_GetError());
        sgf_interface_clear(sgf_intf);
        return NULL;
    }

    // XXX
    // 可以改成从内存加载资源
    // 加载字体
    sgf_intf->font = TTF_OpenFont("./resources/fonts/simhei.ttf", 18);
    if (sgf_intf->font == NULL) {
        SDL_Log("Could not open font: %s\n", SDL_GetError());
        sgf_interface_clear(sgf_intf);
        return NULL;
    }

    // 用户加载游戏资源
    ret = game_load_resources(sgf_intf);
    if (ret != 0) {
        sgf_interface_clear(sgf_intf);
        return NULL;
    }

    return sgf_intf;
}

// 游戏运行
void game_run(Sgf_interface* sgf_intf)
{
    SDL_assert(sgf_intf != NULL);

    int game_update_frequency = DEFAULT_GAME_UPDATE_FREQUENCY;
    if (sgf_intf->game_update_frequency > 0) {
        game_update_frequency = sgf_intf->game_update_frequency;
    }

    int quit = 0;
    SDL_Event evt;

    // FPS相关
    Uint64 frequency, prev_count, now_count;
    float frame_elapsed_time = 0.0f; // 前后两帧画面的耗时(ms)
    float lag = 0.0f; // 画面更新与游戏更新之间的延迟
    float game_update_elapsed = 1000.0f / game_update_frequency; // 游戏更新时长

    frequency = SDL_GetPerformanceFrequency();
    prev_count = SDL_GetPerformanceCounter();

    while (!quit) {
        if (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) {
                quit = 1;
            } else {
                game_event_processing(&evt);
            }
        } else {

            now_count = SDL_GetPerformanceCounter();

            //前后两帧画面的耗时(ms)
            frame_elapsed_time = (now_count - prev_count) * 1000.0f / frequency;

            prev_count = now_count;

            lag += frame_elapsed_time;

            if (sgf_intf != NULL) {
                //更新游戏
                while (lag >= game_update_elapsed) {
                    game_update(sgf_intf, game_update_elapsed);
                    lag -= game_update_elapsed;
                }

                //清除背景
                renderer_clear(sgf_intf);

                //渲染图像
                game_render(sgf_intf, lag);

                //渲染帧率
                if (sgf_intf->show_fps) {
                    render_fps(sgf_intf, frame_elapsed_time);
                }

                // 显示图像（刷新缓冲）
                SDL_RenderPresent(sgf_intf->renderer);
            }
        }
    }

    //清理
    if (sgf_intf != NULL) {
        game_quit(sgf_intf);
        sgf_interface_clear(sgf_intf);
    }
}
