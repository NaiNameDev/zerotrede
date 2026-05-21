typedef struct {
	vec4 position;
	vec4 rotation;
	vec4 forward;
	vec4 up;
} camera;

camera ncamera(vec4 p, vec4 r, vec4 f, vec4 u) {
	return (camera){p,r,f,u};
}
mat4 get_camera_view(camera cam) {
	mat4 r = mulmat4(rotation_x_mat4(cam.rotation.x), mulmat4(rotation_y_mat4(cam.rotation.y), rotation_z_mat4(cam.rotation.z)));
	return look_at_mat4(mulmat4vec4(r, cross3(cam.up, cam.forward)), mulmat4vec4(r, cam.up), mulmat4vec4(r, cam.forward), mulmat4vec4(r, cam.position));
}
