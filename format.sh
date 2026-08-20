#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")"

VENV_DIR=".venv"

if [ ! -d "$VENV_DIR" ]; then
	python3 -m venv "$VENV_DIR"
fi

source "$VENV_DIR/bin/activate"

pip install --quiet astyle_py==1.0.5

astyle_py --astyle-version=3.4.7 --rules=.astyle-rules.yml \
	src/*.c include/*.h examples/*/main/*.c
