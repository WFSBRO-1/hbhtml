#include "js_runtime.h"
#include "canvas2d.h"
#include "js_prelude.h"
#include <stdio.h>
#include <string.h>

static duk_context *ctx = NULL;
static char initErr[900] = "";

const char *js_init_error(void)
{
    return initErr;
}

static char *g_out = NULL;
static size_t g_outSize = 0;
static size_t g_outLen = 0;

static void out_append(const char *s)
{
    if (!g_out)
        return;
    size_t n = strlen(s);
    if (g_outLen + n >= g_outSize - 1)
        n = g_outSize - 1 - g_outLen;
    memcpy(g_out + g_outLen, s, n);
    g_outLen += n;
    g_out[g_outLen] = '\0';
}

static duk_ret_t native_print(duk_context *c)
{
    duk_idx_t n = duk_get_top(c);
    for (duk_idx_t i = 0; i < n; i++) {
        if (i > 0)
            out_append(" ");
        out_append(duk_safe_to_string(c, i));
    }
    out_append("\n");
    return 0;
}

void js_shutdown(void)
{
    if (ctx) {
        duk_destroy_heap(ctx);
        ctx = NULL;
    }
}


/* Прогоняет маленькие команды по одной, чтобы понять, какая ломается. */
static void run_selftest(char *dst, size_t n)
{
    static const char *snips[] = {
        "1+1",
        "var a0 = 0;",
        "var a1 = {};",
        "var a2 = [];",
        "var a3 = [1, 2];",
        "var a4 = []; a4.push(1);",
        "var a5 = new Array();",
        "var a6 = 'x'.charAt(0);",
        "var a7 = parseInt('16', 16);",
        "var a8 = Math.floor(2.5);",
    };
    size_t len = strlen(dst);
    for (unsigned i = 0; i < sizeof snips / sizeof snips[0]; i++) {
        char o[120];
        bool ok = js_run(snips[i], o, sizeof o);
        char line[120];
        for (char *p = o; *p; p++)
            if (*p == '\n') *p = ' ';
        snprintf(line, sizeof line, "%u %s %.30s\n", i, ok ? "ok" : "ERR", o);
        size_t l = strlen(line);
        if (len + l + 1 >= n)
            break;
        memcpy(dst + len, line, l + 1);
        len += l;
    }
}

bool js_init(void)
{
    initErr[0] = '\0';
    ctx = duk_create_heap_default();
    if (!ctx) {
        snprintf(initErr, sizeof initErr, "duk_create_heap_default failed");
        return false;
    }

    duk_push_c_function(ctx, native_print, DUK_VARARGS);
    duk_put_global_string(ctx, "print");
    canvas_register(ctx);

    char out[256];
    if (!js_run(PRELUDE_JS, out, sizeof out)) {
        snprintf(initErr, sizeof initErr, "prelude: %s\n--- selftest ---\n", out);
        run_selftest(initErr, sizeof initErr);
        js_shutdown();
        return false;
    }
    return true;
}

bool js_reset(void)
{
    js_shutdown();
    return js_init();
}

bool js_run(const char *src, char *out, size_t outSize)
{
    if (!ctx || outSize == 0) {
        if (outSize)
            out[0] = '\0';
        return false;
    }

    g_out = out;
    g_outSize = outSize;
    g_outLen = 0;
    out[0] = '\0';

    bool ok = (duk_peval_string(ctx, src) == 0);
    if (!ok) {
        out_append("Error: ");
        out_append(duk_safe_to_string(ctx, -1));
        out_append("\n");
    }
    duk_pop(ctx);

    g_out = NULL;
    return ok;
}

static bool call_global_num(const char *name, double nowMs, char *err, size_t errSize)
{
    if (!ctx) {
        snprintf(err, errSize, "JS not running");
        return false;
    }
    duk_get_global_string(ctx, name);
    duk_push_number(ctx, nowMs);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        snprintf(err, errSize, "Error: %s", duk_safe_to_string(ctx, -1));
        duk_pop(ctx);
        return false;
    }
    duk_pop(ctx);
    return true;
}

bool js_start(double nowMs, char *err, size_t errSize)
{
    return call_global_num("__hbStart", nowMs, err, errSize);
}

bool js_frame(double nowMs, char *err, size_t errSize)
{
    return call_global_num("__hbFrame", nowMs, err, errSize);
}

void js_dispatch_key(const char *type, const char *code, int keyCode)
{
    if (!ctx)
        return;
    duk_get_global_string(ctx, "__hbKey");
    duk_push_string(ctx, type);
    duk_push_string(ctx, code);
    duk_push_int(ctx, keyCode);
    if (duk_pcall(ctx, 3) != DUK_EXEC_SUCCESS) {
        /* ошибки в обработчике клавиш пока молча игнорируем */
    }
    duk_pop(ctx);
}
