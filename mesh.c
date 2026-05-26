typedef struct {
	vec4 color;
} material_t;
typedef struct {
	dynamic_vec3 vertices;
	dynamic_vec3 normales;
	dynamic_vec2 uvs;
	vec3 position;
	vec3 rotation;
	vec3 scale;

	material_t mtl;
} mesh_t;

mesh_t nmesh(dynamic_vec3 v, dynamic_vec3 n, dynamic_vec2 u, vec3 pos, vec3 rot, vec3 scl) {
	return (mesh_t){v,n,u,pos,rot,scl, (material_t){nvec4(1.0f, 1.0f, 1.0f, 1.0f)}};
}

void draw(mesh_t m, camera_t viewer, mat4 proj, mat4 toscr, dynamic_uint8_t pix, dynamic_float depth) {
	mat4 view = get_camera_view(viewer);
	mat4 rot = mulmat4(rotation_z_mat4(m.rotation.z), mulmat4(rotation_y_mat4(m.rotation.y), rotation_x_mat4(m.rotation.x)));
	mat4 scale = scale_mat4(m.scale);
	mat4 mod = mulmat4(rot, mulmat4(scale, translate_mat4(m.position)));
	mat4 fin = mulmat4(mod, mulmat4(view, mulmat4(proj, toscr)));

	dynamic_vec3 v = clone_vec3(&m.vertices);
	for (size_t i = 3; i < v.size+3; i+=3) {
		vec4 a = vec3tovec4(v.arr[i-1], 1.0f);
		vec4 b = vec3tovec4(v.arr[i-2], 1.0f);
		vec4 c = vec3tovec4(v.arr[i-3], 1.0f);
		
		a = mulmat4vec4(fin, a);
		a = w_transformation(a);
		b = mulmat4vec4(fin, b);
		b = w_transformation(b);
		c = mulmat4vec4(fin, c);
		c = w_transformation(c);
		
		vec3 n = normalize3(cross3(minus3(vec4tovec3(b), vec4tovec3(a)), minus3(vec4tovec3(c), vec4tovec3(a))));
		if (n.z >= 0.0f) {
#if DBG_CULLING_MODE == 0
			// light source in 0,0,0
			float l = (dot3(vec4tovec3(mulmat4vec4(rot, vec3tovec4(m.normales.arr[i-1], 1.0f))), normalize3(minus3(nvec3(0,0,0), vec4tovec3(mulmat4vec4(mod, vec3tovec4(m.vertices.arr[i-1], 1.0f)))))) + 1.0f) / 2.0f;
			vec4 col = nvec4(l, l, l, 1.0f);
			draw_trg(vec4tovec3(a), vec4tovec3(b), vec4tovec3(c), col, col, col, pix, depth);
#else
			draw_trg(vec4tovec3(a), vec4tovec3(b), vec4tovec3(c), nvec4(1.0f, 0.0f, 0.0f, 0.0f), nvec4(0.0f, 1.0f, 0.0f, 1.0f), nvec4(0.0f, 0.0f, 1.0f, 1.0f), pix, depth);
#endif
		}
	}
	dealloc_vec3(&v);
}
mesh_t create_mesh_from_obj(char* path) {
	mesh_t ret;
	ret.vertices = malloc_vec3(0);
	ret.normales = malloc_vec3(0);
	ret.position = nvec3(0,0,0);
	ret.rotation = nvec3(0,0,0);
	ret.scale = nvec3(1,1,1);
	ret.mtl.color = nvec4(1,1,1,1);
	
	FILE* f = fopen(path, "r");
	if (f == NULL) {
		printf("ERROR: can not find file \"%s\"!\n", path);
		exit(-1);
	}

	dynamic_char obj = malloc_char(0);
	int c;
	while ((c = fgetc(f)) != EOF) {
		put_char(&obj, c);
	}
	put_char(&obj, '\0');
	fclose(f);


	dynamic_float tv = malloc_float(0); //vertexes
	dynamic_float tn = malloc_float(0); //noramls

	char mod = 'S';
	//std::vector<std::string> key_words = {"o", "f", "vn", "vt", "v", "usemtl", "s", "#", "g"};
	//char key_words_mod[9] = "ofntvms#g";
	dynamic_char word = malloc_char(0);

	for(size_t i = 0; i < obj.size; i++) {
		if (obj.arr[i] == ' ' || obj.arr[i] == '\n') {
			if (word.size != 0) {
				int skip = 0;

				if (word.arr[0] == 'v') {
					if (word.size == 1) {
						mod = 'v';
						skip = 1;
					}
					else if (word.size > 1 && word.arr[1] == 'n') {
						mod = 'n';
						skip = 1;
					}
					else if (word.size > 1 && word.arr[1] == 't') {
						mod = 'S';
					}
				}
				else if (word.arr[0] == 'f') {
					mod = 'f';
					skip = 1;
				}
				else if (word.arr[0] == 'o') {
					mod = 'S';
				}
				else if (word.arr[0] == 'u') {
					mod = 'S';
				}
				else if (word.arr[0] == 's') {
					mod = 'S';
				}
				else if (word.arr[0] == '#') {
					mod = 'S';
				}
				else if (word.arr[0] == 'g') {
					mod = 'S';
				}
				if (mod != 'S' && !skip) {
					put_char(&word, '\0');
					switch(mod) {
						case 'v':
							put_float(&tv, atof(word.arr));
							break;
						case 'n':
							put_float(&tn, atof(word.arr));
							break;
						case 'f':
							dynamic_char buffer = malloc_char(0);
							
							int cnt = 0;
							for (size_t i = 0; i < word.size; i++) {
								if (word.arr[i] != '/' && word.arr[i] != '\0') {put_char(&buffer, word.arr[i]); continue;}
								
								put_char(&buffer, '\0');
								int ib = atoi(buffer.arr);
								if (cnt == 0) {
									put_vec3(&ret.vertices, nvec3(tv.arr[ib * 3 - 3], tv.arr[ib * 3 - 2], tv.arr[ib * 3 - 1]));
								}
								if (cnt == 2) {
									put_vec3(&ret.normales, nvec3(tn.arr[ib * 3 - 3], tn.arr[ib * 3 - 2], tn.arr[ib * 3 - 1]));
								}
								
								cnt++;
								clear_char(&buffer);
							}
							dealloc_char(&buffer);
							break;
					}
				}
				clear_char(&word);
			}
		}
		else put_char(&word, obj.arr[i]);
	}

	dealloc_char(&obj);
	dealloc_float(&tv);
	dealloc_float(&tn);
	return ret;
}
void free_mesh(mesh_t* m) {
	dealloc_vec3(&m->vertices);
	dealloc_vec3(&m->normales);
}
