#!/bin/bash
# build_docs.sh
# Regenera la documentación HTML con Doxygen + Graphviz
# Uso: ./build_docs.sh   (desde bash/WSL/Git Bash)

set -e
cd "$(dirname "$0")"

# Asegurar Graphviz en PATH sin importar la sesión actual
GV="/c/Program Files/Graphviz/bin"
if [ -d "$GV" ]; then
    case ":$PATH:" in
        *":$GV:"*) : ;;
        *) export PATH="$GV:$PATH" ;;
    esac
fi

if ! command -v dot >/dev/null 2>&1; then
    echo "ERROR: Graphviz (dot) no encontrado en PATH"
    echo "Instalalo con:  winget install Graphviz.Graphviz"
    exit 1
fi
if ! command -v doxygen >/dev/null 2>&1; then
    echo "ERROR: doxygen no encontrado en PATH"
    exit 1
fi

rm -rf docs/html
doxygen Doxyfile
echo
echo "Documentacion regenerada: docs/html/index.html"
echo "Landing con diagramas:   docs/index.html"
