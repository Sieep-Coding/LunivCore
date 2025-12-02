<div id="top" align="center">
    
![](assets/lunivcore-logo.png)

##### Business Intelligence as Code: Generate reports using `lcore`, a SQL and markdown inspired language.
#### [EXAMPLES](https://github.com/Sieep-Coding/LunivCore/tree/main/examples) | [WEBSITE](https://lunivcore.vercel.app/)
##### Or use with [Lua](https://www.lua.org/)
##### Get started by reading the [docs.](https://lunivcore.vercel.app/)

</div>

## Why Now?
The goal of `LunivCore` is to enable **"BI-as-Code"**, allowing analysts to define complex reports without requiring re-compilation of the core engine.

> I built `LunivCore` after getting tired of the constraints of traditional enterprise BI platforms. Instead of relying on a Markdown-driven system like [evidence](https://github.com/evidence-dev/evidence/), I wanted a lower-level foundation. Something lean, predictable, and fully under my control. Using C and Lua gives me that flexibility.
>
> This gives me the freedom to layer features on top such as Markdown rendering, web output, and anything else in deliberate manner.
>
>  Nick, LunivCore creator

# See It In Action

LunivCore is an open-source, engineer focused alternative to PowerBI, Tableau, and other business intelligence tool.

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

Or run it on the Lua VM through C functions:

```lua
local dataset = BI.new("Q1 Aggregation KPIs")

dataset:add("North", 340)
dataset:add("South", 210)

print("Total:", dataset:sum())
print("Average:", dataset:avg())
print("Min:", dataset:min())   
print("Max:", dataset:max())
print("Count:", dataset:count())

print("Max - Min:", dataset:max() - dataset:min())

dataset:plot()
```

```text
./lcore examples/lua/aggregation.lua
Total: 550
Average: 275.0
Min: 210
Max: 340
Count: 2
Max - Min: 130

--- Q1 Aggregation KPIs ---
North      | ██████████████████████████████████ (340)
South      | █████████████████████ (210)
```

LunivCore generates a terminal-dashboard or website from markdown-inspired `.lcore` or `Lua` files:

- **Low-level** framework engineered for integration anywhere that supports C, including microcontrollers, websites, servers, and more.
- **SQL-like statements** inside markdown-inspired files to run queries against your data sources
- **Charts and components** are rendered using these query results
- **Templated pages** generate many pages from a single markdown template
- **Loops** and **If / Else** statements allow control of what is displayed to users
- **Coded for Performance**: LunivCore is made with **C**, enabling high-performance.

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
│   │   │   ├── lbind.c
│   │   │   └── lbind.h
|   |   |   
│   │   └── lcore_exec.c
│   │   └── lcore_exec.h                    
│   │   └── main.c              
│   └── lua/           
│       │
│       ├── config.lua          
│       └── dsl/
│           ├── dataset.lua
│           └── utils.lua
└── examples/
    └── helloworld.lcore
    └── quarterly_review.lcore
    ├── lua/ 
        ├── helloworld.lua 

```



# License

LunivCore is licensed under the MIT license. See the [LICENSE](LICENSE.md) file for licensing information.
