include $(DEVKITARM)/3ds_rules

TARGET   := hbhtml
BUILD    := build
SOURCES  := .
INCLUDES := .
ARCH     := -march=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS   := -g -Wall -O2 -mword-relocations -ffunction-sections $(ARCH) \
            $(addprefix -I,$(INCLUDES))
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS  := -specs=3dsx.specs -g $(ARCH)
LIBS     := -lctru -lm

CFILES   := $(foreach d,$(SOURCES),$(wildcard $(d)/*.c))
OBJS     := $(addprefix $(BUILD)/,$(notdir $(CFILES:.c=.o)))
VPATH    := $(SOURCES)

all: $(BUILD) $(TARGET).3dsx

$(BUILD):
	@mkdir -p $@

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

$(TARGET).3dsx: $(TARGET).elf
	3dsxtool $< $@

clean:
	rm -rf $(BUILD) $(TARGET).elf $(TARGET).3dsx

.PHONY: all clean
