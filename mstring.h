#ifndef __MSTRING_H__
    #define __MSTRING_H__

#include <stdio.h>

typedef unsigned char uchar;

enum{
    RUS = 1,
    ENG = 2
};

enum{
    ValueError = 1,
    IndexError,
};

typedef struct string{
    uchar *data;
    size_t length;
    size_t real_size; 
}string_t;

typedef struct string_array{
    string_t ** data;
    size_t size;
    size_t max_size;
}string_array_t;


static string_t * string_create(size_t length);
void string_free(string_t * string);
string_t * string_dup(const uchar * str);
void print_string(string_t * str, FILE * file);
void println_string(string_t * str, FILE * file);
string_t * string_concat(const string_t * left, const string_t * right);
void string_reverse(string_t * string);
void string_lstrip(string_t * string);
void string_rstrip(string_t * string);
void string_strip(string_t * string);
void string_remove_last(string_t * string, const size_t amount);
void string_append(string_t * string, const uchar symbol);
void string_copy(string_t * dest, const string_t * src);
unsigned int string_count(const string_t * string, const uchar symbol, int start, int end);
int string_find(const string_t * string, const uchar symbol, int start, int end);
int string_rfind(const string_t * string, const uchar symbol, int start, int end);
int string_swartswith(const string_t * string, const uchar * pattern);
int string_endswith(const string_t * string, const uchar * pattern);
void string_array_free(string_array_t * string_array);
string_array_t * string_split(const string_t * string, const uchar symbol);
string_t * string_join(const string_array_t * str_arr, const uchar * pat);
string_t * string_replace(const string_t * string, const uchar * old, const uchar * new);
string_t * string_center(const string_t * string, const uchar symbol);
void string_empty(string_t * string);
void string_set(string_t * string, uchar symbol, size_t size);
int is_upper(const uchar symbol);
int is_lower(const uchar symbol);
int is_digit(const uchar symbol);
int is_space(const uchar symbol);
int is_alpha(const uchar symbol);
int is_alnum(const uchar symbol);
uchar to_upper(const uchar symbol);
uchar to_lower(const uchar symbol);
void string_upper(string_t * string);
void string_lower(string_t * string);
void string_swapcase(string_t * string);
void string_capitalize(string_t * string);
static int template(const string_t * string, int (check)(const uchar symbol));
int string_islower(const string_t * string);
int string_isupper(const string_t * string);
int string_isdigit(const string_t * string);
int string_isspace(const string_t * string);
int string_isalpha(const string_t * string);
int string_isalnum(const string_t * string);
int string_compare(const string_t * str1, const string_t * str2);
size_t str_len(const uchar * str);
void swap(void * a, void * b, size_t size);
static void error(const uchar * func_name, const uchar * err_msg, int err_code);

#endif
