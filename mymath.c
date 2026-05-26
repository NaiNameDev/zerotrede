#define PI 3.14159265359f
// vec4
typedef struct {
	float x;
	float y;
	float z;
	float w;
} vec4;

inline vec4 nvec4(float x, float y, float z, float w) {
	return (vec4){x,y,z,w};
}
inline vec4 lerpv4(vec4 a, vec4 b, float t) {
	return nvec4((1.0f - t) * a.x + t * b.x, (1.0f - t) * a.y + t * b.y, (1.0f - t) * a.z + t * b.z, 1.0f);
}
inline vec4 scal_div_vec4(vec4 v, float s) {
	v.x/=s; v.y/=s; v.z/=s;
	return v;
}

typedef struct {
	float x;
	float y;
	float z;
} vec3;
typedef struct {
	float x;
	float y;
} vec2;

inline vec2 nvec2(float x, float y) {
	return (vec2){x, y};
}

inline float clampf(float n, float min, float max) {
	return n > min ? (n < max ? n : max) : min;
}
inline vec4 w_transformation(vec4 v) {
	float w = clampf(v.w, -FAR, -NEAR/2);
	return (vec4){v.x / w, v.y / w, w, w};
}

inline vec3 nvec3(float x, float y, float z) {
	return (vec3){x,y,z};
}
inline vec3 lerpv3(vec3 a, vec3 b, float t) {
	return nvec3((1.0f - t) * a.x + t * b.x, (1.0f - t) * a.y + t * b.y, (1.0f - t) * a.z + t * b.z);
}
inline vec4 vec3tovec4(vec3 v, float w) {
	return (vec4){v.x, v.y, v.z, w};
}
inline vec3 vec4tovec3(vec4 v) {
	return (vec3){v.x, v.y, v.z};
}

inline float lerp(float a, float b, float t) {
	return (1.0f - t) * a + t * b;
}
inline vec3 scal_mul_vec3(vec3 v, float s) {
	v.x*=s; v.y*=s; v.z*=s;
	return v;
}
inline vec3 mulvec3(vec3 v1, vec3 v2) {
	return nvec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
inline float dot3(vec3 v1, vec3 v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
inline float lenght3(vec3 v) {
	return sqrt(dot3(v,v));
}
inline vec3 normalize3(vec3 v) {
	float l = lenght3(v);
	v.x /= l; v.y /= l; v.z /= l;
	return v;
}
inline vec3 minus3(vec3 v1, vec3 v2) {
	return (vec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}
inline vec3 plus3(vec3 v1, vec3 v2) {
	return (vec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}
inline vec3 cross3(vec3 v1, vec3 v2) {
	return (vec3){(v1.y * v2.z - v1.z * v2.y), (v1.x * v2.z - v1.z * v2.x), (v1.x * v2.y - v1.y * v2.x)};
}
inline vec3 abs3(vec3 v) {
	return nvec3(fabsf(v.x), fabsf(v.y), fabsf(v.z));
}
inline void pvec3(vec3 v) {
	printf("(%f, %f, %f, %f)\n",v.x,v.y,v.z);
}
inline void pvec3nn(vec3 v) {
	printf("(%f, %f, %f, %f)",v.x,v.y,v.z);
}
inline float max3(float x, float y, float z) {
	return x > y ? (x > z ? x : z) : (y > z ? y : z);
}
inline float min3(float x, float y, float z) {
	return x < y ? (x > z ? z : x) : (y > z ? z : y);
}

typedef struct {
	float arr[3][3];
} mat3;

inline mat3 nmat3() {
	return (mat3){1.0f,0.0f,0.0f,
				  0.0f,1.0f,0.0f,
				  0.0f,0.0f,1.0f};
}

inline vec3 mulmat3vec3(mat3 m, vec4 v) {
	vec3 ret = nvec3(1,1,1);

	ret.x = m.arr[0][0] * v.x + m.arr[0][1] * v.y + m.arr[0][2] * v.z;
	ret.y = m.arr[1][0] * v.x + m.arr[1][1] * v.y + m.arr[1][2] * v.z;
	ret.z = m.arr[2][0] * v.x + m.arr[2][1] * v.y + m.arr[2][2] * v.z;
	
	return ret;
}
//mat 4

typedef struct {
	float arr[4][4];
} mat4;

inline mat4 nmat4() {
	return (mat4){1.0f,0.0f,0.0f,0.0f,
				  0.0f,1.0f,0.0f,0.0f,
				  0.0f,0.0f,1.0f,0.0f,
				  0.0f,0.0f,0.0f,1.0f};
}
inline void pmat4(mat4 m) {
	for (int i = 0; i < 4; i++) {
		printf("|");
		for (int j = 0; j < 4; j++) {
			printf("%f,", m.arr[i][j]);
		}
		printf("|\n");
	}
}
inline vec4 mulmat4vec4(mat4 m, vec4 v) {
	vec4 ret = nvec4(1,1,1,1);

	ret.x = m.arr[0][0] * v.x + m.arr[0][1] * v.y + m.arr[0][2] * v.z + m.arr[0][3] * v.w;
	ret.y = m.arr[1][0] * v.x + m.arr[1][1] * v.y + m.arr[1][2] * v.z + m.arr[1][3] * v.w;
	ret.z = m.arr[2][0] * v.x + m.arr[2][1] * v.y + m.arr[2][2] * v.z + m.arr[2][3] * v.w;
	ret.w = m.arr[3][0] * v.x + m.arr[3][1] * v.y + m.arr[3][2] * v.z + m.arr[3][3] * v.w;	
	
	return ret;
}
inline vec3 mulmat4vec3(mat4 m, vec3 v) {
	vec3 ret = nvec3(1,1,1);

	ret.x = m.arr[0][0] * v.x + m.arr[0][1] * v.y + m.arr[0][2] * v.z + m.arr[0][3] * 1.0f;
	ret.y = m.arr[1][0] * v.x + m.arr[1][1] * v.y + m.arr[1][2] * v.z + m.arr[1][3] * 1.0f;
	ret.z = m.arr[2][0] * v.x + m.arr[2][1] * v.y + m.arr[2][2] * v.z + m.arr[2][3] * 1.0f;
	
	return ret;
}
inline mat4 mulmat4(mat4 m1, mat4 m2) {
	mat4 ret;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			ret.arr[i][j] = 0;
			for (int k = 0; k < 4; k++) {
				ret.arr[i][j] += m1.arr[k][j] * m2.arr[i][k];
			}
		}
	}

	return ret;
}
inline float deg2rad(float deg) {
	return deg * (PI/180.0f);
}
inline float rad2deg(float rad) {
	return rad * (180.0f/PI);
}
inline mat4 rotation_x_mat4(float rx) {
	mat4 ret = nmat4();
	
	ret.arr[1][1] =  cos(rx);
	ret.arr[2][2] =  cos(rx);
	ret.arr[1][2] =  sin(rx);
	ret.arr[2][1] = -sin(rx);
	
	return ret;
}
inline mat4 rotation_y_mat4(float rx) {
	mat4 ret = nmat4();
	
	ret.arr[0][0] =  cos(rx);
	ret.arr[2][2] =  cos(rx);
	ret.arr[2][0] =  sin(rx);
	ret.arr[0][2] = -sin(rx);
	
	return ret;
}
inline mat4 rotation_z_mat4(float rx) {
	mat4 ret = nmat4();
	
	ret.arr[0][0] =  cos(rx);
	ret.arr[1][1] =  cos(rx);
	ret.arr[0][1] =  sin(rx);
	ret.arr[1][0] = -sin(rx);
	
	return ret;
}
inline mat4 scale_mat4(vec3 s) {
	mat4 ret = nmat4();
	
	ret.arr[0][0] =  s.x;
	ret.arr[1][1] =  s.y;
	ret.arr[2][2] =  s.z;
	
	return ret;
}
inline mat4 translate_mat4(vec3 p) {
	mat4 ret = nmat4();
	
	ret.arr[0][3] = p.x;
	ret.arr[1][3] = p.y;
	ret.arr[2][3] = p.z;
	
	return ret;
}
inline mat4 look_at_mat4(vec3 left, vec3 up, vec3 forward, vec3 position) {
	mat4 ret = nmat4();
	
	ret.arr[0][0] = left.x;
	ret.arr[1][0] = left.y;
	ret.arr[2][0] = left.z;
	ret.arr[0][1] = up.x;
	ret.arr[1][1] = up.y;
	ret.arr[2][1] = up.z;
	ret.arr[0][2] = forward.x;
	ret.arr[1][2] = forward.y;
	ret.arr[2][2] = forward.z;
	ret.arr[0][3] = position.x;
	ret.arr[1][3] = position.y;
	ret.arr[2][3] = position.z;

	return ret;
}
inline mat4 projection_mat4(float a, float fov, float far, float near) {
	mat4 m = nmat4();
	
	m.arr[0][0] = 1 / tan(fov / 2) * a;
	m.arr[1][1] = 1 / tan(fov / 2);
	m.arr[2][2] = -(far + near) / (far - near);
	m.arr[2][3] = (-2 * far * near) / (far - near);
	m.arr[3][2] = 1;
	m.arr[3][3] = 0;

	return m;
}
inline mat4 to_screen_mat4(float w, float h) {
	mat4 m = nmat4();
	
	m.arr[0][0] = w / 2;
	m.arr[1][1] =-h / 2;
	m.arr[0][3] = w / 2;
	m.arr[1][3] = h / 2;

	return m;
}
