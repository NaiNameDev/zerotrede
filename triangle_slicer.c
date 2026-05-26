void draw_trg(vec3 a, vec3 b, vec3 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth);

inline void slice_x_out_single(vec3 a, vec3 b, vec3 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth, int x) {
	vec3 tmp_1 = nvec3(x, c.y + ((x - c.x) * (b.y - c.y) / (b.x - c.x)), 0.0f);
	vec3 tmp_2 = nvec3(x, a.y + ((x - a.x) * (b.y - a.y) / (b.x - a.x)), 0.0f);
	vec3 b1 = baricentric_coords(a, b, c, tmp_1);
	vec3 b2 = baricentric_coords(a, b, c, tmp_2);
	tmp_1.z = bari_blend_float(a.z, b.z, c.z, b1);
	tmp_2.z = bari_blend_float(a.z, b.z, c.z, b2);
	//tmp_1.z = a.z * b1.x + b.z * b1.y + c.z * b1.z;
	//tmp_2.z = a.z * b2.x + b.z * b2.y + c.z * b2.z;
#if DBG_CULLING_MODE == 0
	draw_trg(b, tmp_1, tmp_2, cb, bari_blend(ca, cb, cc, b1), bari_blend(ca, cb, cc, b2), pix, depth);
#else
	draw_trg(b, tmp_1, tmp_2, cb, cc, ca, pix, depth);
#endif
}
inline void slice_y_out_single(vec3 a, vec3 b, vec3 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth, int y) {
	vec3 tmp_1 = nvec3(c.x + ((b.x - c.x) / (b.y - c.y))*(y - c.y), y, b.z);
	vec3 tmp_2 = nvec3(a.x + ((b.x - a.x) / (b.y - a.y))*(y - a.y), y, b.z);
	vec3 b1 = baricentric_coords(a, b, c, tmp_1);
	vec3 b2 = baricentric_coords(a, b, c, tmp_2);
	tmp_1.z = bari_blend_float(a.z, b.z, c.z, b1);
	tmp_2.z = bari_blend_float(a.z, b.z, c.z, b2);
#if DBG_CULLING_MODE == 0
	draw_trg(b, tmp_1, tmp_2, cb, bari_blend(ca, cb, cc, b1), bari_blend(ca, cb, cc, b2), pix, depth);
#else
	draw_trg(b, tmp_1, tmp_2, cb, cc, ca, pix, depth);
#endif
}
inline void slice_x_out(vec3 a, vec3 b, vec3 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth, int x) {
	vec3 tmp_1 = nvec3(x, c.y + ((x - c.x) * (b.y - c.y) / (b.x - c.x)), 1.0f);
	vec3 tmp_2 = nvec3(x, a.y + ((x - a.x) * (b.y - a.y) / (b.x - a.x)), 1.0f);
	vec3 b1 = baricentric_coords(a, b, c, tmp_1);
	vec3 b2 = baricentric_coords(a, b, c, tmp_2);
	tmp_1.z = bari_blend_float(a.z, b.z, c.z, b1);
	tmp_2.z = bari_blend_float(a.z, b.z, c.z, b2);
#if DBG_CULLING_MODE == 0
	draw_trg(a, tmp_1, c, ca, bari_blend(ca, cb, cc, b1), cc, pix, depth);
	draw_trg(a, tmp_2, tmp_1, ca, bari_blend(ca, cb, cc, b2), bari_blend(ca, cb, cc, b1), pix, depth);
#else
	draw_trg(a, tmp_1, c, ca, cb, cc, pix, depth);
	draw_trg(a, tmp_2, tmp_1, ca, cb, cc, pix, depth);
#endif
}
inline void slice_y_out(vec3 a, vec3 b, vec3 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth, int y) {
	vec3 tmp_1 = nvec3(c.x + ((b.x - c.x) / (b.y - c.y))*(y - c.y), y, b.z);
	vec3 tmp_2 = nvec3(a.x + ((b.x - a.x) / (b.y - a.y))*(y - a.y), y, b.z);
	vec3 b1 = baricentric_coords(a, b, c, tmp_1);
	vec3 b2 = baricentric_coords(a, b, c, tmp_2);
	tmp_1.z = bari_blend_float(a.z, b.z, c.z, b1);
	tmp_2.z = bari_blend_float(a.z, b.z, c.z, b2);
#if DBG_CULLING_MODE == 0
	draw_trg(a, tmp_1, c, ca, bari_blend(ca, cb, cc, b1), cc, pix, depth);
	draw_trg(a, tmp_2, tmp_1, ca, bari_blend(ca, cb, cc, b2), bari_blend(ca, cb, cc, b1), pix, depth);
#else
	draw_trg(a, tmp_1, c, ca, cb, cc, pix, depth);
	draw_trg(a, tmp_2, tmp_1, ca, cb, cc, pix, depth);
#endif
}

#define a_x_test_wi a.x > WIDTH
#define a_x_test_ze a.x < 0
#define a_y_test_he a.y > HEIGHT
#define a_y_test_ze a.y < 0

#define b_x_test_wi b.x > WIDTH
#define b_x_test_ze b.x < 0
#define b_y_test_he b.y > HEIGHT
#define b_y_test_ze b.y < 0

#define c_x_test_wi c.x > WIDTH
#define c_x_test_ze c.x < 0
#define c_y_test_he c.y > HEIGHT
#define c_y_test_ze c.y < 0
int hard_edge_test(vec3 a, vec3 b, vec3 c, vec4 ca, vec4 cb, vec4 cc, dynamic_uint8_t pix, dynamic_float depth) {
	// pizdec
	if (a_x_test_wi) {
		if (b_x_test_wi) {slice_x_out_single(b, c, a, cb, cc, ca, pix, depth, WIDTH); return 0;}
		if (c_x_test_wi) {slice_x_out_single(a, b, c, ca, cb, cc, pix, depth, WIDTH); return 0;}
		slice_x_out(c, a, b, cc, ca, cb, pix, depth, WIDTH);
		return 0;
	}
	if (b_x_test_wi) {
		if (a_x_test_wi) {slice_x_out_single(b, c, a, cb, cc, ca, pix, depth, WIDTH); return 0;}
		if (c_x_test_wi) {slice_x_out_single(c, a, b, cc, ca, cb, pix, depth, WIDTH); return 0;}
		slice_x_out(a, b, c, ca, cb, cc, pix, depth, WIDTH);
		return 0;
	}
	if (c_x_test_wi) {
		if (a_x_test_wi) {slice_x_out_single(a, b, c, ca, cb, cc, pix, depth, WIDTH); return 0;}
		if (b_x_test_wi) {slice_x_out_single(c, a, b, cc, ca, cb, pix, depth, WIDTH); return 0;}
		slice_x_out(b, c, a, cb, cc, ca, pix, depth, WIDTH);
		return 0;
	}

	if (a_x_test_ze) {
		if (b_x_test_ze) {slice_x_out_single(b, c, a, cb, cc, ca, pix, depth, 0); return 0;}
		if (c_x_test_ze) {slice_x_out_single(a, b, c, ca, cb, cc, pix, depth, 0); return 0;}
		slice_x_out(c, a, b, cc, ca, cb, pix, depth, 0);
		return 0;
	}
	if (b_x_test_ze) {
		if (a_x_test_ze) {slice_x_out_single(b, c, a, cb, cc, ca, pix, depth, 0); return 0;}
		if (c_x_test_ze) {slice_x_out_single(c, a, b, cc, ca, cb, pix, depth, 0); return 0;}
		slice_x_out(a, b, c, ca, cb, cc, pix, depth, 0);
		return 0;
	}
	if (c_x_test_ze) {
		if (a_x_test_ze) {slice_x_out_single(a, b, c, ca, cb, cc, pix, depth, 0); return 0;}
		if (b_x_test_ze) {slice_x_out_single(c, a, b, cc, ca, cb, pix, depth, 0); return 0;}
		slice_x_out(b, c, a, cb, cc, ca, pix, depth, 0);
		return 0;
	}

	if (a_y_test_he) {
		if (b_y_test_he) {slice_y_out_single(b, c, a, cb, cc, ca, pix, depth, HEIGHT); return 0;}
		if (c_y_test_he) {slice_y_out_single(a, b, c, ca, cb, cc, pix, depth, HEIGHT); return 0;}
		slice_y_out(c, a, b, cc, ca, cb, pix, depth, HEIGHT);
		return 0;
	}
	if (b_y_test_he) {
		if (a_y_test_he) {slice_y_out_single(b, c, a, cb, cc, ca, pix, depth, HEIGHT); return 0;}
		if (c_y_test_he) {slice_y_out_single(c, a, b, cc, ca, cb, pix, depth, HEIGHT); return 0;}
		slice_y_out(a, b, c, ca, cb, cc, pix, depth, HEIGHT);
		return 0;
	}
	if (c_y_test_he) {
		if (a_y_test_he) {slice_y_out_single(a, b, c, ca, cb, cc, pix, depth, HEIGHT); return 0;}
		if (b_y_test_he) {slice_y_out_single(c, a, b, cc, ca, cb, pix, depth, HEIGHT); return 0;}
		slice_y_out(b, c, a, cb, cc, ca, pix, depth, HEIGHT);
		return 0;
	}

	if (a_y_test_ze) {
		if (b_y_test_ze) {slice_y_out_single(b, c, a, cb, cc, ca, pix, depth, 0); return 0;}
		if (c_y_test_ze) {slice_y_out_single(a, b, c, ca, cb, cc, pix, depth, 0); return 0;}
		slice_y_out(c, a, b, cc, ca, cb, pix, depth, 0);
		return 0;
	}
	if (b_y_test_ze) {
		if (a_y_test_ze) {slice_y_out_single(b, c, a, cb, cc, ca, pix, depth, 0); return 0;}
		if (c_y_test_ze) {slice_y_out_single(c, a, b, cc, ca, cb, pix, depth, 0); return 0;}
		slice_y_out(a, b, c, ca, cb, cc, pix, depth, 0);
		return 0;
	}
	if (c_y_test_ze) {
		if (a_y_test_ze) {slice_y_out_single(a, b, c, ca, cb, cc, pix, depth, 0); return 0;}
		if (b_y_test_ze) {slice_y_out_single(c, a, b, cc, ca, cb, pix, depth, 0); return 0;}
		slice_y_out(b, c, a, cb, cc, ca, pix, depth, 0);
		return 0;
	}
	return 1;
}
