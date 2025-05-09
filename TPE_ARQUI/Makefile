MM ?= -DBUDDY

all: bootloader kernel userland image

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all MM=$(MM)

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

buddy:
	$(MAKE) MM="-DBUDDY" all

our:
	$(MAKE) MM="-DOUR" all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean
	find . -type f -name "*.Zone.Identifier" -exec rm -f {} +
	rm -f Shared/*.o

.PHONY: bootloader image collections kernel userland all clean buddy our
