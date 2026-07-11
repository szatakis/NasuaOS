# NasuaOS Documentation

Welcome to the official documentation of **NasuaOS** — a hobby operating system built from scratch for x86_64 computers.

NasuaOS is an experimental operating system focused on learning, low-level programming, and exploring how modern computers work. The project includes its own kernel, memory management, drivers, and system components written mainly in C++.

---

## About NasuaOS

NasuaOS is designed to be a lightweight and modern operating system created without relying on existing operating system kernels.

The main goals of NasuaOS are:

- Learning operating system development
- Understanding hardware and computer architecture
- Creating custom kernel components
- Building a simple and efficient system from the ground up

---

## Features

Current and planned features include:

- x86_64 kernel
- Physical and virtual memory management
- Paging system
- PCI device detection
- Keyboard and mouse drivers
- Graphics subsystem
- File system support
- User applications

---

## Documentation

This documentation contains information about NasuaOS internals, development, and usage.

### Kernel

Learn about the core components of NasuaOS:

- Boot process
- Kernel architecture
- Memory management
- Interrupt handling
- Hardware abstraction

### Drivers

Documentation for hardware support:

- PCI
- Keyboard
- Display
- Storage ata

### Development

Guides for developers:

- Setting up the build environment
- Compiling NasuaOS
- Running in an emulator
- Debugging the kernel
- Contributing to the project

---

## Building NasuaOS

To build NasuaOS, you need:

- x86_64 cross compiler
- Make or CMake
- Limine bootloader tools
- QEMU (recommended for testing)

Example:

```bash
git clone https://github.com/NasuaOS/NasuaOS.git
cd NasuaOS
make