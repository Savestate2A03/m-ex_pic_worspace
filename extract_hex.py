# converts objdump for Melee Code Manager

# Sorry, no comments for this one. Just a lot of regex

import sys
import re

def find_sequence(main_array, sequence):
    indices = []
    len_main = len(main_array)
    len_seq = len(sequence)

    if len_seq == 0:
        return list(range(len_main + 1))

    for i in range(len_main - len_seq + 1):
        if main_array[i : i + len_seq] == sequence:
            return i
    return -1

def main():
    if len(sys.argv) < 3:
        print('extract_hex.py inject.asm inject_hex.asm')
        return

    inject = {}
    inject_hex = {}

    with open(sys.argv[1], "r") as inject_file:
        inject = inject_file.read()

    with open(sys.argv[2], "r") as inject_hex_file:
        inject_hex = inject_hex_file.read()

    hex_rgx = r"(?<!\n )[0-9a-fA-F]{8}"
    hex_rgx_matches = re.findall(hex_rgx, inject_hex)
    hex_rgx_matches_orig = hex_rgx_matches

    data_sequence_start = find_sequence(hex_rgx_matches, ['a1c23cc4', '21abfaad', 'deadbeef', 'fafafafb'])
    hex_rgx_matches = hex_rgx_matches[data_sequence_start:]

    fn_names_rgx = r"8[0-9a-fA-F]{7}\s+<(\S+)>:"
    fn_names_rgx_replacement = r"\1:"
    inject = re.sub(fn_names_rgx, fn_names_rgx_replacement, inject)

    mid_fn_jumps_rgx = r"(8[0-9a-fA-F]{7})\s+<(\S+)\+(\S+)>"
    mid_fn_jumps_rgx_replacement = r"\2_\3"
    mid_fn_jumps_matches = re.findall(mid_fn_jumps_rgx, inject)

    inject = re.sub(mid_fn_jumps_rgx, mid_fn_jumps_rgx_replacement, inject)

    for match in mid_fn_jumps_matches:
        inject = inject.replace(f'{match[0]}:', f'{match[1]}_{match[2]}:\n{match[0]}:')

    fn_jumps_rgx = r"8[0-9a-fA-F]{7}\s+<(\S+)>"
    fn_jumps_rgx_replacement = r"\1"
    inject = re.sub(fn_jumps_rgx, fn_jumps_rgx_replacement, inject)

    inject = inject.replace(',', ', ')

    register_addressing = r"( [\-0-9]+)(\([r0-9]+\))"
    register_addressing_matches = re.findall(register_addressing, inject)
    for match in register_addressing_matches:
        inject = inject.replace(f'{match[0]}{match[1]}', f' {hex(int(match[0]))}{match[1]}')

    pic_bcl_resolution = r"4\*cr7\+so"
    pic_bcl_resolution_replacement = r"31"
    inject = re.sub(pic_bcl_resolution, pic_bcl_resolution_replacement, inject)

    inject = inject.splitlines()
    checks_passed = -1

    line_chec_rgx = r"([0-9a-fA-F]{8}:\s+([0-9a-fA-F]{2})\s+([0-9a-fA-F]{2})\s+([0-9a-fA-F]{2})\s+([0-9a-fA-F]{2})\s+)(.*)"

    for line in range(len(inject)):
        try:
            if line < len(inject) - 4:
                matches = re.findall(line_chec_rgx, inject[line])[0]
                if ((matches[1]+matches[2]+matches[3]+matches[4]).upper() == 'A1C23CC4'):
                    matches = re.findall(line_chec_rgx, inject[line+1])[0]
                    if ((matches[1]+matches[2]+matches[3]+matches[4]).upper() == '21ABFAAD'):
                        matches = re.findall(line_chec_rgx, inject[line+2])[0]
                        if ((matches[1]+matches[2]+matches[3]+matches[4]).upper() == 'DEADBEEF'):
                            matches = re.findall(line_chec_rgx, inject[line+3])[0]
                            if ((matches[1]+matches[2]+matches[3]+matches[4]).upper() == 'FAFAFAFB'):
                                inject = inject[:line]
                                break
        except:
            pass

    inject.append('')
    inject.append('pic_data:')

    instructions_rgx = r"[0-9a-fA-F]{8}:\s+[0-9a-fA-F]{2}\s+[0-9a-fA-F]{2}\s+[0-9a-fA-F]{2}\s+[0-9a-fA-F]{2}\s+(.+)"
    long_expand_rgx = r"\.long 0x([0-9a-fA-F]+)"

    for line in range(len(inject)):
        try:
            instructions_rgx_matches = re.findall(instructions_rgx, inject[line])
            inject[line] = instructions_rgx_matches[0]
        except:
            pass
        try:
            long_expand_rgx_matches = re.findall(long_expand_rgx, inject[line])
            inject[line] = f'.long 0x{f"{int(long_expand_rgx_matches[0], 16):08x}"}'
        except:
            pass
        
    for hex_match in hex_rgx_matches:
        inject.append(f'.long 0x{hex_match}')

    inject = inject[inject.index('_start:'):]
    inject = "\n".join(inject)

    print("\n --- ASM ---")
    print(inject)
    print("\n --- BIN ---")

    second = False
    for word in hex_rgx_matches_orig:
        print(word, end='\n' if second else ' ')
        second = not second

if __name__ == '__main__':
    main()