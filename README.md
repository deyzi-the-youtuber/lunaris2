<h1 align=center>Lunaris 2</h1>
<p align=center>Remade light of the Lunaris kernel</p>

**What is Lunaris?**\
Lunaris is a UNIX-like kernel that is written completely in C and assembly.\
Lunaris takes inspiration from Linux, and the Fiwix kernel.

Lunaris might just be the new Linux 👀 (if it weren't so unstable)

## Compiling the Lunaris kernel
To compile the Lunaris kernel, you need to install the following:
- Unix-like system / environment
- Binutils
- NASM
- GCC
- grub-mkrescue (needed for creating ISO image)

After installing, run 'make', and the kernel image will be built.\
To get kernel symbols working, go into any file of the C source files, make a small change, then revert the change, and recompile. \
This should allow kernel symbols to be used in the kernel.

## How to run Lunaris
To run Lunaris, run 'make iso' to update the ISO image with the compiled kernel. After, run 'make qemu'.

## Licensing
Lunaris is licensed under the GNU General Public License version 2. For more information, see LICENSE.
