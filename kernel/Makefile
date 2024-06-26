# stuff
objout = Image
iso_objout = myos.img
iso_dir = ./isodir
scripts_dir = ./scripts
c_flags = -I include -m32 -g -ffreestanding -fstack-protector
asm_flags = -f elf32 -g
ld_flags = -T link.ld -m elf_i386 -z noexecstack

# compilers / assemblers
export cc = gcc
export as = nasm
export linker = ld

# source files
c_src := $(shell find . -type f -name "*.c")
asm_src := $(shell find . -type f -name "*.S")
# object files
c_obj := $(patsubst %.c, %.o, $(c_src))
asm_obj := $(patsubst %.S, %.o, $(asm_src))

all: $(shell python $(scripts_dir)/gen-config.py) $(objout) 

$(objout): $(asm_obj) $(c_obj)
	@echo " LD   $(objout)"
	@$(linker) $(ld_flags) -o $(objout) $^
	@strip $(objout) --strip-unneeded
	@echo "$(objout) is ready"

%.o: %.S
	@echo " AS   $<"
	@$(as) $(asm_flags) $< -o $@

%.o: %.c
	@echo " CC   $<"
	@$(cc) -c $< -o $@ $(c_flags)

clean:
	@rm -f $(c_obj) $(asm_obj) $(iso_objout) $(objout)

iso:
	@cp $(objout) $(iso_dir)/boot/
	@grub-mkrescue isodir -o $(iso_objout) -d /usr/lib/grub/i386-pc
	@qemu-img convert -f raw -O vpc $(iso_objout) disk.vhd

debug:
	@qemu-system-x86_64 \
		-d int \
		-hda $(iso_objout) \
		-m 2G \
		-vga std

qemu:
	@qemu-system-x86_64 \
		-cpu host \
		-enable-kvm \
		-hda $(iso_objout) \
		-m 2G \
		-serial stdio \
		-vga std
