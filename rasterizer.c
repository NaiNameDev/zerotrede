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

void draw_direct_line(dynamic_uint8_t* pix, dynamic_float* depth, dynamic_vec4* mmx, dynamic_vec4* mmx_color, dynamic_float* mmx_depth, int w, int h, int minidx, int sminidx, int j, int len) {
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

void fill_trg_inside(dynamic_uint8_t* pix, dynamic_float* depth, dynamic_vec4* mmx, dynamic_vec4* mmx_color, dynamic_float* mmx_depth, int w, int h) {
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

void draw_trg(vec4 a, vec4 b, vec4 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth);

float trg_s(vec4 a, vec4 b, vec4 c) {
	vec4 na = minus3(c, a);
	vec4 nb = minus3(b, a);
	na.z = 0.0f;
	nb.z = 0.0f;
	return 0.5 * (lenght3(cross3(na, nb)) / (lenght3(na) * lenght3(nb))) * lenght3(na) * lenght3(nb);
}

vec4 baricentric_coords(vec4 a, vec4 b, vec4 c, vec4 vec) {
	float big_s = trg_s(a, b, c);
	return nvec4(trg_s(vec, b, c) / big_s, trg_s(vec, a, c) / big_s, trg_s(vec, a, b) / big_s, 1.0f);
}
vec4 bari_blend(vec4 c1, vec4 c2, vec4 c3, vec4 bari) {
	return nvec4(c1.x * bari.x + c2.x * bari.y + c3.x * bari.z, 
				 c1.y * bari.x + c2.y * bari.y + c3.y * bari.z,
				 c1.z * bari.x + c2.z * bari.y + c3.z * bari.z, 1.0f);
}

void slice_x_out(vec4 a, vec4 b, vec4 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth, int x) {
	vec4 tmp_1 = nvec4(x, c.y + ((x - c.x) * (b.y - c.y) / (b.x - c.x)), 0.0f, 1.0f);
	vec4 tmp_2 = nvec4(x, a.y + ((x - a.x) * (b.y - a.y) / (b.x - a.x)), 0.0f, 1.0f);
	vec4 b1 = baricentric_coords(a, b, c, tmp_1);
	vec4 b2 = baricentric_coords(a, b, c, tmp_2);
	tmp_1.w = a.w * b1.x + b.w * b1.y + c.w * b1.z;
	tmp_2.w = a.w * b2.x + b.w * b2.y + c.w * b2.z;
	//draw_trg(a, tmp_1, c, ca, bari_blend(ca, cb, cc, b1), cc, pix, depth);
	//draw_trg(a, tmp_2, tmp_1, ca, bari_blend(ca, cb, cc, b2), bari_blend(ca, cb, cc, b1), pix, depth);
	draw_trg(a, tmp_1, c, ca, cb, cc, pix, depth);
	draw_trg(a, tmp_2, tmp_1, ca, cb, cc, pix, depth);
}
void slice_y_out(vec4 a, vec4 b, vec4 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth, int y) {
	vec4 tmp_1 = nvec4(c.x + ((b.x - c.x) / (b.y - c.y))*(y - c.y), y, 0.0f, b.w);
	vec4 tmp_2 = nvec4(a.x + ((b.x - a.x) / (b.y - a.y))*(y - a.y), y, 0.0f, b.w);
	vec4 b1 = baricentric_coords(a, b, c, tmp_1);
	vec4 b2 = baricentric_coords(a, b, c, tmp_2);
	tmp_1.w = a.w * b1.x + b.w * b1.y + c.w * b1.z;
	tmp_2.w = a.w * b2.x + b.w * b2.y + c.w * b2.z;
	//draw_trg(a, tmp_1, c, ca, bari_blend(ca, cb, cc, b1), cc, pix, depth);
	//draw_trg(a, tmp_2, tmp_1, ca, bari_blend(ca, cb, cc, b2), bari_blend(ca, cb, cc, b1), pix, depth);
	draw_trg(a, tmp_1, c, ca, cb, cc, pix, depth);
	draw_trg(a, tmp_2, tmp_1, ca, cb, cc, pix, depth);
}
void slice_w_out(vec4 a, vec4 b, vec4 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth, int w) {
	vec4 tmp_1 = nvec4(c.y + ((w - c.w) * (b.y - c.y) / (b.w - c.w)), c.x + ((b.x - c.x) / (b.w - c.w))*(w - c.w), 0.0f, b.w);
	vec4 tmp_2 = nvec4(a.y + ((w - a.w) * (b.y - a.y) / (b.w - a.w)), a.x + ((b.x - a.x) / (b.w - a.w))*(w - a.w), 0.0f, b.w);
	vec4 b1 = baricentric_coords(a, b, c, tmp_1);
	vec4 b2 = baricentric_coords(a, b, c, tmp_2);
	tmp_1.w = a.w * b1.x + b.w * b1.y + c.w * b1.z;
	tmp_2.w = a.w * b2.x + b.w * b2.y + c.w * b2.z;
	//draw_trg(a, tmp_1, c, ca, bari_blend(ca, cb, cc, b1), cc, pix, depth);
	//draw_trg(a, tmp_2, tmp_1, ca, bari_blend(ca, cb, cc, b2), bari_blend(ca, cb, cc, b1), pix, depth);
	draw_trg(a, tmp_1, c, ca, cb, cc, pix, depth);
	draw_trg(a, tmp_2, tmp_1, ca, cb, cc, pix, depth);
}

void draw_trg(vec4 a, vec4 b, vec4 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth) {
	int is_a_out = a.x > WIDTH || a.y > HEIGHT || a.x < 0 || a.y < 0 || -a.w > FAR || -a.w < NEAR;
	int is_b_out = b.x > WIDTH || b.y > HEIGHT || b.x < 0 || b.y < 0 || -b.w > FAR || -b.w < NEAR;
	int is_c_out = c.x > WIDTH || c.y > HEIGHT || c.x < 0 || c.y < 0 || -c.w > FAR || -c.w < NEAR;

	if (is_a_out && is_b_out && is_c_out) return;
	if (is_a_out || is_b_out || is_c_out) {
		if(is_a_out && (!is_b_out && !is_c_out)) {
			if 		(a.x < 0.0) {slice_x_out(c, a, b, cc, ca, cb, pix, depth, 0); return;}
			else if (a.x > HEIGHT) {slice_x_out(c, a, b, cc, ca, cb, pix, depth, HEIGHT); return;}
			else if (a.y < 0.0) {slice_y_out(c, a, b, cc, ca, cb, pix, depth, 0); return;}
			else if (a.y > WIDTH) {slice_y_out(c, a, b, cc, ca, cb, pix, depth, WIDTH); return;}
			//else if (a.w < NEAR) {slice_w_out(c, a, b, cc, ca, cb, pix, depth, NEAR); return;}
			//else if (a.w > FAR) {slice_w_out(c, a, b, cc, ca, cb, pix, depth, FAR); return;}
		}
		if(is_c_out && (!is_b_out && !is_a_out)) {
			if 		(c.x < 0.0) {slice_x_out(b, c, a, cb, cc, ca, pix, depth, 0); return;}
			else if (c.x > HEIGHT) {slice_x_out(b, c, a, cb, cc, ca, pix, depth, HEIGHT); return;}
			else if (c.y < 0.0) {slice_y_out(b, c, a, cb, cc, ca, pix, depth, 0); return;}
			else if (c.y > WIDTH) {slice_y_out(b, c, a, cb, cc, ca, pix, depth, WIDTH); return;}
			//else if (c.w < NEAR) {slice_w_out(b, c, a, cb, cc, ca, pix, depth, NEAR); return;}
			//else if (c.w > FAR) {slice_w_out(b, c, a, cb, cc, ca, pix, depth, FAR); return;}
		}
		if(is_b_out && (!is_c_out && !is_a_out)) {
			if 		(b.x < 0.0) {slice_x_out(a, b, c, ca, cb, cc, pix, depth, 0); return;}
			else if (b.x > HEIGHT) {slice_x_out(a, b, c, ca, cb, cc, pix, depth, HEIGHT); return;}
			else if (b.y < 0.0) {slice_y_out(a, b, c, ca, cb, cc, pix, depth, 0); return;}
			else if (b.y > WIDTH) {slice_y_out(a, b, c, ca, cb, cc, pix, depth, WIDTH); return;}
			//else if (b.y < NEAR) {slice_w_out(a, b, c, ca, cb, cc, pix, depth, NEAR); return;}
			//else if (b.y > FAR) {slice_w_out(a, b, c, ca, cb, cc, pix, depth, FAR); return;}
		}
	}
	
	int minx = floor(min3(a.x, b.x, c.x));
	int maxx = floor(max3(a.x, b.x, c.x));
	size_t s = maxx - minx + 1;
	if (s > WIDTH + HEIGHT) return;

	dynamic_vec4 minmax_x[s];
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
		fill_trg_inside(&pix, &depth, &minmax_x[i], &minmax_x_color[i], &minmax_x_depth[i], WIDTH, HEIGHT);
		
		dealloc_vec4(&minmax_x[i]);
		dealloc_vec4(&minmax_x_color[i]);
		dealloc_float(&minmax_x_depth[i]);
	}
	dealloc_vec4(&lab);
	dealloc_vec4(&lbc);
	dealloc_vec4(&lca);
}
