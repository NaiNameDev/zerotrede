#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <SDL2/SDL.h>

#include "dynamic.c"

#define WIDTH 640
#define HEIGHT 480

#include "mymath.c"
DEFINE_DYNAMYC_TYPE(vec3)
DEFINE_DYNAMYC_TYPE(char)
DEFINE_DYNAMYC_TYPE(uint8_t);
#include "rasterizer.c"
#include "mesh.c"

int main() {
	vec3 t1 = nvec3(0.0f, 1.0f, 0.0f);
	vec3 t2 = nvec3(1.0f, -1.0f, 0.0f);
	vec3 t3 = nvec3(-1.0f, -1.0f, 0.0f);
	
	vec3 c1 = nvec3(1.0f, 0.0f, 0.0f);
	vec3 c2 = nvec3(0.0f, 1.0f, 0.0f);
	vec3 c3 = nvec3(0.0f, 0.0f, 1.0f);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow(
		"Window",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WIDTH,
		HEIGHT,
		SDL_WINDOW_SHOWN
	);

	if (window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }



	SDL_Event e;
	uint32_t st = SDL_GetTicks();
	SDL_Surface* sr = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
	
	mat4 to_screen = to_screen_mat4(WIDTH, HEIGHT);
	mat4 proj = projection_mat4(WIDTH/HEIGHT, deg2rad(90.0f), 100.0f, 0.1f);
	mat4 view = nmat4();

	dynamic_vec3 v = malloc_vec3(3);
	v.arr[0] = t1;
	v.arr[1] = t2;
	v.arr[2] = t3;
	mesh ttg = nmesh(v, v, nvec3(0,0,-2.0f), nvec3(0,0,0), nvec3(1,1,1));
	
	while (1) {
		uint32_t msec = SDL_GetTicks() - st;
		//if(msec > 0) printf("fps: %f\n", 1000.0 / (double) msec);
		st = SDL_GetTicks();

		SDL_SetRenderDrawColor(renderer,0,0,0,255);
		SDL_RenderClear(renderer);
		if (SDL_PollEvent(&e) && e.type == SDL_QUIT) break;

		ttg.position.x = sin(SDL_GetTicks() / 1000.0f) * 4.0f;
		ttg.position.z = cos(SDL_GetTicks() / 1000.0f) * 4.0f - 6;
		
		dynamic_vec3 p = project_vertices(&ttg, view, proj, to_screen);
		for (int i = 0; i < p.size; i+=3) {
			draw_trg(p.arr[i], p.arr[i+1], p.arr[i+2], c1, c2, c3, sr);
		}
		dealloc_vec3(&p);


		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, sr);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(texture);
	}
	
	dealloc_vec3(&v);

	SDL_FreeSurface(sr);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();	
}
