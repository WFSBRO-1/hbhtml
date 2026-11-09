#include "config.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define DEFAULT_FOLDER "sdmc:/"

void config_load(Config *c)
{
    snprintf(c->folder, sizeof c->folder, "%s", DEFAULT_FOLDER);

    FILE *f = fopen(CONFIG_FILE, "r");
    if (!f)
        return;

    char line[CONFIG_PATH_MAX + 16];
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strncmp(line, "folder=", 7) == 0)
            snprintf(c->folder, sizeof c->folder, "%s", line + 7);
    }
    fclose(f);
}

void config_save(const Config *c)
{
    mkdir("sdmc:/3ds", 0777);
    mkdir(CONFIG_DIR, 0777);

    FILE *f = fopen(CONFIG_FILE, "w");
    if (!f)
        return;
    fprintf(f, "folder=%s\n", c->folder);
    fclose(f);
}
