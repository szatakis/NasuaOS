# Building NasuaOS

This guide explains how to build NasuaOS from source.

## Requirements

Install the following tools before building:

- Git
- GCC
- G++
- CMake
- Xorriso
- Clang
- LLD
- NASM
- QEMU *(optional, recommended for testing)*

## Clone the Repository

```bash
git clone https://github.com/szatakis/NasuaOS.git
cd NasuaOS
```

## Build

Compile the operating system:

```bash
make all
```

## Build and Run

If QEMU is installed, you can build and launch NasuaOS with:

```bash
make run
```

# Platform Setup

## Ubuntu / Debian

Install the required packages:

```bash
sudo apt update

sudo apt install \
    git \
    gcc \
    g++ \
    cmake \
    xorriso \
    clang \
    lld \
    nasm \
    qemu-system-x86
```

Clone the repository:

```bash
git clone https://github.com/szatakis/NasuaOS.git
cd NasuaOS
```

Build the project:

```bash
make all
```

Run it:

```bash
make run
```

## Windows (WSL / WSL2)

### 1. Install WSL

Install WSL with either:

- Ubuntu
- Debian

### 2. Install Dependencies

Inside your Linux distribution, follow the **Ubuntu / Debian** instructions above.

### 3. (Optional) Install QEMU on Windows

If you want to run NasuaOS directly from `make run`, install QEMU for Windows and ensure it is available in your system's `PATH`.

Otherwise, you can simply build the ISO:

```bash
make all
```

---

# Build Targets

| Command | Description |
|---------|-------------|
| `make all` | Build NasuaOS |
| `make run` | Build and run in QEMU |
| `make clean` | Remove build artifacts |

# Troubleshooting

### `make: command not found`

Install Make:

```bash
sudo apt install make
```

### `nasm: command not found`

```bash
sudo apt install nasm
```

### `clang: command not found`

```bash
sudo apt install clang lld
```

### `xorriso: command not found`

```bash
sudo apt install xorriso
```