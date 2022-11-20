.SILENT:

.PHONY: all
all: CSRBfsTweaks.so

CSRBfsTweaks.so: CSRBfsTweaks.c
	echo Compiling $@

	gcc -W -Wall -fPIC \
		-Wl,-init,CSRBfsTweaksInit -Wl,-z,initfirst \
		-shared \
		$< \
		-o $@ \
		-ldl
# NOTE: -ldl has to be at the end

.PHONY: clean
clean:
	echo Cleaning
	rm -f CSRBfsTweaks.so core.*

rebuild: clean all

