#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <SDL2/SDL.h>

#include "dynamic.c"

#define WIDTH 640
#define HEIGHT 640
#define NEAR 0.1f
#define FAR 100.0f
#define FOV 90.0f

#include "mymath.c"
DEFINE_DYNAMYC_TYPE(vec4)
DEFINE_DYNAMYC_TYPE(char)
DEFINE_DYNAMYC_TYPE(float)
DEFINE_DYNAMYC_TYPE(uint8_t);
#include "rasterizer.c"
#include "mesh.c"

int main() {
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
	mat4 proj = projection_mat4(WIDTH/HEIGHT, deg2rad(FOV), FAR, NEAR);

	mesh a = create_mesh_from_obj("test_teto.obj");
	a.position.z = -3.0f;
	a.position.y = 2.5f;
	a.scale = nvec4(0.05f, 0.05f, 0.05f, 1.0f);
	a.rotation.x = deg2rad(180.0f);
	
	vec4 cam_pos = nvec4(0,0,0,1);
	while (1) {
		uint32_t msec = SDL_GetTicks() - st;
		if(msec > 0) printf("fps: %f\n", 1000.0 / (double) msec);
		st = SDL_GetTicks();

		SDL_SetRenderDrawColor(renderer,0,0,0,255);
		SDL_RenderClear(renderer);
		if (SDL_PollEvent(&e) && e.type == SDL_QUIT) break;
	

		SDL_LockSurface(sr);
		dynamic_uint8_t pix = malloc_uint8_t(sr->h * sr->pitch);
		dynamic_float depth = malloc_float(sr->h * sr->w);
		fill_zeros_uint8_t(&pix);
		fill_zeros_float(&depth);

		//a.rotation.x = st / 1000.0f;
		a.rotation.y = st / 2000.0f;
		//mat4 r = rotation_y_mat4(st/1000.0f);
		//cam_pos.y = cos(st / 1000.0f);
		//mat4 view = look_at_mat4(mulmat4vec4(r, nvec4(1,0,0,1)), mulmat4vec4(r, nvec4(0,1,0,1)), mulmat4vec4(r, nvec4(0,0,-1,1)), cam_pos);
		mat4 view = nmat4();

		draw(a, view, proj, to_screen, sr, pix, depth);

		memcpy(sr->pixels, pix.arr, sr->h * sr->pitch);
		dealloc_uint8_t(&pix);
		dealloc_float(&depth);
		SDL_UnlockSurface(sr);
		

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, sr);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(texture);
	}
	
	free_mesh(&a);

	SDL_FreeSurface(sr);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();	
}
