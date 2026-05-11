typedef struct {
	vec4 color;
} material;
typedef struct {
	dynamic_vec4 vertices;
	dynamic_vec4 normales;
	vec4 position;
	vec4 rotation;
	vec4 scale;

	material mtl;
} mesh;

mesh nmesh(dynamic_vec4 v, dynamic_vec4 n, vec4 pos, vec4 rot, vec4 scl) {
	return (mesh){v,n,pos,rot,scl, (material){nvec4(1.0f, 1.0f, 1.0f, 1.0f)}};
}

void draw(mesh m, mat4 view, mat4 proj, mat4 toscr, SDL_Surface* sr, dynamic_uint8_t pix, dynamic_float depth) {
	//backward matrix multiplication is not work here idk why
	mat4 rot = mulmat4(rotation_x_mat4(m.rotation.x), mulmat4(rotation_y_mat4(m.rotation.y), rotation_z_mat4(m.rotation.z)));
	mat4 mod = mulmat4(rot, mulmat4(scale_mat4(m.scale), translate_mat4(m.position)));
	mat4 fin = mulmat4(mod, mulmat4(view, mulmat4(proj, toscr)));

	dynamic_vec4 v = clone_vec4(&m.vertices);
	for (size_t i = 0; i < v.size; i++) {
		v.arr[i] = mulmat4vec4(fin, v.arr[i]);
		v.arr[i] = scal_div_vec4(v.arr[i], v.arr[i].w);
	}

	for (int i = 0; i < v.size; i+=3) {
		vec4 tn = cross3(minus3(m.vertices.arr[i+1], m.vertices.arr[i]), minus3(m.vertices.arr[i+2], m.vertices.arr[i]));
		if (dot3(normalize3(mulmat4vec4(rot, tn)), nvec4(0,0,1,0)) >= 0.0f) {
			draw_trg(v.arr[i], v.arr[i+1], v.arr[i+2], nvec4(1,0,0,1), nvec4(0,1,0,1), nvec4(0,0,1,1), sr, pix, depth);
		}
	}
	dealloc_vec4(&v);
}
mesh create_mesh_from_obj(char* path) {
	mesh ret;
	ret.vertices = malloc_vec4(0);
	ret.normales = malloc_vec4(0);
	ret.position = nvec4(0,0,0,1);
	ret.rotation = nvec4(0,0,0,1);
	ret.scale = nvec4(1,1,1,1);
	ret.mtl.color = nvec4(1,1,1,1);
	
	FILE* f = fopen(path, "r");
	if (f == NULL) {
		printf("ERROR: can not find file \"%s\"!\n", path);
		exit(-1);
	}

	dynamic_char obj = malloc_char(0);
	int i = 0, c;
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

	for(int i = 0; i < obj.size; i++) {
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
								if (word.arr[i] != '/') {put_char(&buffer, word.arr[i]); continue;}
								
								put_char(&buffer, '\0');
								int ib = atoi(buffer.arr);
								if (cnt == 0) {
									put_vec4(&ret.vertices, nvec4(tv.arr[ib * 3 - 3], tv.arr[ib * 3 - 2], tv.arr[ib * 3 - 1] ,1.0f));
								}
								if (cnt == 1) {
									put_vec4(&ret.normales, nvec4(tn.arr[ib * 3 - 3], tn.arr[ib * 3 - 2], tn.arr[ib * 3 - 1], 1.0f));
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
void free_mesh(mesh* m) {
	dealloc_vec4(&m->vertices);
	dealloc_vec4(&m->normales);
}
