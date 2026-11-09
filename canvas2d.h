#pragma once
#include "duktape.h"

#define CANVAS_W 400
#define CANVAS_H 240

void canvas_register(duk_context *ctx);
void canvas_reset(void);
void canvas_present(void);
