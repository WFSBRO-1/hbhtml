#include "input_map.h"
#include <stdio.h>

static KeyBinding bindings[] = {
    { KEY_DUP,    "ArrowUp",    38 },
    { KEY_DDOWN,  "ArrowDown",  40 },
    { KEY_DLEFT,  "ArrowLeft",  37 },
    { KEY_DRIGHT, "ArrowRight", 39 },
    { KEY_A,      "KeyZ",       90 },
    { KEY_B,      "KeyX",       88 },
    { KEY_START,  "Enter",      13 },
    { KEY_SELECT, "ShiftRight", 16 },
    { KEY_L,      "KeyQ",       81 },
    { KEY_R,      "KeyE",       69 },
};
#define NB (sizeof(bindings) / sizeof(bindings[0]))

static void emit(const char *type, const char *code, int keyCode)
{
    printf("%s %s (%d)\n", type, code, keyCode);
}

void input_init(void) {}

void input_dispatch(u32 down, u32 up)
{
    for (size_t i = 0; i < NB; i++) {
        if (down & bindings[i].hid)
            emit("keydown", bindings[i].code, bindings[i].keyCode);
        if (up & bindings[i].hid)
            emit("keyup",   bindings[i].code, bindings[i].keyCode);
    }
}
