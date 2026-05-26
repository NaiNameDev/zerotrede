vec4 ga, gb, gc;
vec4 gca, gcb, gcc;

void draw_edge_line(vec4 a, vec4 b, dynamic_vec4 mmx[], int minx, dynamic_uint8_t* pix, dynamic_float* depth, int w, int h) {
	int x1, x2, y1, y2;
	x1 = floor(a.x);
	x2 = floor(b.x);
	y1 = floor(a.y);
	y2 = floor(b.y);

	int dx = abs(x1 - x2);
	int dy = abs(y1 - y2);
	int sx = (x1 < x2 ? 1 : -1);
	int sy = (y1 < y2 ? 1 : -1);
	int er = dx - dy;

	while (1) {
		vec4 np = nvec4(x1, y1, 0.0f, 1.0f);
		vec4 bari = baricentric_coords(ga, gb, gc, np);
		put_vec4(&mmx[x1 - minx], np);
			
		float dp = bari_blend_float(-ga.w, -gb.w, -gc.w, bari);

		float curdp = get_depth_pixel(depth, x1, y1, w, h);
		if (curdp < NEAR) curdp = FAR;
		
		if (dp < curdp && dp > NEAR && dp < FAR) {
			vec4 col = bari_blend(gca, gcb, gcc, bari);
			
			set_pixel(pix, x1, y1, w, h, floor(col.x * 255), floor(col.y * 255), floor(col.z * 255));
			set_depth_pixel(depth, x1, y1, w, h, dp);
		}
		
		if (x1 == x2 && y1 == y2) return;
		
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
}

inline void draw_direct_line(dynamic_uint8_t* pix, dynamic_float* depth, dynamic_vec4* mmx, int w, int h, int minidx, int sminidx, int j, int len) {
	int curx = mmx->arr[minidx].x;
	int cury = mmx->arr[minidx].y + j + 1;
	vec4 bari = baricentric_coords(ga, gb, gc, nvec4(curx, cury, 0.0f, 1.0f));
		
	float dp = bari_blend_float(-ga.w, -gb.w, -gc.w, bari);

	float curdp = get_depth_pixel(depth, curx, cury, w, h);
	if (curdp < NEAR) curdp = FAR;
	
	if (dp < curdp && dp > NEAR && dp < FAR) {
		vec4 col = bari_blend(gca, gcb, gcc, bari);
		
		set_pixel(pix, curx, cury, w, h, floor(col.x * 255), floor(col.y * 255), floor(col.z * 255));
		set_depth_pixel(depth, curx, cury, w, h, dp);
	}
}

inline void fill_trg_inside(dynamic_uint8_t* pix, dynamic_float* depth, dynamic_vec4* mmx, int w, int h) {
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
			draw_direct_line(pix, depth, mmx, w, h, minidx, sminidx, j, len);
		}
	}
	else {
		int len = min - smin;
		if (len == 1) return;
		for (long j = 0; j < len - 1; j++) {
			draw_direct_line(pix, depth, mmx, w, h, sminidx, minidx, j, len);
		}
	}
}

void draw_trg(vec4 a, vec4 b, vec4 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth) {
	if (-a.w > FAR  && -b.w > FAR  && -c.w > FAR)  return;
	if (-a.w < NEAR && -b.w < NEAR && -c.w < NEAR) return;
	if (a.x < 0 && b.x < 0 && c.x < 0) return;
	if (a.x > WIDTH && b.x > WIDTH && c.x > WIDTH) return;
	if (a.y < 0 && b.y < 0 && c.y < 0) return;
	if (a.y > HEIGHT && b.y > HEIGHT && c.y > HEIGHT) return;

	int is_a_out = a.x > WIDTH || a.y > HEIGHT || a.x < 0 || a.y < 0 || -a.w > FAR || -a.w < NEAR;
	int is_b_out = b.x > WIDTH || b.y > HEIGHT || b.x < 0 || b.y < 0 || -b.w > FAR || -b.w < NEAR;
	int is_c_out = c.x > WIDTH || c.y > HEIGHT || c.x < 0 || c.y < 0 || -c.w > FAR || -c.w < NEAR;

	// pizdec
	if (!(!is_a_out && !is_b_out && !is_c_out)) {
		if (hard_edge_test(a, b, c, ca, cb, cc, pix, depth) == 0) return;
	}
	
	int minx = floor(min3(a.x, b.x, c.x));
	int maxx = floor(max3(a.x, b.x, c.x));
	size_t s = maxx - minx + 1;
	if (s > WIDTH + HEIGHT) return;

	dynamic_vec4 minmax_x[s];
	for (int i = 0; i < s; i++) {
		minmax_x[i] = malloc_vec4(0);
	}
	
	ga = a;gb = b;gc = c;
	gca = ca;gcb = cb;gcc = cc;
	draw_edge_line(a, b, minmax_x, minx, &pix, &depth, WIDTH, HEIGHT);
	draw_edge_line(b, c, minmax_x, minx, &pix, &depth, WIDTH, HEIGHT);
	draw_edge_line(c, a, minmax_x, minx, &pix, &depth, WIDTH, HEIGHT);
	
	for (size_t i = 0; i < s; i++) {
		bari_fill_trg_inside(&pix, &depth, &minmax_x[i], WIDTH, HEIGHT);
		dealloc_vec4(&minmax_x[i]);
	}
}
