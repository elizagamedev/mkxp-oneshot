#!/bin/bash -eu

if [ $# -lt 2 ]; then
  echo "usage: make-journal-linux.sh SOURCE_PATH OUTPUT_DIR"
  exit 1
fi

source_path="$1"
output_dir="$2"
venv="$(mktemp -d)"
workpath="$(mktemp -d)"

function atexit {
  [ -n "$venv" ] && rm -rf "$venv"
  [ -n "$workpath" ] && rm -rf "$workpath"
}

trap atexit EXIT

# Create and initialize venv
python3 -m venv "$venv"
set +u
source "$venv/bin/activate"
set -u
pip install --upgrade pip
pip install pyqt5 pyinstaller

# Create standalone python distribution
pyinstaller --onefile \
	    --distpath "$output_dir" \
	    --workpath "$workpath" \
	    --specpath="$source_path/journal/unix" \
	    "$source_path/journal/unix/journal-linux.spec"
