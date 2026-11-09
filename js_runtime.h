#pragma once
#include <stdbool.h>
#include <stddef.h>

#define JS_OUT_MAX 400

bool js_init(void);
bool js_reset(void);
const char *js_init_error(void);
void js_shutdown(void);

/* Выполняет скрипт. print() и текст ошибки пишутся в out. */
bool js_run(const char *src, char *out, size_t outSize);

/* Вызывает __hbStart / __hbFrame из префикса. */
bool js_start(double nowMs, char *err, size_t errSize);
bool js_frame(double nowMs, char *err, size_t errSize);

/* Передаёт нажатие в addEventListener игры. */
void js_dispatch_key(const char *type, const char *code, int keyCode);
