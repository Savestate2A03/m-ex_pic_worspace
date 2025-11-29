#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <input-melee.link> <output-linker.ld>" >&2
    exit 1
fi

IN="$1"
OUT="$2"

# I have no idea what I'm doing, but this seems
# to be a working, albeit nuclear, approach.

{
    echo "/* SSBM symbols */"
    echo ""

    sed -E 's/0?x?([0-9A-Fa-f]+):([^$]+)$/\2 = 0x\1;/g' "$IN"

    echo ""
    cat <<'EOF'
SECTIONS
{
    . = 0x80000000;

    .text : {
        *(.text)
        *(.text.*)
        . = ALIGN(4);
        *(.got2)
        . = ALIGN(4);
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4);
        __data_start = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        __data_end = .;
        /* BSS goes at the end */
        __bss_start = .;
        *(.sbss)
        *(.sbss.*)
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
    }

    /DISCARD/ : {
        *(.eh_frame)
        *(.eh_frame_hdr)
        *(.comment)
        *(.note.*)
        *(.debug*)
        *(.fixup*)
        *(.gnu.attributes)
    }
}
EOF
} > "$OUT"
