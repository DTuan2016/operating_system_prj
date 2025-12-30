#include <list_syscall.h>
#include <ctype.h>
#include <string.h>

void init_list(list_syscall *list){
    list->value = NULL;
    list->num_sys = 0;
}

void destroy_list(list_syscall *list){
    free(list->value);
    list->num_sys = 0;
}

int push_to_list(list_syscall *list, long value){
    long *tmp = realloc(list->value, (list->num_sys + 1) * sizeof(long));
    if(!tmp){
        return -1;
    }
    list->value = tmp;
    list->value[list->num_sys++] = value;
    return 0;
}

int has_in_list(list_syscall *list, long value){
    for(size_t i = 0; i < list->num_sys; i++){
        if(list->value[i] == value){
            return 1;
        }
    }
    return 0;
}

int parse_list(char *s, list_syscall *list){
    if(!s || !*s){
        return 0;
    }

    char *tmp = strdup(s);
    if(!tmp){
        return 1;
    }

    char *tok = strtok(tmp, ",");
    int result = 0;
    while(tok){
        while(*tok == ' ') tok++;
        if(isdigit((unsigned char)tok[0])){
            long value = atol(tok);

            if(push_to_list(list, value) != 0){
                result = -1;
                break;
            }

        } else {
            int nr = syscall_lookup_nr(tok);
            if(nr < 0){
                LOG_ERROR("Unknown syscall name: '%s'\n", tok);
                result = -1;
                break;
            }

            if(push_to_list(list, nr) != 0){
                result = -1;
                break;
            }
        }

        tok = strtok(NULL, ",");
    }
    free(tmp);
    return result;
}