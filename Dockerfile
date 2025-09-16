FROM ubuntu:24.04

RUN apt update && apt install -y build-essential gcc-riscv64-linux-gnu qemu-system-riscv64 gdb-multiarch
