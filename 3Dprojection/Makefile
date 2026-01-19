CC := clang
obj := 2dto3d
CFLAGS := -I./ -L./ -lraylib -lm -Wl,-rpath,./

all: $(obj)

$(obj): main.c 
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean all

clean:
	rm -rf $(obj)
