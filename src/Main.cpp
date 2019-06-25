#include <iostream>

#include <thread>
#include <mutex>
#include <vector>
#include <SDL.h>

#include "MathFunctions.h"

bool StartSDL();
void TerminateProgram(int exitCode);
void ClearBackground(SDL_Renderer* renderer, SDL_Texture* texture, Uint32 clearColor);
void ShowBuffer(SDL_Renderer* renderer, SDL_Texture* texture);

void DrawSphereScene(SDL_Renderer* renderer, SDL_Texture* texture);

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* frontBuffer;

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;

double sphereX = 0.0;
double sphereY = 0.0;
double sphereZ = 3.0;
double sphereR = 1.0;

int main(int argc, char* argv[]) {
	if (!StartSDL()) {
		std::cerr << "Could not start SDL!\n";
		return 1;
	}

	int frameCount = 0;

	SDL_Event event;

	while (true) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				TerminateProgram(0);
			} else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_UP) {
					sphereY += 0.1;
				} else if (event.key.keysym.sym == SDLK_DOWN) {
					sphereY -= 0.1;
				} else if (event.key.keysym.sym == SDLK_LEFT) {
					sphereX -= 0.1;
				} else if (event.key.keysym.sym == SDLK_RIGHT) {
					sphereX += 0.1;
				} else if (event.key.keysym.sym == SDLK_w) {
					sphereZ += 0.1;
				} else if (event.key.keysym.sym == SDLK_s) {
					sphereZ -= 0.1;
				} else if (event.key.keysym.sym == SDLK_a) {
					sphereR -= 0.1;
				} else if (event.key.keysym.sym == SDLK_d) {
					sphereR += 0.1;
				}
			}
		}

		ClearBackground(renderer, frontBuffer, 0xFF101000);
		DrawSphereScene(renderer, frontBuffer);
		ShowBuffer(renderer, frontBuffer);

		std::cout << "Presenting frame " << frameCount + 1 << "\n";
		++frameCount;
		SDL_Delay(20);
	}
	TerminateProgram(0);
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
	window = SDL_CreateWindow("Raycaster", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (window == nullptr) {
		std::cerr << SDL_GetError() << "\n";
		return false;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		std::cerr << SDL_GetError() << "\n";
		return false;
	}
	frontBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (frontBuffer == nullptr) {
		std::cerr << SDL_GetError() << "\n";
		return false;
	}

	return true;
}

/// <summary>
/// Cleans up SDL and immediately exits the program.
/// </summary>
/// <param name="exitCode"></param>
void TerminateProgram(int exitCode) {
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
	exit(exitCode);
}

/// <summary>
/// Sets the texture as the render target and clears it with the specified color (in ABGR format).
/// </summary>
/// <param name="renderer"></param>
/// <param name="texture"></param>
/// <param name="clearColor"></param>
void ClearBackground(SDL_Renderer* renderer, SDL_Texture* texture, Uint32 clearColor) {
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, clearColor & 0xFF, clearColor & 0xFF00 >> 2, clearColor & 0xFF0000 >> 4, clearColor & 0xFF000000 >> 6);
	SDL_RenderClear(renderer);
}

/// <summary>
/// Sets the render target to the renderer's screen, copies the texture to it, and presents it to the screen.
/// </summary>
/// <param name="renderer"></param>
/// <param name="texture"></param>
void ShowBuffer(SDL_Renderer* renderer, SDL_Texture* texture) {
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, frontBuffer, NULL, NULL);
	SDL_RenderPresent(renderer);
}

/// <summary>
/// Draws a test sphere with the raycaster.
/// The sphere has a one unit radius, and is positioned at (0, 0, -3). The camera is positioned at (0, 0, 0) and looks
/// directly at the sphere. The sphere is perfectly white, but light values linearly drop to half brightness depending
/// on the angle from the normal of the face.
/// </summary>
/// <param name="renderer"></param>
/// <param name="texture"></param>
void DrawSphereScene(SDL_Renderer* renderer, SDL_Texture* texture) {
	std::vector<std::thread> threads;
	std::mutex sdlLock;
	for (int threadNum = 0; threadNum < static_cast<int>(std::thread::hardware_concurrency()); ++threadNum) {
		threads.emplace_back([renderer, texture, threadNum, &sdlLock]() {
			// The field of view in degrees. This is the angle between the highest and lowest rays on screen.
			constexpr Degrees vFov = 75.0;
			// The aspect ratio of the display in width:height ratio.
			constexpr double aspectRatio = static_cast<double>(WINDOW_WIDTH) / WINDOW_HEIGHT;
			// The field of view horizontally. This is calculated based on the vertical fov and the aspect ratio.
			constexpr Degrees hFov = vFov * aspectRatio;

			for (int screenY = threadNum; screenY < WINDOW_HEIGHT; screenY += static_cast<int>(std::thread::hardware_concurrency())) {
				// The gradient of the Y axis of the ray (i.e. number of Y units moved per Z unit).
				double yGradient = std::tan(ToRadians(Lerp(vFov / 2.0, vFov / -2.0, ((screenY + 1ll) / (2.0 * WINDOW_HEIGHT)))));
				for (int screenX = 0; screenX < WINDOW_WIDTH; ++screenX) {
					double xGradient = std::tan(ToRadians(Lerp(hFov / -2.0, hFov / 2.0, ((screenX + 1ll) / (2.0 * WINDOW_WIDTH)))));
					// Now to calculate the intersection point of the line and the sphere.
					// This can be solved with some algerbraic manipulation and the quadratic formula.
					// You get no solution if the sphere contains the origin. Otherwise you're likely to get two solutions,
					// the lower value will be the closer one, and therefore the one you see.
					double quadPart1 = -(2 * xGradient * sphereX - 2 * yGradient * sphereY - 2 * sphereZ);
					double quadPart2 = 4 * (xGradient * xGradient + yGradient * yGradient + 1) * (sphereX * sphereX + sphereY * sphereY + sphereZ * sphereZ - sphereR * sphereR);
					double quadPart3 = 2 * (xGradient * xGradient + yGradient * yGradient + 1);
					if (quadPart2 < 0.0) {
						continue;
					}
					double z1 = (quadPart1 + std::sqrt(quadPart2)) / quadPart3;
					double z2 = (quadPart1 - std::sqrt(quadPart2)) / quadPart3;
					if (IsBetween(z1, z2, 0.0)) {
						continue;
					}
					double z = /*Min(z1, z2)*/z1;
					double x = xGradient * z;
					double y = yGradient * z;

					// Now determine the brightness. We use the cosine rule to give us the angle, and then determine the
					// brightness from that.
					double dotProduct = (sphereX - x) * sphereX + (sphereY - y) * sphereY + (sphereZ - z) * sphereZ;
					double cosineValue = dotProduct / Pythagoras(sphereX, sphereY, sphereZ) / sphereR;
					double brightness = 0.5 * Clamp(0.0, 1.0, cosineValue) + 0.5;

					sdlLock.lock();
					SDL_SetRenderDrawColor(renderer, static_cast<Uint8>(brightness * 255), static_cast<Uint8>(brightness * 255), static_cast<Uint8>(brightness * 255), SDL_ALPHA_OPAQUE);
					SDL_RenderDrawPoint(renderer, screenX, screenY);
					sdlLock.unlock();
				}
			}
		});
	}
	for (int threadNum = 0; threadNum < static_cast<int>(std::thread::hardware_concurrency()); ++threadNum) {
		threads[threadNum].join();
	}
}

