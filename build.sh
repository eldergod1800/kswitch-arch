#!/bin/bash
# Build script for KTodo

set -e

echo "Building KTodo..."

# Check if dependencies are installed
echo "Checking dependencies..."
DEPS=(qt5-base qt5-quickcontrols2 qt5-declarative kirigami2 kconfig ki18n kcoreaddons cmake extra-cmake-modules)
MISSING=()

for dep in "${DEPS[@]}"; do
    if ! pacman -Qi "$dep" &>/dev/null; then
        MISSING+=("$dep")
    fi
done

if [ ${#MISSING[@]} -gt 0 ]; then
    echo "Missing dependencies: ${MISSING[*]}"
    echo "Install with: sudo pacman -S ${MISSING[*]}"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure
echo "Configuring..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DKDE_INSTALL_USE_QT_SYS_PATHS=ON

# Build
echo "Building..."
cmake --build .

echo ""
echo "Build complete!"
echo "To install, run: sudo cmake --install build"
echo "Or create a package: makepkg -si -p PKGBUILD.ktodo"
