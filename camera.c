typedef struct {
	vec3 position;
	vec3 rotation;
	vec3 forward;
	vec3 up;
} camera_t;

camera_t ncamera(vec3 p, vec3 r, vec3 f, vec3 u) {
	return (camera_t){p,r,f,u};
}
mat4 get_camera_view(camera_t cam) {
	mat4 r = mulmat4(rotation_z_mat4(cam.rotation.z), mulmat4(rotation_y_mat4(cam.rotation.y), rotation_x_mat4(cam.rotation.x)));
	return look_at_mat4(mulmat4vec3(r, cross3(cam.up, cam.forward)), mulmat4vec3(r, cam.up), mulmat4vec3(r, cam.forward), mulmat4vec3(r, cam.position));
}
