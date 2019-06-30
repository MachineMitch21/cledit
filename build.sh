obj_dir="obj"
out_dir="dist"
prog_name="cledit"
src_dir="src"

mkdir -p "$obj_dir" "$out_dir"

shopt -s nullglob # make sure no *.* globs with nothing get processed

# for asm_file in "$src_dir"/*.asm; do
#   nasm -isrc/ -g -f elf32 -o "$obj_dir"/$(basename "$asm_file").o "$asm_file"
# done

for c_file in "$src_dir"/*.c; do
  gcc -fno-stack-protector -g -c -Wall -Wextra -pedantic -std=c99 -o "$obj_dir"/$(basename "$c_file").o "$c_file"
done

gcc -o "$out_dir"/"$prog_name" "$obj_dir"/*.o
# ld -m elf_i386 -shared-libgcc -dynamic-linker /lib32/ld-linux.so.2 /lib32/libc.so.6 -lc -o "$out_dir"/"$prog_name" "$obj_dir"/*.o