#include "browser.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

#define ROOT "sdmc:/"

static bool is_html(const char *name)
{
    size_t n = strlen(name);
    return n > 5 && strcasecmp(name + n - 5, ".html") == 0;
}

static void join(char *out, size_t n, const char *dir, const char *name)
{
    size_t len = strlen(dir);
    if (len && dir[len - 1] == '/')
        snprintf(out, n, "%s%s", dir, name);
    else
        snprintf(out, n, "%s/%s", dir, name);
}

/* Сначала папки, потом файлы, внутри каждой группы по алфавиту. */
static int cmp_entries(const void *a, const void *b)
{
    const BrowserEntry *x = a;
    const BrowserEntry *y = b;
    if (x->isDir != y->isDir)
        return y->isDir - x->isDir;
    return strcasecmp(x->name, y->name);
}

static void fix_visible(Browser *b)
{
    if (b->cursor < b->scroll)
        b->scroll = b->cursor;
    if (b->cursor >= b->scroll + BROWSER_VISIBLE)
        b->scroll = b->cursor - BROWSER_VISIBLE + 1;
}

void browser_open(Browser *b, const char *path)
{
    snprintf(b->path, sizeof b->path, "%s", path);
    b->count = 0;
    b->cursor = 0;
    b->scroll = 0;

    DIR *d = opendir(b->path);
    if (!d)
        return;

    struct dirent *ent;
    while (b->count < BROWSER_MAX && (ent = readdir(d)) != NULL) {
        if (ent->d_name[0] == '.')
            continue;

        char full[BROWSER_PATH_MAX + 256];
        join(full, sizeof full, b->path, ent->d_name);

        struct stat st;
        if (stat(full, &st) != 0)
            continue;

        bool isDir = S_ISDIR(st.st_mode);
        if (!isDir && !is_html(ent->d_name))
            continue;

        BrowserEntry *e = &b->entries[b->count++];
        snprintf(e->name, sizeof e->name, "%s", ent->d_name);
        e->isDir = isDir;
    }
    closedir(d);

    qsort(b->entries, b->count, sizeof(BrowserEntry), cmp_entries);
}

void browser_move(Browser *b, int delta)
{
    if (b->count == 0)
        return;
    b->cursor += delta;
    if (b->cursor < 0)
        b->cursor = 0;
    if (b->cursor >= b->count)
        b->cursor = b->count - 1;
    fix_visible(b);
}

/* Поднимается на уровень выше. Корень sdmc:/ не покидаем. */
bool browser_up(Browser *b)
{
    if (strcmp(b->path, ROOT) == 0)
        return false;

    char parent[BROWSER_PATH_MAX];
    snprintf(parent, sizeof parent, "%s", b->path);

    size_t len = strlen(parent);
    while (len > 1 && parent[len - 1] == '/')
        parent[--len] = '\0';

    char *slash = strrchr(parent, '/');
    if (!slash)
        return false;

    if (slash > parent && slash[-1] == ':')
        slash[1] = '\0';   /* sdmc:/3ds -> sdmc:/ */
    else
        *slash = '\0';     /* sdmc:/3ds/hbhtml -> sdmc:/3ds */

    browser_open(b, parent);
    return true;
}

/* Возвращает true, если выбран файл. Папка открывается и возвращает false. */
bool browser_enter(Browser *b, char *out, size_t outSize)
{
    if (b->count == 0)
        return false;

    BrowserEntry *e = &b->entries[b->cursor];
    char full[BROWSER_PATH_MAX + 256];
    join(full, sizeof full, b->path, e->name);

    if (e->isDir) {
        browser_open(b, full);
        return false;
    }

    snprintf(out, outSize, "%s", full);
    return true;
}
