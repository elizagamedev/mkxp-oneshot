#!/bin/bash
if [ $# -lt 1 ]; then
  echo "usage: pixmapper.sh <image>"
  exit 1
fi
basename="$(basename "$1" | tr '[:upper:]' '[:lower:]')"
name="${basename%.*}"
cfile="$name.c"
convert "$1" -alpha on -transparent '#00ff00' -alpha extract -negate xbm:- | sed "/^#define/d;s/-_bits/${name}_mask/g;s/^static char/unsigned char/" > "$cfile"
echo "unsigned char ${name}_rgb[] = {" >> "$cfile"
convert "$1" -alpha on -transparent '#00ff00' -depth 8 bgra:- | xxd -i >> "$cfile"
echo "};" >> "$cfile"
