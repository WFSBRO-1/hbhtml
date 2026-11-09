#pragma once

#define CONFIG_DIR       "sdmc:/3ds/hbhtml"
#define CONFIG_FILE      CONFIG_DIR "/config.ini"
#define CONFIG_PATH_MAX  512

typedef struct {
    char folder[CONFIG_PATH_MAX];
} Config;

void config_load(Config *c);
void config_save(const Config *c);
