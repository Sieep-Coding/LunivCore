# LunivCore: C/Lua Business Intelligence Engine from [Luniv Consulting](https://luniv.tech/)

[!WARNING]
> Work in Progress. Not ready for production use.
---

## LunivCore
The goal of `LunivCore` is to enable **"BI-as-Code"**, allowing analysts to define complex reports without requiring re-compilation of the core engine.

---

## LunivCore Features
* **High Performance:** Core data structure handling and rendering implemented in C.
* **Clean DSL:** Define datasets, add data points, and plot results using simple, object-oriented Lua syntax (`dataset:add()`, `dataset:plot()`).
* **Extensible:** Easily add new C-backed methods (e.g., `dataset:filter()`, `dataset:calculate_average()`) to the Lua object system via Userdata and Metatables.
* **Portable:** Built using standard C (C99) and the embeddable Lua library.
* **Support:** On-going, lifetime support from the passionate dev team at [Luniv Consulting](https://luniv.tech/)

---

## Getting Started

### Prerequisites (CachyOS / Arch Linux)

Since this project is built on CachyOS, you will need the standard development toolchain and Lua libraries:

```bash
sudo pacman -S base-devel lua
```

