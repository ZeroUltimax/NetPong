//Using SDL and standard IO
#include <SDL.h>
#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <ctime>
#include <string>
#include "Aabb.h"
#include "ReducingBb.h"

using namespace std;
using namespace ZeroUltimax;

void drawBoxe(AABB& box, Point& displacement, SDL_Renderer* render){

	SDL_Rect r{ displacement.x + box.left, displacement.y + box.top, box.right - box.left, box.bot - box.top };

	SDL_RenderDrawRect(render, &r);
}

void drawBoxes(const std::list<AABB>& boxes, Point& displacement, SDL_Renderer* render){
	
	vector<SDL_Rect> rectangles;

	for (AABB box : boxes){
		SDL_Rect r{ displacement.x + box.left, displacement.y + box.top, box.right - box.left, box.bot - box.top };
	
		rectangles.push_back(r);
	}

	SDL_RenderDrawRects(render, rectangles.data(), rectangles.size());
	
}

int main(int argc, char* args[]){

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else	{
		
	}

	SDL_Window* window;
	SDL_Renderer* render;

	int windowSize = 1000;

	SDL_CreateWindowAndRenderer(windowSize, windowSize, 0, &window, &render);

	SDL_SetWindowTitle(window, "Initializing");
	SDL_SetWindowPosition(window, 50, 50);
	SDL_Event event;

	bool continuePlay = true;

	int width = 1000;

	int disp = (windowSize - width)/2;

	ReducingBb reduce(AABB(0, width, 0, width));

	float granularity = 50;

	auto engine = mt19937(static_cast<uint_fast32_t> (time(NULL)));


	auto roll = bind(uniform_int_distribution<int>(0, width / granularity), engine);

	while (continuePlay){

		while (SDL_PollEvent(&event)){
			switch (event.type){
			case SDL_QUIT:
				continuePlay = false;
				break;
			case SDL_MOUSEMOTION:
			{
				Point hit{ event.motion.x - disp, event.motion.y - disp };
				if (reduce.contains(hit)){
					cout << "hit detected" << endl;
					reduce.hit(hit);
				}
			}
			}
		}

		Point hit{ roll()*granularity, roll()*granularity };
		if (reduce.contains(hit)){
			//cout << "Point (" << hit.x << ", " << hit.y << ")" << endl;
			reduce.hit(hit);

			//SDL_SetWindowTitle(window, to_string(reduce.area()).data());
		}

		SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
		SDL_RenderClear(render);

		SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
		drawBoxes(reduce.getCornerBoxes(), Point({ disp, disp }), render);

		SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
		drawBoxe(reduce.getBounds(), Point({ disp, disp }), render);

		SDL_RenderPresent(render);
	
	}

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}