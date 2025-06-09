# Trabajo Práctico N° 2 – Kernel

**Grupo 20 – Alfieri, Di Candia, Díaz Varela**

---

## Descripción

Este proyecto consiste en la implementación de un sistema operativo básico con funcionalidades de manejo de procesos, memoria, sincronización y comunicación entre procesos. El sistema soporta:

- Planificación de procesos.
- Semáforos para sincronización.
- Dos estrategias de manejo de memoria: Buddy System y Heap Enlazado.
- Pipes para comunicación entre procesos.
- Shell interactiva con comandos implementados.

El objetivo es comprender e implementar conceptos de bajo nivel en el diseño de sistemas operativos.

---

## Compilación y Ejecución

### Requisitos

- Linux
- QEMU
- `make`
- Permisos de sudo para algunas ejecuciones

### Comandos básicos

Desde la raíz del proyecto:

```bash
cd Toolchain
make all
cd ..
./run.sh
