#pragma once
#include <stddef.h>

/* Читает весь файл в память. Вызывающий освобождает free(). */
char *html_read_file(const char *path);

/* Собирает текст всех inline <script> (без src= и без module/importmap). */
char *html_extract_scripts(const char *html);
