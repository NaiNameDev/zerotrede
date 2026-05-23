#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include <GLFW/glfw3.h>

#include "dynamic.c"

#define WIDTH 1280.0f
#define HEIGHT 1280.0f
#define NEAR 1.0f
#define FAR 100.0f
#define FOV 90.0f

int is_key_pressed(GLFWwindow* win, int key) {
	return glfwGetKey(win, key) == GLFW_PRESS;
}

#include "mymath.c"
#include "camera.c"
DEFINE_DYNAMYC_TYPE(vec4)
DEFINE_DYNAMYC_TYPE(char)
DEFINE_DYNAMYC_TYPE(float)
DEFINE_DYNAMYC_TYPE(uint8_t);
#include "rasterizer.c"
#include "mesh.c"

int main() {
	if (!glfwInit()) return -1;
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "MyWorld", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	mat4 to_screen = to_screen_mat4(WIDTH, HEIGHT);
	mat4 proj = projection_mat4(HEIGHT/WIDTH, deg2rad(FOV), FAR, NEAR);

	camera mc = ncamera(nvec4(0,0,0,1), nvec4(0,0,0,1), nvec4(0,0,-1,1), nvec4(0,1,0,1));

	mesh a = create_mesh_from_obj("test_teto.obj");
	a.position.z = 3.0f;
	//a.position.x = -1.0f;
	a.position.y = -2.5f;
	a.scale = nvec4(0.05f, 0.05f, 0.05f, 1.0f);
	
	double lstt = glfwGetTime();
	double max_fps = 0.0f;
	double all_fps = 0.0f;
	uint32_t frame_cnt = 0;
	while (!glfwWindowShouldClose(window)) {
        double curt = glfwGetTime();
		double delta = curt - lstt;
		lstt = curt;
		if (delta > 0.001f) {
			printf("DELTA: %lf, FPS: %lf, MAX_FPS: %lf AVG: %lf\n", delta, 1.0f / delta, max_fps, all_fps/frame_cnt); 
			if (1.0f/delta > max_fps) max_fps = (double)(1.0f/delta);
			frame_cnt++;
			all_fps += 1.0f/delta;
		}
		if (is_key_pressed(window, GLFW_KEY_W)) mc.position.z += delta * 3.0f;
		if (is_key_pressed(window, GLFW_KEY_S)) mc.position.z -= delta * 3.0f;
		if (is_key_pressed(window, GLFW_KEY_D)) mc.position.x += delta * 3.0f;
		if (is_key_pressed(window, GLFW_KEY_A)) mc.position.x -= delta * 3.0f;
		if (is_key_pressed(window, GLFW_KEY_LEFT_SHIFT)) mc.position.y += delta * 3.0f;
		if (is_key_pressed(window, GLFW_KEY_SPACE)) mc.position.y -= delta * 3.0f;
	/*	
		
		if (is_key_pressed(window, GLFW_KEY_LEFT)) a.rotation.z -= delta;
		if (is_key_pressed(window, GLFW_KEY_RIGHT)) a.rotation.z += delta;
		if (is_key_pressed(window, GLFW_KEY_DOWN)) a.rotation.y -= delta;
		if (is_key_pressed(window, GLFW_KEY_UP)) a.rotation.y += delta;
	*/	

		glClear(GL_COLOR_BUFFER_BIT);
		
		dynamic_uint8_t pix = malloc_uint8_t(WIDTH * HEIGHT * 4);
		dynamic_float depth = malloc_float(WIDTH * HEIGHT);
		fill_zeros_uint8_t(&pix);
		fill_zeros_float(&depth);
		// draw zone
		
		a.rotation.y += delta;
		draw(a, mc, proj, to_screen, pix, depth);


		//draw zone end
		glDrawPixels(WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, (void*)&pix.arr[0]);
		glfwSwapBuffers(window);
        glfwPollEvents();
		
		dealloc_uint8_t(&pix);
		dealloc_float(&depth);
	}
	
	free_mesh(&a);

	glfwDestroyWindow(window);
    glfwTerminate();
}
