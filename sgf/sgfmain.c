#include "sgfgame.h"

int main(int argc, char* argv[])
{
	Sgf_interface* game = sgf_interface_init();
	if (game == NULL) {
		return -1;
	}

	game_run(game);

	return 0;
}