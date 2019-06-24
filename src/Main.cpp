#include <iostream>
#include <SDL.h>

bool StartSDL();

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* frontBuffer;

int main(int argc, char* argv[]) {
	if (!StartSDL()) {
		std::cerr << "Could not start SDL!\n";
		return 1;
	}

	int frameCount = 0;

	SDL_Event event;

	while (true) {
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			break;
		}

		SDL_SetRenderTarget(renderer, frontBuffer);
		SDL_SetRenderDrawColor(renderer, frameCount % 3 == 0 ? 255 : 0, frameCount % 3 == 1 ? 255 : 0, frameCount % 3 == 2 ? 255 : 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		SDL_Rect screenPos;
		screenPos.x = 0;
		screenPos.y = 0;
		screenPos.w = 640;
		screenPos.h = 480;
		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, frontBuffer, NULL, NULL);
		std::cout << "Presenting frame " << frameCount + 1 << "\n";
		++frameCount;
		SDL_RenderPresent(renderer);
		SDL_Delay(1000);
	}

	if (frontBuffer != nullptr) {
		SDL_DestroyTexture(frontBuffer);
	}
	if (renderer != nullptr) {
		SDL_DestroyRenderer(renderer);
	}
	if (window != nullptr) {
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
	return 0;
}

/// <summary>
/// Starts up SDL with all the things necessary to create the visuals. This includes initialising the components,
/// creatinng the window, renderer, and texture used for rendering. Returns whether everything was successful or not,
/// and writes any errors to stderr if they occur.
/// </summary>
/// <returns>true if everything initialised, false if something failed</returns>
bool StartSDL() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cerr << SDL_GetError() << "\n";
		return false;
	}
	window = SDL_CreateWindow("Raycaster", 100, 100, 640, 480, 0);
	if (window == nullptr) {
		std::cerr << SDL_GetError() << "\n";
		return false;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		std::cerr << SDL_GetError() << "\n";
		return false;
	}
	frontBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 640, 480);
	if (frontBuffer == nullptr) {
		std::cerr << SDL_GetError() << "\n";
		return false;
	}

	return true;
}