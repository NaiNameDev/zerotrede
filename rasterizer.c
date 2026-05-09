void set_pixel(dynamic_uint8_t* pix, unsigned int x, unsigned int y, int w, int h, int r, int g, int b) {
	if (x >= 0 && y >= 0 && x < w && y < h) {
		pix->arr[4 * (y * w + x)] = r;
		pix->arr[4 * (y * w + x) + 1] = g;
		pix->arr[4 * (y * w + x) + 2] = b;
		pix->arr[4 * (y * w + x) + 3] = 255;
	}
}

// DEALLOC IT AFTER USE!!!!!!!
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

struct tc_args {
	dynamic_uint8_t* pix;
	dynamic_vec4* mmx;
	dynamic_vec4* mmx_color;
	int w;
	int h;
};
void* thread_coloring(void* arg) {
	struct tc_args a = *(struct tc_args*)arg;

	int minidx = 0;
	int sminidx = 0;
	int min = INT_MAX;
	int smin = INT_MAX;
	for (size_t j = 0; j < a.mmx->size - 1; j++) {
		if (abs(a.mmx->arr[j].y - a.mmx->arr[j + 1].y) > 1) {
			smin = a.mmx->arr[j].y;
			min = a.mmx->arr[j + 1].y;
			minidx = j+1;
			sminidx = j;
			break;
		}
	}
	if (min == INT_MAX || smin == INT_MAX) return NULL;
	if (smin > min) {
		int len = smin - min;
		if (len == 1) return NULL;
		for (long j = 0; j < len; j++) {
			vec4 c = lerpv(a.mmx_color->arr[minidx], a.mmx_color->arr[sminidx], (float)j / (float)len);
			set_pixel(a.pix, a.mmx->arr[minidx].x, a.mmx->arr[minidx].y + j, a.w, a.h, floor(c.x * 255), floor(c.y * 255), floor(c.z * 255));
		}
	}
	else {
		int len = min - smin;
		if (len == 1) return NULL;
		for (long j = 0; j < len; j++) {
			vec4 c = lerpv(a.mmx_color->arr[sminidx], a.mmx_color->arr[minidx], (float)j / (float)len);
			set_pixel(a.pix, a.mmx->arr[sminidx].x, a.mmx->arr[sminidx].y + j, a.w, a.h, floor(c.x * 255), floor(c.y * 255), floor(c.z * 255));
		}
	}
	free(arg);
}

void draw_trg(vec4 a, vec4 b, vec4 c, vec4 ca, vec4 cb, vec4 cc, SDL_Surface* sr, dynamic_uint8_t pix) {
	int minx = floor(min3(a.x, b.x, c.x));
	int maxx = floor(max3(a.x, b.x, c.x));
	size_t s = maxx - minx + 1;

	dynamic_vec4 minmax_x[s]; //static array of dynamic array of vec4
	dynamic_vec4 minmax_x_color[s];
	for (int i = 0; i < s; i++) {
		minmax_x[i] = malloc_vec4(0);
		minmax_x_color[i] = malloc_vec4(0);
	}
	dynamic_vec4 lab = get_line(a,b);
	dynamic_vec4 lbc = get_line(b,c);
	dynamic_vec4 lca = get_line(c,a);
	
	for (size_t i = 0; i < lab.size; i++) {
		int idx = (int)lab.arr[i].x;
		put_vec4(&minmax_x[idx - minx], lab.arr[i]);

		vec4 c = lerpv(ca, cb, (float)i / (float)lab.size);
		put_vec4(&minmax_x_color[idx - minx], c);

		set_pixel(&pix, floor(lab.arr[i].x), floor(lab.arr[i].y), sr->w, sr->h, floor(c.x * 255), floor(c.y * 255), floor(c.z * 255));
	}
	for (size_t i = 0; i < lbc.size; i++) {
		int idx = (int)lbc.arr[i].x;
		put_vec4(&minmax_x[idx - minx], lbc.arr[i]);
		
		vec4 c = lerpv(cb, cc, (float)i / (float)lbc.size);
		put_vec4(&minmax_x_color[idx - minx], c);

		set_pixel(&pix, floor(lbc.arr[i].x), floor(lbc.arr[i].y), sr->w, sr->h, floor(c.x * 255), floor(c.y * 255), floor(c.z * 255));
	}
	for (size_t i = 0; i < lca.size; i++) {
		int idx = (int)lca.arr[i].x;
		put_vec4(&minmax_x[idx - minx], lca.arr[i]);
		
		vec4 c = lerpv(cc, ca, (float)i / (float)lca.size);
		put_vec4(&minmax_x_color[idx - minx], c);
		
		set_pixel(&pix, floor(lca.arr[i].x), floor(lca.arr[i].y), sr->w, sr->h, floor(c.x * 255), floor(c.y * 255), floor(c.z * 255));
	}

	for (size_t i = 0; i < s; i++) {
		struct tc_args* a = malloc(sizeof(struct tc_args)); *a = (struct tc_args){&pix, &minmax_x[i], &minmax_x_color[i], sr->w, sr->h};
		thread_coloring((void*)a);
	}

	dealloc_vec4(&lab);
	dealloc_vec4(&lbc);
	dealloc_vec4(&lca);
	for (int i = 0; i < s; i++) {
		dealloc_vec4(&minmax_x[i]);
		dealloc_vec4(&minmax_x_color[i]);
	}
}
