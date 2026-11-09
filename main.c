#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include "browser.h"
#include "config.h"

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
    printf("START save & exit\n\n");
    if (status && status[0])
        printf("%s\n", status);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    gfxInitDefault();
    consoleInit(GFX_TOP, &topCon);
    consoleInit(GFX_BOTTOM, &botCon);

    Config cfg;
    config_load(&cfg);
    browser_open(&br, cfg.folder);

    char status[BROWSER_PATH_MAX + 64] = "";
    draw(status);

    while (aptMainLoop()) {
        hidScanInput();
        u32 down = hidKeysDown();
        bool dirty = false;

        if (down & KEY_START)
            break;

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
            if (browser_enter(&br, file, sizeof file))
                snprintf(status, sizeof status, "Selected:\n%s", file);
            else
                status[0] = '\0';
            dirty = true;
        }

        if (dirty)
            draw(status);

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    snprintf(cfg.folder, sizeof cfg.folder, "%s", br.path);
    config_save(&cfg);

    gfxExit();
    return 0;
}
