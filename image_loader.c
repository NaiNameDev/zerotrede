typedef struct {
	uint8_t* image;
	unsigned int width;
	unsigned int height;
	unsigned int channels;
} image;

image load_image(char* path) {
    int width, height, channels;
    uint8_t *data = stbi_load(path, &width, &height, &channels, 4);
    
    if (data == NULL) {
		printf("ERROR: can not find file \"%s\"!\n", path);
		exit(-1);
    }

    return (image){data, width, height, channels};
}

void free_image(image* i) {
	stbi_image_free(i->image);
}

