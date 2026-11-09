#pragma once
#include <3ds.h>

typedef struct {
    u32         hid;
    const char *code;
    int         keyCode;
} KeyBinding;

void input_init(void);
void input_dispatch(u32 down, u32 up);
