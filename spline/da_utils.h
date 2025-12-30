#ifndef __DA_UTILS_H__
#define __DA_UTILS_H__

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

#endif
