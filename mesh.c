typedef struct {
	dynamic_vec3 vertices;
	dynamic_vec3 normales;
	vec3 position;
	vec3 rotation;
	vec3 scale;
} mesh;

mesh nmesh(dynamic_vec3 v, dynamic_vec3 n, vec3 pos, vec3 rot, vec3 scl) {
	return (mesh){v,n,pos,rot,scl};
}

dynamic_vec3 project_vertices(mesh* m, mat4 view, mat4 proj, mat4 toscr) {
	mat4 rot = mulmat4(rotation_x_mat4(m->rotation.x), mulmat4(rotation_y_mat4(m->rotation.y), rotation_z_mat4(m->rotation.z)));
	mat4 model = mulmat4(rot, mulmat4(scale_mat4(m->scale), translate_mat4(m->position)));

	dynamic_vec3 v = clone_vec3(&m->vertices);
	for (size_t i = 0; i < v.size; i++) {
		v.arr[i] = mulmat4vec3(model, v.arr[i]);
		v.arr[i] = mulmat4vec3(view, v.arr[i]);
		v.arr[i] = mulmat4vec3(proj, v.arr[i]);
		v.arr[i] = mulmat4vec3(toscr, v.arr[i]);
		v.arr[i] = scal_div_vec3(v.arr[i], v.arr[i].w);
	}

	return v;
}
void calculate_normals(mesh* m) {
	
}
mesh create_from_obj(char* path) {
	mesh ret;
	ret.position = nvec3(0,0,0);
	ret.rotation = nvec3(0,0,0);
	ret.scale = nvec3(0,0,0);
	FILE* f = fopen(path, "r");
	
	if (f == NULL) {
		printf("CANT FIND FILE %s ABORTING!\n", path);
		exit(-1);
	}

	dynamic_char obj = malloc_char(0);
	int i = 0, c;
	while ((c = fgetc(f)) != EOF) {
		put_char(&obj, c);
	}
	put_char(&obj, '\0');

	

	dealloc_char(&obj);
	fclose(f);
}
