#include "canvas2d.h"
#include <3ds.h>
#include <string.h>

/* Пиксели канваса: 0xRRGGBB. Экран 400x240, поэтому канвас 1:1. */
static u32 pix[CANVAS_W * CANVAS_H];

static duk_ret_t n_fill_rect(duk_context *c)
{
    int x = duk_get_int(c, 0);
    int y = duk_get_int(c, 1);
    int w = duk_get_int(c, 2);
    int h = duk_get_int(c, 3);
    duk_double_t d = duk_get_number(c, 4);
    u32 col = (d >= 0 && d <= 16777215.0) ? (u32)d : 0;

    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > CANVAS_W) w = CANVAS_W - x;
    if (y + h > CANVAS_H) h = CANVAS_H - y;
    if (w <= 0 || h <= 0)
        return 0;

    for (int j = y; j < y + h; j++)
        for (int i = x; i < x + w; i++)
            pix[j * CANVAS_W + i] = col;
    return 0;
}

void canvas_register(duk_context *ctx)
{
    duk_push_c_function(ctx, n_fill_rect, 5);
    duk_put_global_string(ctx, "__hbFillRect");
}

void canvas_reset(void)
{
    memset(pix, 0, sizeof pix);
}

/* Копирует канвас на верхний экран. Формат BGR8, экран хранится повёрнутым. */
void canvas_present(void)
{
    u8 *fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
    for (int y = 0; y < CANVAS_H; y++) {
        for (int x = 0; x < CANVAS_W; x++) {
            u32 c = pix[y * CANVAS_W + x];
            u8 *p = fb + (x * 240 + (239 - y)) * 3;
            p[0] = c & 0xFF;
            p[1] = (c >> 8) & 0xFF;
            p[2] = (c >> 16) & 0xFF;
        }
    }
}
