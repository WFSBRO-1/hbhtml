#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "browser.h"
#include "canvas2d.h"
#include "config.h"
#include "html_loader.h"
#include "input_map.h"
#include "js_runtime.h"

#define MSG_MAX 1024

static PrintConsole topCon;
static PrintConsole botCon;
static Browser br;

static void draw(const char *status)
{
    consoleSelect(&topCon);
    consoleClear();
    printf("hbhtml  %s\n\n", br.path);
    if (br.count == 0)
        printf("  (no .html files or folders here)\n");
    for (int i = br.scroll; i < br.count && i < br.scroll + BROWSER_VISIBLE; i++) {
        printf("%c %s%s\n",
               i == br.cursor ? '>' : ' ',
               br.entries[i].name,
               br.entries[i].isDir ? "/" : "");
    }

    consoleSelect(&botCon);
    consoleClear();
    printf("UP/DOWN move   A open   B up\n");
    printf("X test JS      START exit\n\n");
    if (status && status[0])
        printf("%s", status);
}

static bool start_game(const char *path, char *msg, size_t n)
{
    char *html = html_read_file(path);
    if (!html) {
        snprintf(msg, n, "Cannot read:\n%s", path);
        return false;
    }
    char *scripts = html_extract_scripts(html);
    free(html);
    if (!scripts) {
        snprintf(msg, n, "Out of memory");
        return false;
    }

    if (!js_reset()) {
        free(scripts);
        snprintf(msg, n, "JS init failed");
        return false;
    }
    canvas_reset();

    char err[JS_OUT_MAX];
    bool ok = js_run(scripts, err, sizeof err);
    free(scripts);
    if (!ok) {
        snprintf(msg, n, "%s", err);
        return false;
    }
    if (!js_start((double)osGetTime(), err, sizeof err)) {
        snprintf(msg, n, "%s", err);
        return false;
    }
    return true;
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    gfxInitDefault();
    consoleInit(GFX_TOP, &topCon);
    consoleInit(GFX_BOTTOM, &botCon);

    bool jsOk = js_init();

    Config cfg;
    config_load(&cfg);
    browser_open(&br, cfg.folder);

    char status[MSG_MAX] = "";
    bool inGame = false;
    if (!jsOk)
        snprintf(status, sizeof status, "JS init failed:\n%s\n", js_init_error());
    draw(status);

    while (aptMainLoop()) {
        hidScanInput();
        u32 down = hidKeysDown();
        u32 up = hidKeysUp();

        if (inGame) {
            if (down & KEY_START) {
                inGame = false;
                js_reset();
                status[0] = '\0';
                draw(status);
            } else {
                input_dispatch(down, up);
                char err[JS_OUT_MAX];
                if (js_frame((double)osGetTime(), err, sizeof err)) {
                    canvas_present();
                } else {
                    inGame = false;
                    js_reset();
                    snprintf(status, sizeof status, "%s", err);
                    draw(status);
                }
            }
        } else {
            if (down & KEY_START)
                break;

            bool dirty = false;
            if (down & KEY_DOWN) {
                browser_move(&br, 1);
                dirty = true;
            }
            if (down & KEY_UP) {
                browser_move(&br, -1);
                dirty = true;
            }
            if (down & KEY_B) {
                browser_up(&br);
                status[0] = '\0';
                dirty = true;
            }
            if (down & KEY_A) {
                char file[BROWSER_PATH_MAX + 256];
                if (browser_enter(&br, file, sizeof file)) {
                    if (start_game(file, status, sizeof status))
                        inGame = true;
                } else {
                    status[0] = '\0';
                }
                dirty = true;
            }
            if (down & KEY_X) {
                char out[JS_OUT_MAX];
                js_run("print('hello from JS', 1 + 2)", out, sizeof out);
                snprintf(status, sizeof status, "JS:\n%s", out);
                dirty = true;
            }

            if (dirty && !inGame)
                draw(status);
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    snprintf(cfg.folder, sizeof cfg.folder, "%s", br.path);
    config_save(&cfg);

    js_shutdown();
    gfxExit();
    return 0;
}
