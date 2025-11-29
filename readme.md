# LunivCore: C/Lua Business Intelligence Engine from [Luniv Consulting](https://luniv.tech/)

Get started by reading the [docs.](https://lunivcore.vercel.app/)

> [!WARNING]
> Work in Progress. Not ready for production use.

# See It In Action

Instantly start answering the most important questions for your business.

This `.lcore` file:

```bash
# Sales Report Q1 (examples/helloworld.lcore)

dataset Sales "Q1 Regional Sales" {
    North: 340
    South: 120
    East: 210
    West: 450
}

plot Sales as bar

```

#### Outputs:

```text
--- Q1 Regional Sales ---
North      | ██████████████████████████████████ (340)
South      | ████████████ (120)
East       | █████████████████████ (210)
West       | █████████████████████████████████████████████ (450)
----------------
```

## LunivCore
The goal of `LunivCore` is to enable **"BI-as-Code"**, allowing analysts to define complex reports without requiring re-compilation of the core engine.


## LunivCore Features
* **High Performance:** Core data structure handling and rendering implemented in C.
* **Built-in Compiler:** View the powerful [lcore](https://github.com/Sieep-Coding/LunivCore/tree/main/src/C/lcore) compiler.
* **Clean DSL:** Define datasets, add data points, and plot results using simple, object-oriented Lua syntax (`dataset:add()`, `dataset:plot()`).
* **Extensible:** Easily add new C-backed methods (e.g., `dataset:filter()`, `dataset:calculate_average()`) to the Lua object system via Userdata and Metatables.
* **Portable:** Built using standard C (C99) and the embeddable Lua library.
* **Support:** On-going, lifetime support from the passionate dev team at [Luniv Consulting](https://luniv.tech/)


## Getting Started

### Prerequisites (CachyOS / Arch Linux)

Since this project is built on CachyOS, you will need the standard development toolchain and Lua libraries:

```bash
sudo pacman -S base-devel lua
```

### Current structure
```text
/lunivcore
│
├── Makefile
│
├── src/
│   │
│   ├── C/               
│   │   │
│   │   ├── core/             
│   │   │   ├── dataset.c
│   │   │   └── dataset.h
│   │   ├── lcore/           
│   │   │   ├── ast.c
│   │   │   ├── ast.h
│   │   │   ├── lexer.c
│   │   │   ├── lexer.h
│   │   │   ├── parser.c
│   │   │   └── parser.h          
│   │   │   ├── render.c
│   │   │   └── render.h
│   │   ├── lua_bindings/       
│   │   └── main.c              
│   └── lua/           
│       │
│       ├── config.lua          
│       └── dsl/
│           ├── dataset.lua
│           └── utils.lua
└── examples/
    └── helloworld.lcore

```

