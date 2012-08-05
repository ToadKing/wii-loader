CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc

CFLAGS = -Os -Iinclude -I$(DEVKITPRO)/libogc/include -DGEKKO -DHW_RVL -mrvl -mcpu=750 -meabi -mhard-float -Wall
LDFLAGS = -DGEKKO -DHW_RVL -mrvl -mcpu=750 -meabi -mhard-float -Wl,-Map,$(notdir $@).map,--section-start,.init=0x81200000 -L$(DEVKITPRO)/libogc/lib/wii -lfat -logc

OBJ = source/dol.o source/main.o

DOL_TARGET = loader.dol
ELF_TARGET = loader.elf

all: $(DOL_TARGET)

%.dol: %.elf
	$(DEVKITPPC)/bin/elf2dol $< $@

$(ELF_TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(DOL_TARGET)
	rm -f $(ELF_TARGET)
	rm -f $(OBJ)
