include $(DEVKITARM)/3ds_rules

TARGET   := hbhtml
BUILD    := build
SOURCES  := .
INCLUDES := .
ARCH     := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
ICON     := icon-1.png

LIBDIRS  := $(DEVKITPRO)/libctru
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

$(TARGET).smdh: $(ICON)
	smdhtool --create "HB HTML" "HTML runtime" "WFSBRO" $(ICON) $@

$(TARGET).3dsx: $(TARGET).elf $(TARGET).smdh
	3dsxtool $< $@ --smdh=$(TARGET).smdh

clean:
	rm -rf $(BUILD) $(TARGET).elf $(TARGET).3dsx $(TARGET).smdh

.PHONY: all clean
