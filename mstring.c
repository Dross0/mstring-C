#include <stdio.h>
#include <stdlib.h>
#include "mstring.h"



static string_t * string_create(size_t length){
    string_t * str = (string_t *)malloc(sizeof(string_t));
    if (str == NULL){
        return NULL;
    }
    str->data = (uchar *)malloc(sizeof(uchar) * (length + 1));
    if (str->data == NULL){
        return NULL;
    }
    str->length = length;
    str->real_size = 0;
    return str;
}

void string_free(string_t * string){
    free(string->data);
    free(string);
}

string_t * string_dup(const uchar * str){
    string_t * string = string_create(str_len(str));
    if (string == NULL){
        return NULL;
    }
    memcpy(string->data, str, sizeof(uchar) * (string->length + 1));
    string->real_size = string->length;
    return string;
}

void print_string(string_t * str, FILE * file){
    fwrite(str->data, sizeof(uchar), str->real_size, file);
}

void println_string(string_t * str, FILE * file){
    fwrite(str->data, sizeof(uchar), str->real_size, file);
    fprintf(file, "\n");
}

string_t * string_concat(const string_t * left, const string_t * right){
    string_t * string = string_create(left->length + right->length);
    if (string == NULL){
        return NULL;
    }
    memcpy(string->data, left->data, sizeof(uchar) * left->length);
    memcpy(string->data + left->length, right->data, sizeof(uchar) * (right->length + 1));
    string->real_size = string->length;
    return string;
}

string_t * string_slice(const string_t * string, int start, int end){
    if (start < 0 || end < 0){
        error("string_slice", "start and end must be positive", ValueError);
    }
    if (end > string->real_size){
        error("string_slice", "End index greater than max length of string", IndexError);
    }
    if (start > end){
        error("string_slice", "Start cant be greater than end", ValueError);
    }
    string_t *slice = string_create(end - start);
    if (slice == NULL){
        return NULL;
    }
    size_t i = 0;
    for (i = start; i < end; ++i){
        string_append(slice, string->data[i]);
    }
    return slice;
}

void string_lstrip(string_t * string){
    if (string->real_size < 1){
        return;
    }
    size_t i = 0;
    while (is_space(string->data[i])){
        i++;
    }
    if (i == 0){
        return;
    }    
    size_t j = 0;
    size_t space_finish = i;
    for (j = 0; j <= string->real_size - space_finish; ++j){
        swap(&string->data[i++], &string->data[j], sizeof(uchar));
    }
    string->real_size -= space_finish; 
}

void string_rstrip(string_t * string){
    if (string->real_size < 1){
        return;
    }
    size_t i = string->real_size - 1;
    while (is_space(string->data[i])){
        --i;
    }
    string_remove_last(string, string->real_size - i - 1);
}

void string_strip(string_t * string){
    string_lstrip(string);
    string_rstrip(string);
}

string_t * string_center(const string_t * string, const uchar symbol){
    string_t * res = string_create(string->real_size + 2);
    if (res == NULL){
        return NULL;
    }
    res->real_size = res->length;
    res->data[res->real_size] = '\0';
    res->data[0] = res->data[res->real_size - 1] = symbol;
    size_t i = 0;
    for (i = 1; i < (res->real_size - 1); ++i){
        res->data[i] = string->data[i-1];
    }
    return res;
}

void string_array_free(string_array_t * string_array){
    size_t i = 0;
    for (i = 0; i < string_array->size; ++i){
        string_free(string_array->data[i]);
    }
    free(string_array);
}

static int comp(const string_t * str, const uchar * pat, size_t str_pos, const size_t pat_size){
    size_t i = 0;
    for (i = 0; i < pat_size; ++i){
        if (str_pos == str->real_size || str->data[str_pos++] != pat[i]){
            return 0;
        }
    }
    return 1;
}

string_t * string_replace(const string_t * string, const uchar * old, const uchar * new){
    size_t old_len = str_len(old);
    size_t new_len = str_len(new);
    string_t * new_string = 0;
    if (old_len < new_len){
        new_string = string_create(string->real_size + (string->real_size * (new_len - old_len)));
    }
    else{
        new_string = string_create(string->real_size);
    }
    if (new_string == NULL){
        return NULL;
    }
    int is_equal = 0;
    size_t i = 0;
    size_t j = 0;
    for (i = 0; i < string->real_size; ++i){
        if (string->data[i] == old[0] && (is_equal = comp(string, old, i, old_len))){
            for (j = 0; j < new_len; ++j){
                string_append(new_string, new[j]);
            }
            i += old_len - 1;
        }
        else{
            string_append(new_string, string->data[i]);
        }
    }
    return new_string;
}

string_t * string_join(const string_array_t * str_arr, const uchar * pat){
    size_t len_sum = 0;
    size_t i = 0;
    for (i = 0; i < str_arr->size; ++i){
        len_sum += str_arr->data[i]->real_size;
    }
    size_t pat_len = str_len(pat);
    string_t * string = string_create(len_sum + (pat_len * (str_arr->size - 1)));
    if (string == NULL){
        return NULL;
    }
    size_t j = 0;
    for (i = 0; i < str_arr->size; ++i){
        for (j = 0; j < str_arr->data[i]->real_size; ++j){
            string_append(string, str_arr->data[i]->data[j]);
        }
        if (i != (str_arr->size - 1)){
            for (j = 0; j < pat_len; ++j){
                string_append(string, pat[j]);
            }
        }
    }
    return string;
}


string_array_t * string_split(const string_t * string, const uchar symbol){
    string_array_t * string_arr = (string_array_t *)malloc(sizeof(string_array_t));
    if (string_arr == NULL){
        return NULL;
    }
    string_arr->max_size = 20;
    string_arr->size = 0;
    string_arr->data = (string_t *)malloc(sizeof(string_t *) * string_arr->max_size); 
    if (string_arr->data == NULL){
        return NULL;
    }
    size_t str_size = 20;
    string_t * str = 0;
    size_t i = 0;
    int c = 0;    
    for (i = 0; i < string->real_size; ++i){
        if (string->data[i] == symbol){
            continue;
        }
        str = string_create(str_size);
        while (i < string->real_size && string->data[i] != symbol){
            string_append(str, string->data[i]);
            c++;
            if (c == str->length){
                str_size *= 2;
                str->data = (uchar *)realloc(str->data, str_size * sizeof(uchar));
                str->length = str_size;
                if (str->data == NULL){
                    string_free(str);
                    string_array_free(string_arr);
                    return NULL;
                }
            }
            i++;
        }
        c = 0;
        str_size = 20;
        string_arr->data[string_arr->size++] = str;
        if (string_arr->size == string_arr->max_size){
            string_arr->max_size *= 2;
            string_arr->data = (string_t *)realloc(string_arr->data, string_arr->max_size * sizeof(string_t *));
            if (string_arr->data == NULL){
                string_array_free(string_arr);
                return NULL;
            }
        }
    }
    return string_arr;
}

void string_copy(string_t * dest, const string_t * src){
    size_t i = 0;
    while (i < dest->length){
        if (i == src->real_size){
            return;
        }
        dest->data[i] = src->data[i];
        i++;
    }
    dest->real_size = i;
}

void string_reverse(string_t * string){
    size_t i = 0;
    uchar tmp = 0;
    for (i = 0; i < string->real_size / 2; ++i){
        tmp = string->data[i];
        string->data[i] = string->data[string->real_size - i - 1];
        string->data[string->real_size - i - 1] = tmp;
    }
}

void string_remove_last(string_t * string, const size_t amount){
    string->real_size -= amount;
    if (string->real_size < 0){
        error("string_remove_last", "Amount cant be greater than length", IndexError);
    }
    string->data[string->real_size] = '\0';
}

void string_append(string_t * string, const uchar symbol){
    if (string->real_size == string->length){
        error("string_append", "Max length of string", IndexError);
    }
    string->data[string->real_size++] = symbol;
    string->data[string->real_size] = '\0';
}

unsigned int string_count(const string_t * string, const uchar symbol, int start, int end){
    if (start < 0 || end < 0){
        error("string_count", "start and end must be positive", ValueError);
    }
    if (end > string->real_size){
        error("string_count", "End index greater than max length of string", IndexError);
    }
    unsigned int count = 0;
    size_t i = 0;
    for (i = start; i < end; ++i){
        if (string->data[i] == symbol){
            count++;
        }
    }
    return count;
}

int string_find(const string_t * string, const uchar symbol, int start, int end){
    if (start < 0 || end < 0){
        error("string_find", "start and end must be positive", ValueError);
    }
    if (end > string->real_size){
        error("string_find", "End index greater than max length of string", IndexError);
    }
    size_t i = 0;
    for (i = start; i < end; ++i){
        if (string->data[i] == symbol){
            return i;
        }
    }
    return -1;
}

int string_rfind(const string_t * string, const uchar symbol, int start, int end){
    if (start < 0 || end < 0){
        error("string_find", "start and end must be positive", ValueError);
    }
    if (end > string->real_size){
        error("string_find", "End index greater than max length of string", IndexError);
    }
    size_t i = 0;
    for (i = end - 1; i >= start; --i){
        if (string->data[i] == symbol){
            return i;
        }
    }
    return -1;
}

void string_set(string_t * string, uchar symbol, size_t size){ 
    if (size > string->length){
        error("string_set", "Size cant be greater than length of string", IndexError);
    }
    size_t i = 0;
    for (i = 0; i < size; ++i){
        string->data[i] = symbol;
    }
    if (size > string->real_size){
        string->real_size = size;
        string->data[string->real_size] = '\0';
    }
}

int string_swartswith(const string_t * string, const uchar * pattern){
    size_t pattern_size = str_len(pattern);
    if (string->real_size < pattern_size){
        return 0;
    }
    size_t i = 0;
    for (i = 0; i < pattern_size; ++i){
        if (pattern[i] != string->data[i]){
            return 0;
        }
    }
    return 1;
}

int string_endswith(const string_t * string, const uchar * pattern){
    size_t pattern_size = str_len(pattern);
    if (string->real_size < pattern_size){
        return 0;
    }
    size_t i = 0;
    for (i = 0; i < string->real_size && i != pattern_size; ++i){
        if (pattern[pattern_size - i - 1] != string->data[string->real_size - i - 1]){
            return 0;
        }        
    }
    return 1;
}

int is_upper(const uchar symbol){
    if (symbol >= 'A' && symbol <= 'Z'){
        return ENG;
    }
    if (symbol >= 'А' && symbol <= 'Я'){
        return RUS;
    }
    return 0;
}

int is_lower(const uchar symbol){
    if (symbol >= 'a' && symbol <= 'z'){
        return ENG;
    }
    if (symbol >= 'а' && symbol <= 'я'){
        return RUS;
    }
    return 0;
}

int is_digit(const uchar symbol){
    return (symbol >= '0' && symbol <= '9');
}

int is_alpha(const uchar symbol){
    return (is_upper(symbol) || is_lower(symbol));
}

int is_alnum(const uchar symbol){
    return (is_upper(symbol) || is_lower(symbol) || is_digit(symbol));
}

int is_space(const uchar symbol){
    return (symbol == ' ' || symbol == '\t' || symbol == '\v'
            || symbol == '\f' || symbol == '\n' || symbol == '\r');
}

uchar to_upper(const uchar symbol){
    int type = is_lower(symbol);
    if (type == RUS){
        return symbol - 'а' + 'А'; // a, A == rus symbols
    }
    if (type == ENG){
        return symbol - 'a' + 'A'; //a == 97, A == 65
    }
    return symbol;
}

uchar to_lower(const uchar symbol){
    int type = is_upper(symbol);
    if (type == RUS){
        return symbol - 'А' + 'а'; // a, A == rus symbols
    }
    if (type == ENG){
        return symbol - 'A' + 'a'; //a == 97, A == 65
    }
    return symbol;
}

void string_upper(string_t * string){
    size_t i = 0;
    for (i = 0; i < string->real_size; ++i){
        string->data[i] = to_upper(string->data[i]);
    }
}

void string_lower(string_t * string){
    size_t i = 0;
    for (i = 0; i < string->real_size; ++i){
        string->data[i] = to_lower(string->data[i]);
    }
}

void string_swapcase(string_t * string){
    size_t i = 0;
    for (i = 0; i < string->real_size; ++i){
        if (is_lower(string->data[i])){
            string->data[i] = to_upper(string->data[i]);
        }
        else{
            string->data[i] = to_lower(string->data[i]);
        }
    }
}

void string_capitalize(string_t * string){
    string_lower(string);
    if (string->real_size > 0){
        string->data[0] = to_upper(string->data[0]);
    }
}

static int template(const string_t * string, int (check)(const uchar symbol)){
    size_t i = 0;
    for (i = 0; i < string->real_size; ++i){
        if ((*check)(string->data[i]) == 0){
            return 0;
        }
    }
    return 1;
}

int string_islower(const string_t * string){
    return template(string, is_lower);
}

int string_isupper(const string_t * string){
    return template(string, is_upper);
}

int string_isdigit(const string_t * string){
    return template(string, is_digit);
}

int string_isalpha(const string_t * string){
    return template(string, is_alpha);
}

int string_isalnum(const string_t * string){
   return template(string, is_alnum);
}

int string_isspace(const string_t * string){
    return template(string, is_space);
}

int string_compare(const string_t * str1, const string_t * str2){
    uchar c1 = 0;
    uchar c2 = 0;
    size_t i = 0;
    do {
        c1 = str1->data[i];
        c2 = str2->data[i++];
        if (c1 == '\0'){
            return c1 - c2;
        }
    } while (c1 == c2);
    return c1 - c2;
}

void string_empty(string_t * string){
    string->data[0] = '\0';
    string->real_size = 0;
}

size_t str_len(const uchar * str){
    size_t length = 0;     
    while (str[length]){
        length++;
    }
    return length;
}

static void error(const uchar * func_name, const uchar * err_msg, int err_code){
    fprintf(stderr, func_name);
    fprintf(stderr, ": ");
    fprintf(stderr, err_msg);
    exit(err_code);
}

void swap(void * a, void * b, size_t size){
    char temp = 0;
    size_t i = 0;
    for (i = 0; i < size; ++i){
        temp = *((char *) a + i);
        *((char *) a + i) = *((char *) b + i);
        *((char *) b + i) = temp;
    }
}

int main(){
    string_t * hel = string_dup("fellojjir");
    printf("%d\n", string_islower(hel));

    return 0;
}