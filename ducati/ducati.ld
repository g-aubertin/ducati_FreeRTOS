/* linker script based on the readme from
   the gcc-arm-none-eabi-4_6-2012q2 toolchain */

MEMORY
{

    TEXT (rwx) : ORIGIN = 0x00000000, LENGTH = 256K
    DATA (rwx) : ORIGIN = 0x80000000, LENGTH = 1M
    TRACE (rwx) : ORIGIN = 0x9F000000, LENGTH = 8000K
}

SECTIONS
{
    .text :
    {
	KEEP(*(.vectors))
	*(.text)
	*(.rodata*)
    } > TEXT

    .data :
    {
	*(vtable)
	*(.data*)
    } > DATA

    .bss :
    {
	_bss = .;
	*(.bss*)
	*(COMMON)
	_ebss = .;
    } > DATA

    .resource_table :
    {
	_resource_table = .;
    } > DATA

    .tracebuf :
    {
	_tracebuf = .;
    } > TRACE

}
