# Prebyte

**Prebyte** is a modern, language-agnostic preprocessor and templating engine designed to integrate seamlessly into any build pipeline or tooling system. It combines the performance of C++ with the flexibility of Lua, enabling dynamic, scoped, and efficient preprocessing for any kind of file — source code, templates, configs, docs, and beyond.

> ⚡ Build the code before the code becomes bytes.

---

## ✨ Features

* 🔧 **Universal syntax** – works in `.cpp`, `.json`, `.md`, `.html`, `.conf`, and more
* 📜 **Directive-based processing** – includes, macros, conditionals, loops (planned)
* 🧠 **Scoped resolution** – avoid repeated computation with file-based caching
* 🚠 **Lua-powered logic** – plug in dynamic values and helper functions easily
* 🧹 **Language-agnostic** – usable in any file, even those without native macros
* ♻ **Composable templates** – define your own directives and behaviors

---

## 🗂 File Example

```txt
%%define PROJECT_NAME Prebyte

Project: %%PROJECT_NAME%%
Build date: %%__DATE__%%
UUID: %%__UUID__%%
```

\`\`

```lua
vars = {
  PROJECT_NAME = "Prebyte"
}

funcs = {
  __DATE__ = function()
    return os.date("%Y-%m-%d")
  end,

  __UUID__ = function()
    return string.format("uuid-%x", os.time())
  end
}
```

**Resulting Output**

```txt
Project: Prebyte
Build date: 2025-05-13
UUID: uuid-66513f3f
```

---

## 📆 Project Structure

Prebyte follows a hybrid architecture:

* **C++ Core**

  * Handles file processing, directive parsing, scoping, caching, and rendering
* **Lua DSL**

  * Provides dynamic data, custom functions, and user-defined logic
* **CLI Tool**

  * Drives preprocessing with flags, overrides, and configuration loading

---

## 🧠 Variable & Function Conventions

| Syntax         | Meaning                                |
| -------------- | -------------------------------------- |
| `%%VAR_NAME%%` | Static variable from `vars` table      |
| `%%__NAME__%%` | Evaluated function from `funcs` table  |
| `%%func()%%`   | (Planned) Explicit call with arguments |

* `__ALL_CAPS__` is used to clearly indicate **dynamic, computed values**
* Function results are **cached per scope** (e.g. once per file)

---

## 🧹 Supported Directives

| Directive            | Description                      |
| -------------------- | -------------------------------- |
| `%%define KEY VALUE` | Define a static variable         |
| `%%if CONDITION`     | Start a conditional block        |
| `%%else` / `%%endif` | Conditional branching            |
| `%%include "file"`   | Include contents of another file |
| `%%for VAR in LIST`  | (Planned) Iterate over values    |

---

## 🔧 Lua Config: `build.lua`

Your Lua config powers dynamic behavior.

```lua
vars = {
  AUTHOR = "You",
  VERSION = "1.0.0"
}

funcs = {
  __DATE__ = function()
    return os.date("%Y-%m-%d")
  end,

  __BUILD_ID__ = function()
    return os.time()
  end
}
```

Prebyte looks up `%%VAR%%` in `vars`, and `%%__VAR__%%` in `funcs`.

---

## 🧠 Scoping & Performance

Prebyte uses **scoped evaluation** to avoid repeated computation:

* `global` – Shared across all files
* `file` – Cached per file (e.g. `__UUID__`, `__FILE__`)
* `block` – For looped/temporary scopes (future feature)

Example: `__UUID__` is only computed **once per file**, even if used 20×.

---

## 💻 CLI Usage (planned)

```bash
prebyte input.txt --out output.txt

# With variable overrides
prebyte template.md --define VERSION=2.1.0

# Use a custom Lua config
prebyte src/ --config build.lua

# Trace all resolved tokens
prebyte docs/readme.txt --trace
```

---

## 🥪 Planned Features

* `%%for%%` and `%%repeat%%` blocks
* Filters (e.g. `%%upper(PROJECT_NAME)%%`)
* Macro sets (define grouped tokens)
* Watch mode & incremental builds
* Plugin system via Lua

---

## 🔐 License

MIT License. See `LICENSE`.

---

## 💬 Contributing

Prebyte is in early development — ideas, improvements, and PRs are welcome!

* [Open an issue](https://github.com/YOUR_USERNAME/prebyte/issues)
* [Submit a pull request](https://github.com/YOUR_USERNAME/prebyte/pulls)

---

Made with ❤️ and too many macros.
