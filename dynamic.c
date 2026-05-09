#define DEFINE_DYNAMYC_TYPE(TYPE) \
typedef struct {\
	size_t size;\
	TYPE* arr;\
} dynamic_##TYPE;\
\
dynamic_##TYPE malloc_##TYPE(size_t size) { \
	TYPE* ret = (TYPE*)malloc(sizeof(TYPE) * size);\
	return (dynamic_##TYPE){size, ret};\
}\
dynamic_##TYPE clone_##TYPE(dynamic_##TYPE* o) {\
	dynamic_##TYPE ret = malloc_##TYPE(o->size);\
	memcpy(ret.arr, o->arr, o->size * sizeof(TYPE));\
	return ret;\
}\
void dealloc_##TYPE(dynamic_##TYPE* d) {\
	free(d->arr);\
	d->arr = NULL;\
}\
void clear_##TYPE(dynamic_##TYPE* d) {\
	dealloc_##TYPE(d);\
	*d = malloc_##TYPE(0);\
}\
\
void fill_zeros_##TYPE(dynamic_##TYPE* arr) {\
	memset(arr->arr, 0, arr->size * sizeof(TYPE));\
}\
void put_##TYPE(dynamic_##TYPE* d, TYPE var) {\
	d->arr = realloc(d->arr, sizeof(TYPE) * ++d->size);\
	d->arr[d->size - 1] = var;\
}\
void dput_##TYPE(dynamic_##TYPE* d, dynamic_##TYPE* var) {\
	d->arr = realloc(d->arr, sizeof(TYPE) * (d->size + var->size));\
	memcpy((d->arr + d->size), var->arr, var->size);\
	d->size += var->size;\
}\
void arrput_##TYPE(dynamic_##TYPE* d, TYPE* var, int vsize) {\
	d->arr = realloc(d->arr, sizeof(TYPE) * (d->size + vsize));\
	memcpy((d->arr + d->size), var, vsize * sizeof(TYPE));\
	d->size += vsize;\
}\
void posput_##TYPE(dynamic_##TYPE* d, TYPE var, int pos) {\
	d->arr = realloc(d->arr, sizeof(TYPE) * ++d->size);\
	for (size_t i = d->size; pos < i; i--) {\
		d->arr[i] = d->arr[i - 1];\
	}\
	d->arr[pos] = var;\
}\
void posrem_##TYPE(dynamic_##TYPE* d, int pos) {\
	for (size_t i = pos; i < d->size - 1; i++) {\
		d->arr[i] = d->arr[i + 1];\
	}\
	d->arr = realloc(d->arr, sizeof(TYPE) * --d->size);\
}
