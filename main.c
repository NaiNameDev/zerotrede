#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include <GLFW/glfw3.h>
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb/stb_image.h>

#define WIDTH 1280.0f
#define HEIGHT 720.0f
#define NEAR 0.5f
#define FAR 100.0f
#define FOV 60.0f

int is_key_pressed(GLFWwindow* win, int key) {
	return glfwGetKey(win, key) == GLFW_PRESS;
}

#include "dynamic.c"

#include "mymath.c"
#include "camera.c"
DEFINE_DYNAMYC_TYPE(vec4)
DEFINE_DYNAMYC_TYPE(vec3)
DEFINE_DYNAMYC_TYPE(vec2)
DEFINE_DYNAMYC_TYPE(char)
DEFINE_DYNAMYC_TYPE(float)
DEFINE_DYNAMYC_TYPE(uint8_t);
#include "rasterizer.c"
#include "mesh.c"
//#include "image_loader.c"

int cursor_x = WIDTH/2;
int cursor_y = HEIGHT/2;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	cursor_x = xpos;
	cursor_y = ypos;
}

int main() {
	if (!glfwInit()) return -1;
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "MyWorld", NULL, NULL);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//stbi_set_flip_vertically_on_load(true);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	mat4 to_screen = to_screen_mat4(WIDTH, HEIGHT);
	mat4 proj = projection_mat4(HEIGHT/WIDTH, deg2rad(FOV), FAR, NEAR);

	int lx = WIDTH/2;
	int ly = HEIGHT/2;
	camera_t mc = ncamera(nvec3(0,0,0), nvec3(0,0,0), nvec3(0,0,-1), nvec3(0,1,0));

	mesh_t cube = create_mesh_from_obj("test_trg.obj");
	cube.position.z = 3.0f;
	cube.rotation.y = deg2rad(-90.0f);
	mesh_t teto = create_mesh_from_obj("test_teto.obj");
	teto.rotation.y = deg2rad(180.0f);
	teto.position.z = 3.0f;
	teto.position.y = -2.5f;
	teto.scale = nvec3(0.05f, 0.05f, 0.05f);
	
	//image_t test_img = load_image("./test_img.png");

	double lstt = glfwGetTime();
	double max_fps = 0.0f;
	double all_fps = 0.0f;
	uint32_t frame_cnt = 0;
	while (!glfwWindowShouldClose(window)) {
		double curt = glfwGetTime();
		double delta = curt - lstt;
		lstt = curt;
		if (delta > 0.001f) {
			printf("\nDELTA: %lf, FPS: %lf, MAX_FPS: %lf AVG: %lf\n", delta, 1.0f / delta, max_fps, all_fps/frame_cnt); 
			if (1.0f/delta > max_fps) max_fps = (double)(1.0f/delta);
			frame_cnt++;
			all_fps += 1.0f/delta;
		}
		mc.rotation.x += (ly - cursor_y) * 0.001f;
		mc.rotation.y -= (lx - cursor_x) * 0.001f;
		lx = cursor_x;
		ly = cursor_y;
		mat4 r = mulmat4(rotation_z_mat4(-mc.rotation.z), mulmat4(rotation_y_mat4(-mc.rotation.y), rotation_x_mat4(-mc.rotation.x)));
		if (is_key_pressed(window, GLFW_KEY_W)) mc.position = plus3(scal_mul_vec3(mulmat4vec3(r, mc.forward), delta * -3.0f), mc.position);
		if (is_key_pressed(window, GLFW_KEY_S)) mc.position = plus3(scal_mul_vec3(mulmat4vec3(r, mc.forward), delta * 3.0f), mc.position);
		if (is_key_pressed(window, GLFW_KEY_D)) mc.position = plus3(scal_mul_vec3(cross3(mc.up, normalize3(mulmat4vec3(r, mc.forward))), delta * -3.0f), mc.position);
		if (is_key_pressed(window, GLFW_KEY_A)) mc.position = plus3(scal_mul_vec3(cross3(mc.up, normalize3(mulmat4vec3(r, mc.forward))), delta * 3.0f), mc.position);
		if (is_key_pressed(window, GLFW_KEY_LEFT_SHIFT)) mc.position.y += delta * 3.0f;
		if (is_key_pressed(window, GLFW_KEY_SPACE)) mc.position.y -= delta * 3.0f;
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		dynamic_uint8_t pix = malloc_uint8_t(WIDTH * HEIGHT * 4);
		dynamic_float depth = malloc_float(WIDTH * HEIGHT);
		fill_zeros_uint8_t(&pix);
		fill_zeros_float(&depth);
		// draw zone
		

		//teto.rotation.y += delta;
		draw(teto, mc, proj, to_screen, pix, depth);
		//draw(cube, mc, proj, to_screen, pix, depth);


		//draw zone end
		glDrawPixels(WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, (void*)&pix.arr[0]);
		glfwSwapBuffers(window);
        glfwPollEvents();
		
		dealloc_uint8_t(&pix);
		dealloc_float(&depth);
	}
	
	free_mesh(&cube);
	free_mesh(&teto);

	glfwDestroyWindow(window);
    glfwTerminate();
}
