PR_NAME := sensors_log

#
SRC    := ./src
INC    := ./include
LIBS   := ./libs
DRIVER := ./driver

LIBRARY_PATH += "$(shell pwd)/libs/zmod4xxx"

BUILD := ./build
OBJ   := $(BUILD)/obj

SRCS_DRIVER := $(shell find $(DRIVER) -name "*.c" -o -name "*.S")
SRCS_LIBS   := $(shell find $(LIBS) -name "*.c" -o -name "*.S")
SRCS_USER   := $(shell find $(SRC) -name "*.c" -o -name "*.S")

OBJS_DRIVER := $(patsubst $(DRIVER)/%.c, $(OBJ)/driver/%.o, $(SRCS_DRIVER))
OBJS_LIBS   := $(patsubst $(LIBS)/%.c, $(OBJ)/libs/%.o, $(SRCS_LIBS))
OBJS_USER   := $(patsubst $(SRC)/%.c, $(OBJ)/src/%.o, $(SRCS_USER))

SRCS := $(SRCS_DRIVER) $(SRCS_LIBS) $(SRCS_USER)
OBJS := $(OBJS_DRIVER) $(OBJS_LIBS) $(OBJS_USER)

#
CC = xtensa-lx106-elf-gcc
CFLAGS =  -I$(SRC) -I$(INC) -I$(LIBS) -I$(DRIVER) -DICACHE_FLASH -mlongcalls -std=gnu11 -Wall
CFLAGS += -Og -ggdb
# CFLAGS += -Os -g

LDFLAGS = -Teagle.app.v6.ld
LDLIBS  = -nostdlib -Wl,--start-group
LDLIBS += -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -lgcc -lm -lssl
LDLIBS += -L"$(shell pwd)/libs/zmod4xxx" -l_iaq_2nd_gen
LDLIBS += -Wl,--end-group

#
all: build

info: build
	xtensa-lx106-elf-size $(BUILD)/$(PR_NAME) -A

build: $(BUILD)/$(PR_NAME)-0x00000.bin

$(BUILD)/$(PR_NAME)-0x00000.bin: $(BUILD)/$(PR_NAME)
	esptool.py elf2image $^

$(BUILD)/$(PR_NAME)-0x10000.bin: $(BUILD)/$(PR_NAME)-0x00000.bin

$(BUILD)/$(PR_NAME): $(OBJS)
	$(CC) $^ $(LDLIBS) $(LDFLAGS) -o $@

$(OBJ)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJS): $(SRCS)

flash: $(BUILD)/$(PR_NAME)-0x00000.bin $(BUILD)/$(PR_NAME)-0x10000.bin
	esptool.py -a soft_reset write_flash 0 $(word 1,$^) 0x10000 $(word 2,$^)

clean:
	rm -fr ./build/

.PHONY: flash clean all build info
