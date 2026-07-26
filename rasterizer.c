inline void set_pixel(dynamic_uint8_t* pix, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
	if (x >= 0 && y >= 0 && x < w && y < h) {
		pix->arr[4 * (y * w + x)] = r;
		pix->arr[4 * (y * w + x) + 1] = g;
		pix->arr[4 * (y * w + x) + 2] = b;
		pix->arr[4 * (y * w + x) + 3] = 255;
	}
}
inline void set_depth_pixel(dynamic_float* depth, int x, int y, int w, int h, float c) {
	if (x >= 0 && y >= 0 && x < w && y < h) {
		depth->arr[(y * w + x)] = c;
	}
}
inline float get_depth_pixel(dynamic_float* depth, int x, int y, int w, int h) {
	if (x >= 0 && y >= 0 && x < w && y < h) {
		return depth->arr[(y * w + x)];
	}
	return 0.0f;
}

inline dynamic_vec3 get_line(vec3 p1, vec3 p2) {
	int x1, x2, y1, y2;
	x1 = floor(p1.x);
	x2 = floor(p2.x);
	y1 = floor(p1.y);
	y2 = floor(p2.y);

	dynamic_vec3 ret = malloc_vec3(0);
	
	int dx = abs(x1 - x2);
	int dy = abs(y1 - y2);
	int sx = (x1 < x2 ? 1 : -1);
	int sy = (y1 < y2 ? 1 : -1);
	int er = dx - dy;

	while (1) {
		put_vec3(&ret, nvec3(x1, y1, 0.0f));
		
		if (x1 == x2 && y1 == y2) return ret;
		
		int e2 = 2 * er;
		if (e2 > -dy) {
			er -= dy;
			x1 += sx;
		}
		if (e2 < dx) {
			er += dx;
			y1 += sy;
		}
	}
	return ret;
}

inline float trg_s(vec3 a, vec3 b, vec3 c) {
	vec3 na = minus3(c, a);
	vec3 nb = minus3(b, a);
	na.z = 0.0f;
	nb.z = 0.0f;
	return 0.5 * (lenght3(cross3(na, nb)) / (lenght3(na) * lenght3(nb))) * lenght3(na) * lenght3(nb);
}
inline vec3 baricentric_coords(vec3 a, vec3 b, vec3 c, vec3 vec) {
	float big_s = trg_s(a, b, c);
	return nvec3(trg_s(vec, b, c) / big_s, trg_s(vec, a, c) / big_s, trg_s(vec, a, b) / big_s);
}
inline vec4 bari_blend(vec4 c1, vec4 c2, vec4 c3, vec3 bari) {
	return nvec4(c1.x * bari.x + c2.x * bari.y + c3.x * bari.z, 
				 c1.y * bari.x + c2.y * bari.y + c3.y * bari.z,
				 c1.z * bari.x + c2.z * bari.y + c3.z * bari.z, 1.0f);
}
inline float bari_blend_float(float a, float b, float c, vec3 bari) {
	return a * bari.x + b * bari.y + c * bari.z;
}

inline void draw_direct_line(dynamic_uint8_t* pix, dynamic_float* depth, dynamic_vec3* mmx, dynamic_vec4* mmx_color, dynamic_float* mmx_depth, int w, int h, int minidx, int sminidx, int j, int len) {
	int curx = mmx->arr[minidx].x;
	float dp = lerp(mmx_depth->arr[minidx], mmx_depth->arr[sminidx], (float)j / (float)len);
	
	float curdp = get_depth_pixel(depth, curx, mmx->arr[minidx].y + j + 1, w, h);
	if (curdp < NEAR) curdp = FAR;
	
	if (dp < curdp && dp > NEAR && dp < FAR) {
		vec4 c = lerpv4(mmx_color->arr[minidx], mmx_color->arr[sminidx], (float)j / (float)len);
		
		set_pixel(pix, curx, mmx->arr[minidx].y + j + 1, w, h, floor(c.x * 255), floor(c.y * 255), floor(c.z * 255));
		set_depth_pixel(depth, curx, mmx->arr[minidx].y + j + 1, w, h, dp);
	}
}

inline void fill_trg_inside(dynamic_uint8_t* pix, dynamic_float* depth, dynamic_vec3* mmx, dynamic_vec4* mmx_color, dynamic_float* mmx_depth, int w, int h) {
	int minidx = 0;
	int sminidx = 0;
	int min = INT_MAX;
	int smin = INT_MAX;
	for (size_t j = 0; j < mmx->size - 1; j++) {
		if (fabsf(mmx->arr[j].y - mmx->arr[j + 1].y) > 1) {
			smin = mmx->arr[j].y;
			min = mmx->arr[j + 1].y;
			minidx = j+1;
			sminidx = j;
			break;
		}
	}
	if (min == INT_MAX || smin == INT_MAX) return;
	if (smin > min) {
		int len = smin - min;
		if (len == 1) return;
		for (long j = 0; j < len - 1; j++) {
			draw_direct_line(pix, depth, mmx, mmx_color, mmx_depth, w, h, minidx, sminidx, j, len);
		}
	}
	else {
		int len = min - smin;
		if (len == 1) return;
		for (long j = 0; j < len - 1; j++) {
			draw_direct_line(pix, depth, mmx, mmx_color, mmx_depth, w, h, sminidx, minidx, j, len);
		}
	}
}

inline void draw_edge_line(dynamic_vec3 line, dynamic_vec3* mmx, dynamic_vec4* mmx_color, dynamic_float* mmx_depth, vec4 c1, vec4 c2, float d1, float d2, int lerp_prg, dynamic_uint8_t* pix, dynamic_float* depth, int w, int h) {
	put_vec3(mmx, line.arr[lerp_prg]);
		
	vec4 c = lerpv4(c1, c2, (float)lerp_prg / (float) line.size);
	put_vec4(mmx_color, c);
	
	float dp = lerp(d1 * -1.0f, d2 * -1.0f, (float)lerp_prg / (float) line.size);
	put_float(mmx_depth, dp);

	float curdp = get_depth_pixel(depth, floor(line.arr[lerp_prg].x), floor(line.arr[lerp_prg].y), w, h);
	if (curdp < NEAR) curdp = FAR;
	
	if (dp < curdp && dp > NEAR && dp < FAR) {
		set_pixel(pix, floor(line.arr[lerp_prg].x), floor(line.arr[lerp_prg].y), w, h, floor(c.x * 255), floor(c.y * 255), floor(c.z * 255));
		set_depth_pixel(depth, floor(line.arr[lerp_prg].x), floor(line.arr[lerp_prg].y), w, h, dp);
	}
}

#include "triangle_slicer.c"

// LERP BACKEND
#if BARI_RASTERIZER == 0
inline void draw_trg(vec3 a, vec3 b, vec3 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth) {
	if (-a.z > FAR  && -b.z > FAR  && -c.z > FAR)  return;
	if (-a.z < NEAR && -b.z < NEAR && -c.z < NEAR) return;
	if (a.x < 0 && b.x < 0 && c.x < 0) return;
	if (a.x > WIDTH && b.x > WIDTH && c.x > WIDTH) return;
	if (a.y < 0 && b.y < 0 && c.y < 0) return;
	if (a.y > HEIGHT && b.y > HEIGHT && c.y > HEIGHT) return;

	int is_a_out = a.x > WIDTH || a.y > HEIGHT || a.x < 0 || a.y < 0 || -a.z > FAR || -a.z < NEAR;
	int is_b_out = b.x > WIDTH || b.y > HEIGHT || b.x < 0 || b.y < 0 || -b.z > FAR || -b.z < NEAR;
	int is_c_out = c.x > WIDTH || c.y > HEIGHT || c.x < 0 || c.y < 0 || -c.z > FAR || -c.z < NEAR;

	if (!(!is_a_out && !is_b_out && !is_c_out)) {
		if (hard_edge_test(a, b, c, ca, cb, cc, pix, depth) == 0) return;
	}
	int minx = floor(min3(a.x, b.x, c.x));
	int maxx = floor(max3(a.x, b.x, c.x));
	size_t s = maxx - minx + 1;

	dynamic_vec3 minmax_x[s];
	dynamic_vec4 minmax_x_color[s];
	dynamic_float minmax_x_depth[s];
	for (size_t i = 0; i < s; i++) {
		minmax_x[i] = malloc_vec3(0);
		minmax_x_color[i] = malloc_vec4(0);
		minmax_x_depth[i] = malloc_float(0);
	}
	dynamic_vec3 lab = get_line(a,b);
	dynamic_vec3 lbc = get_line(b,c);
	dynamic_vec3 lca = get_line(c,a);

	for (size_t i = 0; i < lab.size; i++) {
		int idx = (int)lab.arr[i].x - minx;
		draw_edge_line(lab, &minmax_x[idx], &minmax_x_color[idx], &minmax_x_depth[idx], ca, cb, a.z, b.z, i, &pix, &depth, WIDTH, HEIGHT);
	}
	for (size_t i = 0; i < lbc.size; i++) {
		int idx = (int)lbc.arr[i].x - minx;
		draw_edge_line(lbc, &minmax_x[idx], &minmax_x_color[idx], &minmax_x_depth[idx], cb, cc, b.z, c.z, i, &pix, &depth, WIDTH, HEIGHT);
	}
	for (size_t i = 0; i < lca.size; i++) {
		int idx = (int)lca.arr[i].x - minx;
		draw_edge_line(lca, &minmax_x[idx], &minmax_x_color[idx], &minmax_x_depth[idx], cc, ca, c.z, a.z, i, &pix, &depth, WIDTH, HEIGHT);
	}

	for (size_t i = 0; i < s; i++) {
		fill_trg_inside(&pix, &depth, &minmax_x[i], &minmax_x_color[i], &minmax_x_depth[i], WIDTH, HEIGHT);
		
		dealloc_vec3(&minmax_x[i]);
		dealloc_vec4(&minmax_x_color[i]);
		dealloc_float(&minmax_x_depth[i]);
	}
	dealloc_vec3(&lab);
	dealloc_vec3(&lbc);
	dealloc_vec3(&lca);
}

// BARI BACKEND
#else
#include "bari.c"
#endif
