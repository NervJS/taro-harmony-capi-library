#!/bin/sh

script_dir=$(dirname "$0")
exports_dir="${script_dir}/../../../exports"
cpp_files=(${exports_dir}/*.cpp)

dts_file="${script_dir}/../index.d.ts"
keep_so_obfuscation_rules="${script_dir}/../obfuscation-rules.txt"
echo "// Type definitions for exported functions\n" > "$dts_file"
echo "# Keep so APIs name obfuscation rules.\n\n-keep-property-name" > "$keep_so_obfuscation_rules"
for file in "${cpp_files[@]}"; do
    cat "$file" | grep -E '^\s*\{\s*("[^"]*")\s*,\s*nullptr\s*,\s*' | grep -o '"[^"]*"' | sed 's/"//g' | while read -r line; do
        echo "export const $line: (...args: any[]) => any | Promise<any>" >> "$dts_file"
        echo "$line" >> "$keep_so_obfuscation_rules"
    done
done
