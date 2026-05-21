void set_pixel(dynamic_uint8_t* pix, unsigned int x, unsigned int y, int w, int h, int r, int g, int b) {
	if (x >= 0 && y >= 0 && x < w && y < h) {
		pix->arr[4 * (y * w + x)] = r;
		pix->arr[4 * (y * w + x) + 1] = g;
		pix->arr[4 * (y * w + x) + 2] = b;
		pix->arr[4 * (y * w + x) + 3] = 255;
	}
}
void set_depth_pixel(dynamic_float* depth, unsigned int x, unsigned int y, int w, int h, float c) {
	if (x >= 0 && y >= 0 && x < w && y < h) {
		depth->arr[(y * w + x)] = c;
	}
}
float get_depth_pixel(dynamic_float* depth, unsigned int x, unsigned int y, int w, int h) {
	if (x >= 0 && y >= 0 && x < w && y < h) {
		return depth->arr[(y * w + x)];
	}
	return 0.0f;
}

dynamic_vec4 get_line(vec4 p1, vec4 p2) {
	int x1, x2, y1, y2;
	x1 = floor(p1.x);
	x2 = floor(p2.x);
	y1 = floor(p1.y);
	y2 = floor(p2.y);

	dynamic_vec4 ret = malloc_vec4(0);
	int s = 0;
	
	int dx = abs(x1 - x2);
	int dy = abs(y1 - y2);
	int sx = (x1 < x2 ? 1 : -1);
	int sy = (y1 < y2 ? 1 : -1);
	int er = dx - dy;

	while (1) {
		put_vec4(&ret, nvec4(x1, y1, 0.0f, 0.0f));
		
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

void draw_trg_line_fill(dynamic_uint8_t* pix, dynamic_float* depth, dynamic_vec4* mmx, dynamic_vec4* mmx_color, dynamic_float* mmx_depth, int w, int h, int minidx, int sminidx, int j, int len) {
	int curx = mmx->arr[minidx].x;
	float dp = lerp(mmx_depth->arr[minidx], mmx_depth->arr[sminidx], (float)j / (float)len);
	
	float curdp = get_depth_pixel(depth, curx, mmx->arr[minidx].y + j + 1, w, h);
	if (curdp < NEAR) curdp = FAR;
	
	if (dp < curdp && dp > NEAR && dp < FAR) {
		vec4 c = lerpv(mmx_color->arr[minidx], mmx_color->arr[sminidx], (float)j / (float)len);
		
		set_pixel(pix, curx, mmx->arr[minidx].y + j + 1, w, h, floor(c.x * 255), floor(c.y * 255), floor(c.z * 255));
		set_depth_pixel(depth, curx, mmx->arr[minidx].y + j + 1, w, h, dp);
	}
}

void draw_trg_line(dynamic_uint8_t* pix, dynamic_float* depth, dynamic_vec4* mmx, dynamic_vec4* mmx_color, dynamic_float* mmx_depth, int w, int h) {
	int minidx = 0;
	int sminidx = 0;
	int min = INT_MAX;
	int smin = INT_MAX;
	for (size_t j = 0; j < mmx->size - 1; j++) {
		if (abs(mmx->arr[j].y - mmx->arr[j + 1].y) > 1) {
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
			draw_trg_line_fill(pix, depth, mmx, mmx_color, mmx_depth, w, h, minidx, sminidx, j, len);
		}
	}
	else {
		int len = min - smin;
		if (len == 1) return;
		for (long j = 0; j < len - 1; j++) {
			draw_trg_line_fill(pix, depth, mmx, mmx_color, mmx_depth, w, h, sminidx, minidx, j, len);
		}
	}
}

void draw_edge_line(dynamic_vec4 line, dynamic_vec4* mmx, dynamic_vec4* mmx_color, dynamic_float* mmx_depth, vec4 c1, vec4 c2, float d1, float d2, int lerp_prg, dynamic_uint8_t* pix, dynamic_float* depth, int w, int h) {
	put_vec4(mmx, line.arr[lerp_prg]);
		
	vec4 c = lerpv(c1, c2, (float)lerp_prg / (float) line.size);
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

void draw_trg(vec4 a, vec4 b, vec4 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth) {
	int minx = floor(min3(a.x, b.x, c.x));
	int maxx = floor(max3(a.x, b.x, c.x));
	size_t s = maxx - minx + 1;
	if (s > WIDTH + HEIGHT) return;

	dynamic_vec4 minmax_x[s]; //static array of dynamic array of vec4
	dynamic_vec4 minmax_x_color[s];
	dynamic_float minmax_x_depth[s];
	for (int i = 0; i < s; i++) {
		minmax_x[i] = malloc_vec4(0);
		minmax_x_color[i] = malloc_vec4(0);
		minmax_x_depth[i] = malloc_float(0);
	}
	dynamic_vec4 lab = get_line(a,b);
	dynamic_vec4 lbc = get_line(b,c);
	dynamic_vec4 lca = get_line(c,a);
	
	for (size_t i = 0; i < lab.size; i++) {
		int idx = (int)lab.arr[i].x - minx;
		draw_edge_line(lab, &minmax_x[idx], &minmax_x_color[idx], &minmax_x_depth[idx], ca, cb, a.w, b.w, i, &pix, &depth, WIDTH, HEIGHT);
	}
	for (size_t i = 0; i < lbc.size; i++) {
		int idx = (int)lbc.arr[i].x - minx;
		draw_edge_line(lbc, &minmax_x[idx], &minmax_x_color[idx], &minmax_x_depth[idx], cb, cc, b.w, c.w, i, &pix, &depth, WIDTH, HEIGHT);
	}
	for (size_t i = 0; i < lca.size; i++) {
		int idx = (int)lca.arr[i].x - minx;
		draw_edge_line(lca, &minmax_x[idx], &minmax_x_color[idx], &minmax_x_depth[idx], cc, ca, c.w, a.w, i, &pix, &depth, WIDTH, HEIGHT);
	}
	
	for (size_t i = 0; i < s; i++) {
		draw_trg_line(&pix, &depth, &minmax_x[i], &minmax_x_color[i], &minmax_x_depth[i], WIDTH, HEIGHT);
		
		dealloc_vec4(&minmax_x[i]);
		dealloc_vec4(&minmax_x_color[i]);
		dealloc_float(&minmax_x_depth[i]);
	}
	dealloc_vec4(&lab);
	dealloc_vec4(&lbc);
	dealloc_vec4(&lca);
}
