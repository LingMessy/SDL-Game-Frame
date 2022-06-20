# 源文件
SRC = game.c sgfgame.c sgfmain.c sgftools.c
# 目标文件
TARGET = main.exe
# SDL 库的位置
SDLDIR = ./SDL-lib
# 编译参数
CFLAGS = -I $(SDLDIR)/include/SDL2 -L $(SDLDIR)/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -O2 #-mwindows
# 选择编译器
CC = gcc.exe

$(TARGET): $(SRC)
	$(CC) $(SRC) $(CFLAGS) -o ./release/$(TARGET)

clean:
	del .\release\$(TARGET)