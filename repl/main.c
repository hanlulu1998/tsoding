#include <stddef.h>
#include <stdio.h>
#include <dlfcn.h>
#include <ffi.h>
#include <stdlib.h>
#include <string.h>
#define STB_C_LEXER_IMPLEMENTATION
#include "stb_c_lexer.h"
#define DA_APPEND(da,value) \
		do {\
			if((da).count >= (da).capacity){\
				if((da).capacity == 0) (da).capacity = 256;\
				else (da).capacity *= 2;\
				(da).items = realloc((da).items,(da).capacity*sizeof(*((da).items)));\
			}\
			(da).items[(da).count++] = (value);\
		} while(0)

#define LINE_TRIM(line) \
	do {\
		size_t len = strlen(line);\
		if(len > 0 && line[len-1] == '\n') line[len-1] = '\0';\
	} while(0)

#define DA_LEN(da) (da).count
#define DA_CLEAR(da) \
	do{\
		(da).count = 0;\
		(da).capacity = 0;\
		free((da).items);\
		(da).items = NULL;\
	} while(0)

typedef struct {
	ffi_type ** items;
	size_t count;
	size_t capacity;
} Types;

typedef struct {
	void ** items;
	size_t count;
	size_t capacity;
} Values;

static char line[1024];

int main(int argc, char * argv[]){
	
	const char* program_name = argv[0];
	if(argc != 2){
		fprintf(stderr, "Usage: %s <input>\n", program_name);
		fprintf(stderr, "ERROR: no input is provided\n");
		return 1;
	}

	const char* dll_name = argv[1];

	void * dll = dlopen(dll_name, RTLD_NOW);
	if(dll == NULL){
		fprintf(stderr, "ERROR: cannot open dll: %s\n",dlerror());
		return 1;
	}

	ffi_cif cif = {0};
	Types args = {0};
	Values values = {0};
	
	stb_lexer l ={0};
	static char string_store[1024];
	
	for(;;){
next:
		printf("> ");
		if(fgets(line,sizeof(line),stdin)==NULL) break;
    fflush(stdout);
		LINE_TRIM(line);
    if (strcmp(line, "q")==0) break;

		stb_c_lexer_init(&l, line, line+strlen(line), string_store, sizeof(string_store));
		if(!stb_c_lexer_get_token(&l)) continue;

		if(l.token != CLEX_id){
			printf("ERROR: function name must be an identifier\n");
			continue;
		}
    
		void * fn = dlsym(dll, l.string);
    if(fn == NULL){
			printf("ERROR: no function %s found\n",l.string);
			continue;
		}
		args.count = 0;
		values.count = 0;

		int int_arg_num = 0;
		int string_arg_num = 0;
		long int_ptrs[8]={0};
		char string_buf[8][256]={0};
		char * string_ptrs[8] = {0};
		
		while(stb_c_lexer_get_token(&l)){
			switch (l.token) {
				case CLEX_intlit:{
					DA_APPEND(args, &ffi_type_sint32);
					int_ptrs[int_arg_num] = l.int_number;
					DA_APPEND(values, int_ptrs+int_arg_num);
					int_arg_num++;
				} break;
				case CLEX_dqstring:{
					DA_APPEND(args, &ffi_type_pointer);
					strcpy(string_buf[string_arg_num], l.string);
					string_ptrs[string_arg_num] = string_buf[string_arg_num];
					DA_APPEND(values, string_ptrs+string_arg_num);
					string_arg_num++;
				} break;
				default:{
					printf("ERROR: invalid argument token\n");
					goto next;
				}break;
			}
		}

		ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, args.count, &ffi_type_void, args.items);
		if(status != FFI_OK){
			printf("ERROR: cannot create ffi_cif\n");
			continue;
		}
		ffi_call(&cif, fn, NULL, values.items);
	}

	printf("Bye\n");
	dlclose(dll);
	DA_CLEAR(args);
	DA_CLEAR(values);
	return 0;
}
