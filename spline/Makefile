cc := clang
target := spline
package_path := /home/hanlulu/code/spline
raylib := raylib
mathlib := m

spline: main.c da_utils.h
	$(cc) -g  $< -I$(package_path) -L$(package_path) -l$(raylib) -l$(mathlib) -o $(target) -Wl,-rpath,$(package_path)

.PHONY: clean run

run: $(target)
	./$(target)

clean:
	rm -rf $(target)
