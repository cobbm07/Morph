#include "game.h"

/*
* Driver file simply starts and runs the game, and cleans it when the game is lost, won, or otherwise closed
* The only thing of note is the 60fps lock, which ensures that each frame takes the same amount of time
* Without this lock, game object speeds would be inconsistent, and in testing the player would move at insane speeds around the map
* Fun to play with but impractical
*/

Game* game = nullptr;

int main(int argc, char* argv[]) {

	const int FPS = 60;
	const int FRAME_DELAY = 1000 / FPS;
	
	Uint32 frame_start;
	int frame_time;

	game = new Game();
	game->init("Morph", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, false);

	while (game->running()) {
		frame_start = SDL_GetTicks();

		game->handle_events();
		game->update();
		game->render();

		// Lock game to 60 fps
		frame_time = SDL_GetTicks() - frame_start;
		if (FRAME_DELAY > frame_time) {
			SDL_Delay(FRAME_DELAY - frame_time);
		}
	}

	game->clean();

	if (Game::won) {
		std::cout << "Congrats! You've won the game!" << std::endl;
	}
	else if (Game::lost) {
		std::cout << "Sorry! You've failed the game! Please try again!" << std::endl;
	}

	return 0;
}