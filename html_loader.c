#include "html_loader.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

char *html_read_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f)
        return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n <= 0) {
        fclose(f);
        return NULL;
    }
    char *buf = malloc((size_t)n + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    size_t got = fread(buf, 1, (size_t)n, f);
    fclose(f);
    buf[got] = '\0';
    return buf;
}

static const char *ci_find(const char *hay, const char *needle)
{
    size_t nl = strlen(needle);
    for (; *hay; hay++) {
        if (strncasecmp(hay, needle, nl) == 0)
            return hay;
    }
    return NULL;
}

static void append(char **buf, size_t *len, size_t *cap, const char *s, size_t n)
{
    if (*len + n + 2 > *cap) {
        size_t nc = (*len + n + 2) * 2;
        char *nb = realloc(*buf, nc);
        if (!nb)
            return;
        *buf = nb;
        *cap = nc;
    }
    memcpy(*buf + *len, s, n);
    *len += n;
    (*buf)[*len] = '\0';
}

char *html_extract_scripts(const char *html)
{
    size_t cap = 1024, len = 0;
    char *out = malloc(cap);
    if (!out)
        return NULL;
    out[0] = '\0';

    const char *p = html;
    while ((p = ci_find(p, "<script")) != NULL) {
        const char *open = strchr(p, '>');
        if (!open)
            break;
        const char *close = ci_find(open, "</script>");
        if (!close)
            break;

        size_t tagLen = (size_t)(open - p);
        char tag[256];
        size_t copy = tagLen < sizeof tag - 1 ? tagLen : sizeof tag - 1;
        memcpy(tag, p, copy);
        tag[copy] = '\0';

        bool external = ci_find(tag, "src=") != NULL;
        bool special = ci_find(tag, "module") != NULL || ci_find(tag, "importmap") != NULL;
        if (!external && !special)
            append(&out, &len, &cap, open + 1, (size_t)(close - open - 1));

        p = close + 9;
    }
    return out;
}
