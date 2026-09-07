#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" >/dev/null && pwd)"

nohup "$SCRIPT_DIR/moon/moon" "main.lua"