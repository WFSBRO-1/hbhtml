#pragma once
#include <stdbool.h>
#include <stddef.h>

#define BROWSER_MAX       512
#define BROWSER_PATH_MAX  512
#define BROWSER_VISIBLE   18

typedef struct {
    char name[256];
    bool isDir;
} BrowserEntry;

typedef struct {
    char path[BROWSER_PATH_MAX];
    BrowserEntry entries[BROWSER_MAX];
    int count;
    int cursor;
    int scroll;
} Browser;

void browser_open(Browser *b, const char *path);
void browser_move(Browser *b, int delta);
bool browser_up(Browser *b);
bool browser_enter(Browser *b, char *out, size_t outSize);
