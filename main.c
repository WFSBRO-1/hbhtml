#include <3ds.h>
#include <stdio.h>
#include "input_map.h"

int main(int argc, char **argv)
{
    (void)argc; (void)argv;

    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);
    input_init();

    printf("hbhtml: skeleton up\n");

    while (aptMainLoop()) {
        hidScanInput();
        input_dispatch(hidKeysDown(), hidKeysUp());

        if (hidKeysDown() & KEY_START)
            break;

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
