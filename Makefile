include $(DEVKITARM)/3ds_rules

TARGET   := hbhtml
BUILD    := build
SOURCES  := .
INCLUDES := .
ARCH     := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

LIBDIRS  := $(LIBCTRU)
INCLUDE  := $(foreach dir,$(LIBDIRS),-I$(dir)/include)
LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

CFLAGS   := -g -Wall -O2 -mword-relocations -ffunction-sections -D__3DS__ \
            $(ARCH) $(INCLUDE) $(addprefix -I,$(INCLUDES))
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS  := -specs=3dsx.specs -g $(ARCH) $(LIBPATHS)
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
