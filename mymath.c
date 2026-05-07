#define PI 3.14159265359f
// vec3
typedef struct {
	float x;
	float y;
	float z;
	float w; //secret dick
} vec3;

vec3 nvec3(float x, float y, float z) {
	return (vec3){x,y,z,1.0f};
}
vec3 lerpv(vec3 a, vec3 b, float t) {
	return nvec3((1.0f - t) * a.x + t * b.x, (1.0f - t) * a.y + t * b.y, (1.0f - t) * a.z + t * b.z);
}
vec3 scal_div_vec3(vec3 v, float s) {
	v.x/=s; v.y/=s; v.z/=s;
	return v;
}
vec3 scal_mul_vec3(vec3 v, float s) {
	v.x*=s; v.y*=s; v.z*=s;
	return v;
}
void pvec3(vec3 v) {
	printf("{%f, %f, %f)\n",v.x,v.y,v.z);
}
void pvec3nn(vec3 v) {
	printf("{%f, %f, %f)",v.x,v.y,v.z);
}
float max3(float x, float y, float z) {
	return x > y ? (x > z ? x : z) : (y > z ? y : z);
}
float min3(float x, float y, float z) {
	return x < y ? (x > z ? z : x) : (y > z ? z : y);
}

//mat 4

typedef struct {
	float arr[4][4];
} mat4;

mat4 nmat4() {
	return (mat4){1,0,0,0,
				  0,1,0,0,
				  0,0,1,0,
				  0,0,0,1};
}
void pmat4(mat4 m) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f,", m.arr[i][j]);
		}
		printf("\n");
	}
}
mat4 mulmat4(mat4 m1, mat4 m2) {
	mat4 ret;
	
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			ret.arr[i][j] = m1.arr[0][j] * m2.arr[i][0] + m1.arr[1][j] * m2.arr[i][1] + m1.arr[2][j] * m2.arr[i][2] + m1.arr[3][j] * m2.arr[i][3];
		}
	}

	return ret;
}
float deg2rad(float deg) {
	return deg * (PI/180.0f);
}
float rad2deg(float rad) {
	return rad * (180.0f/PI);
}
mat4 rotation_x_mat4(float rx) {
	mat4 ret = nmat4();
	ret.arr[1][1] =  cos(rx);
	ret.arr[2][2] =  cos(rx);
	ret.arr[1][2] =  sin(rx);
	ret.arr[2][1] = -sin(rx);
	return ret;
}
mat4 rotation_y_mat4(float rx) {
	mat4 ret = nmat4();
	ret.arr[0][0] =  cos(rx);
	ret.arr[2][2] =  cos(rx);
	ret.arr[2][0] =  sin(rx);
	ret.arr[0][2] = -sin(rx);
	return ret;
}
mat4 rotation_z_mat4(float rx) {
	mat4 ret = nmat4();
	ret.arr[0][0] =  cos(rx);
	ret.arr[1][1] =  cos(rx);
	ret.arr[0][1] =  sin(rx);
	ret.arr[1][0] = -sin(rx);
	return ret;
}
mat4 scale_mat4(vec3 s) {
	mat4 ret = nmat4();
	ret.arr[0][0] =  s.x;
	ret.arr[1][1] =  s.y;
	ret.arr[2][2] =  s.z;
	return ret;
}
mat4 translate_mat4(vec3 p) {
	mat4 ret = nmat4();
	ret.arr[0][3] = p.x;
	ret.arr[1][3] = p.y;
	ret.arr[2][3] = p.z;
	return ret;
}
mat4 projection_mat4(float a, float fov, float far, float near) {
	mat4 m = nmat4();
	
	m.arr[0][0] = 1 / tan(fov / 2) * a;
	m.arr[1][1] = 1 / tan(fov / 2);
	m.arr[2][2] = -(far + near) / (far - near);
	m.arr[2][3] = (-2 * far * near) / (far - near);
	m.arr[3][2] = 1;
	m.arr[3][3] = 0;

	return m;
}
mat4 to_screen_mat4(float w, float h) {
	mat4 m = nmat4();
	
	m.arr[0][0] = w / 2;
	m.arr[1][1] =-h / 2;
	m.arr[0][3] = w / 2;
	m.arr[1][3] = h / 2;

	return m;
}
vec3 mulmat4vec3(mat4 m, vec3 v) {
	v.x = m.arr[0][0] * v.x + m.arr[0][1] * v.y + m.arr[0][2] * v.z + m.arr[0][3] * v.w;
	v.y = m.arr[1][0] * v.x + m.arr[1][1] * v.y + m.arr[1][2] * v.z + m.arr[1][3] * v.w;
	v.z = m.arr[2][0] * v.x + m.arr[2][1] * v.y + m.arr[2][2] * v.z + m.arr[2][3] * v.w;
	v.w = m.arr[3][0] * v.x + m.arr[3][1] * v.y + m.arr[3][2] * v.z + m.arr[3][3] * v.w;	
	return v;
}
