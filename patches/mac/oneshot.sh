#!/bin/bash
[[ $# -eq 1 && "$1" == -psn_* ]] && shift
cd "${0%/*}/../Resources"
DATA="$(cd "../../.." && echo "$PWD")"
export DYLD_FALLBACK_LIBRARY_PATH="$PWD:$DYLD_FALLBACK_LIBRARY_PATH"
exec ./steamshim --gameFolder="$DATA" "$@"
