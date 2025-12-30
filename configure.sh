#!/usr/bin/env bash

set -e

ARCH=$(uname -m)
echo "[*] Detected arch: $ARCH"

case "$ARCH" in
    x86_64)
        HDR="/usr/include/asm/unistd_64.h"
        INCLUDE_LINE="#include <asm/unistd_64.h>"
        ;;
    aarch64|arm64)
        HDR="/usr/include/asm/unistd.h"
        [ -f /usr/include/asm/unistd_64.h ] && HDR="/usr/include/asm/unistd_64.h"
        INCLUDE_LINE="#include <asm/unistd.h>"
        ;;
    arm*)
        HDR="/usr/include/asm/unistd.h"
        INCLUDE_LINE="#include <asm/unistd.h>"
        ;;
    *)
        echo "[!] Unsupported architecture: $ARCH"
        exit 1
        ;;
esac

if [ ! -f "$HDR" ]; then
    echo "[ERR] syscall header not found: $HDR"
    echo "Try: sudo apt install linux-libc-dev"
    exit 1
fi

OUT="src/syscall_table.c"
mkdir -p src

echo "[*] Generating $OUT using $HDR"

# Header begin
cat > "$OUT" <<EOF
// Auto-generated syscall table for $ARCH
$INCLUDE_LINE
#include <string.h>
#include <syscall_table.h>

syscall_map syscall_table[] = {
EOF

# Read define lines
grep -E '^#define[[:space:]]+__NR_[A-Za-z0-9_]+[[:space:]]+[0-9]+' "$HDR" |
while read -r _ name num; do
    base=${name#__NR_}
    echo "    {\"$base\", $name}," >> "$OUT"
done

# Append lookup functions
cat >> "$OUT" <<EOF
};

int syscall_lookup_nr(const char *name) {
    for (size_t i = 0; i < sizeof(syscall_table)/sizeof(syscall_table[0]); i++) {
        if (strcmp(syscall_table[i].name, name) == 0)
            return syscall_table[i].nr;
    }
    return -1; // not found
}

const char* syscall_lookup_name(int nr) {
    for (size_t i = 0; i < sizeof(syscall_table)/sizeof(syscall_table[0]); i++) {
        if (syscall_table[i].nr == nr)
            return syscall_table[i].name;
    }
    return NULL; // not found
}

EOF

COUNT=$(grep -c '{\"' "$OUT")
echo "[OK] Generated $OUT with $COUNT syscalls + lookup functions"
