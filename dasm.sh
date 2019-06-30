src_dir="src"
out_dir="$src_dir"/dasm_files

mkdir -p "$out_dir"

for c_file in "$src_dir"/*.c; do
  gcc -O0 -m32 -S -masm=intel "$c_file" -o "$out_dir"/$(basename "$c_file").s
done