#####################################
########## define of macro ##########
#####################################

# OS name is GAFF
OSNAME=GAFF

# dir pass
ASM=./asm
C=./c
LS=./ls
OBJ=./obj
FONTS=./fonts
TOOLS=./tools

# important filename
IPL=$(OBJ)/ipl.bin
SYS=$(OBJ)/$(OSNAME).sys
MAP=$(OSNAME).map
IMG=$(OSNAME).img

# include pass
INCPAS=./header

# gcc option
GCC-OP=-m32 -nostdlib -fno-builtin -fno-pic -I$(INCPAS)

# all object filename
ASMOBJ=$(OBJ)/asmfunc.o  $(OBJ)/string.o $(OBJ)/srch_regs.o
COBJ=$(OBJ)/main.o $(OBJ)/graphic.o  $(OBJ)/mouse.o $(OBJ)/sprintf.o $(OBJ)/hankaku.o $(OBJ)/descriptor.o $(OBJ)/int.o $(OBJ)/fifo.o $(OBJ)/kbc.o $(OBJ)/memtest.o $(OBJ)/memman.o $(OBJ)/debug.o
ALLOBJ=$(COBJ) $(ASMOBJ)



########################################
########## how to create file ##########
########################################

# ipl.s
$(IPL): $(ASM)/ipl.s
	@gcc -nostdlib -T$(LS)/ipl.ls $? -o $@

# head.s
$(OBJ)/head.bin: $(ASM)/head.s
	@gcc -nostdlib -T$(LS)/head.ls $? -o $@

# fonts
$(C)/hankaku.c: $(FONTS)/hankaku.txt
	@$(TOOLS)/fontmaker/fm $? $@

# suffix about .c
$(OBJ)/%.o: $(C)/%.c
	@gcc $(GCC-OP) $< -c -o $@

# suffix about .s
$(OBJ)/%.o: $(ASM)/%.s
	@as --32 $< -o $@

# all obj
$(OBJ)/allcobj.bin: $(ALLOBJ)
	@ld -m elf_i386 -T$(LS)/os.ls -Map=GAFF.map $? -o $@

# head.s + all obj
$(SYS): $(OBJ)/head.bin $(OBJ)/allcobj.bin
	@cat $? > $@

# img
$(IMG): $(SYS) $(IPL)
	@dd if=$(IPL) of=$@
	@dd seek=1 if=$(SYS) of=$@
#	@mformat -f 1440 -C -B $(IPL) -i $@ ::
#	@mcopy -i $@ $(SYS) ::

# only make img
img: $(IMG)



################################
########## operations ##########
################################

# write GAFF.img to /dev/sdb
burn: $(IMG)
	@sudo dd count=1088 if=/dev/zero of=/dev/sdb						# USBの先頭セクタから、1088=0x440(iplでusbからメモリにロードするデータサイズ÷512)分のデータを削除
	@sudo dd if=$? of=/dev/sdb								# USBにイメージファイルを書き込む

# emulate on qemu
# I explain primary command. (In detail http://www.ne.jp/asahi/it/life/it/kvm/qemu/qemu_tips.html)
# -boot         : we can use 'a', 'c', 'd' after '-boot' option. boot from FDD when we use 'a'. boot from HDD when we use 'c'. (this is default) boot from CDROM (or DVD) when we use 'd'.
# -L            : 
# -m            : describe the amount of memory use in qemu. (unit is MB and default is 128MB)
# -localtime    : use localtime of host OS as that of guest OS.
# -enable-audio : enable audio device on virtual pc.
# -use          : usb support.
# -full-screen  : start in full screen.
# -vga          : 
# -drive        : 
# -fda          : boot from FDD.
# -hda          : boot from HDD.
# -cdrom        : boot from CDROM. (or DVD)
run: $(IMG)
	@qemu-system-i386     -L . -m 100 -localtime -vga std -drive file=$?,format=raw		# 386環境でエミュレート(なぜか警告が出ない)
	@#@qemu-system-i386   -L . -m 100 -localtime -vga std -fda $?				# FDDから起動
	@#@qemu-system-i386   -L . -m 100 -localtime -vga std -hda $?				# HDDから起動
	@#@qemu-system-i386   -L . -m 100 -localtime -vga std -cdrom $?				# CDROMから起動
	@#@qemu-system-x86_64 -L . -m 100 -localtime -vga std -drive file=$?,format=raw		# x64環境でエミュレートしている？

# show mapfile
map: $(MAP)
	@cat $(MAP) | less

# debug OS
# I explain primary command. (In detail http://www.ne.jp/asahi/it/life/it/kvm/qemu/qemu_tips.html)
# it seems usable 'layout asm', 'layout src', 'layout regs', but object files have no infomation about source code, so we can't use 'layout src'
# if we use -g option to gcc, we can add some infomation about source code to object file. and debugger(gdb) can use them.
# but this OS isn't compatible with -g option. so we can't use it. sorry.
debug: $(IMG)
	@qemu-system-i386 -S -s -m 32 -localtime -drive file=$?,format=raw &
	@gdb -q \
	-ex 'target remote localhost:1234' \
	-ex 'set architecture i8086' \
	-ex 'set tdesc filename target.xml' \
	-ex 'break *0x7c00' \
	-ex 'continue'

# qemu monitor
# I explain primary command. (In detail http://www.ne.jp/asahi/it/life/it/kvm/qemu/qemu_tips.html)
# if we want to use 'console' which start qemu window as qemu monitor, we should attach '-monitor stdio' option and specify std I/O to qemu monitor.
# we can use qemu monitor without '-monitor stdio' by pushing 'ctrl + alt + 2' on qemu guest OS window.
# 'ctrl + alt + 1' : guest OS, 'ctrl + alt + 2' : qemu monitor, 'ctrl + alt + 3' : serial console, 'ctrl + alt + 4' : parallel console, 'ctrl + alt + f' : full screen
monitor: $(IMG)
	@qemu-system-i386 -L . -m 32 -localtime -monitor stdio -vga std -drive file=$?,format=raw

# delete all binary file
clean:
	@rm -f $(OBJ)/*
	@rm -f $(MAP)
	@rm -f $(IMG)
