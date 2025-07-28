# Prebyte – Smart Text Preprocessor

**Prebyte** is a modular and programmable template-engine for text and file generation – built with rules, macros, profiles, and a clean CLI.

Use it as a **CLI tool** or embed it as a **C++ library**.

---

## 🚀 Features

*  **Variable substitution** (`%%name%%`, `%%date%%`)
*  **Macros** with arguments
*  **Conditionals & loops** (`%%if`, `%%for`, etc.)
*  **Profiles** (group variables, rules, ignore lists)
*  **Rules** to control behavior
*  **Includes** (recursive & safe)
*  **CLI & C++ API** support

---

## 📦 Installation

<details>
<summary><strong>Build from source</strong></summary>

```bash
git clone https://github.com/Codarity/Prebyte.git
cd Prebyte

# Install cli tool
make

# Or for the Library
make lib
```

</details>

<details>
<summary><strong>Requirements</strong></summary>

- C++23 or higher
- Make
- spdlog
- `pugixml` – for XML parsing (`-lpugixml`)
- `yaml-cpp` – for YAML parsing (`-lyaml-cpp`)
- `fmt` – for fast string formatting (`-lfmt`)
- [`cpptoml`](https://github.com/skystrife/cpptoml) to parse TOML configuration files.
- `nlohmann/json` – for JSON parsing (`-ljson`)

</details>

---

## 🛠️ Command Line Usage

Prebyte can be used as a command-line tool to process text files with macros, variables, profiles, and rules.

### Basic Usage

```bash
prebyte <input-file>
```

Processes the given file and outputs the result to standard output.

```bash
prebyte <input-file> -o <output-file>
```

Processes the file and writes the output to the given file.

```bash
cat input.txt | prebyte
```

Reads from **standard input** and writes to **standard output** (useful for piping).

---

### 📙 Available Commands

| Command                  | Description                                   |
| ------------------------ | --------------------------------------------- |
| `<file>`                 | Process the specified input file              |
| `-o, --output <file>`    | Write output to the specified file            |
| `-h, --help`             | Show help message                             |
| `-v, --version`          | Show version information                      |
| `-e, --explain`          | Show general usage explanation                |
| `-lsr, --list-rules`     | List active rules in the current context      |
| `-lsv, --list-variables` | List defined variables                        |
| `-s, --settings <file>`  | Specify a settings file (YAML, JSON, or TOML) |

With the `-e` flag, Prebyte will output a detailed explanation of the given keywords, commands, and options.

```bash
prebyte -e ignore  # Show explanation for the ignore command
prebyte -e rule    # Show explanation for the rule command
prebyte -e ARGS    # Show explanation how the ARGS array works
prebyte -e macro   # Show explanation for macros
```

---

### 🛠️ Options

| Option                       | Description                                                           |
| ---------------------------- | --------------------------------------------------------------------- |
| `-r, --rule <key>=<value>`   | Define a rule by key-value pair                                       |
| `-i, --ignore <pattern>`     | Ignore variables or rules that match the given pattern                |
| `-P<name>`                   | Load profile with given name                                          |
| `-p, --profile <name>`       | Same as `-P` – load profile by name                                   |
| `-D<key>=<value>`            | Define a variable via command line                                    |
| `-d, --define <key>=<value>` | Alternative for defining variables                                    |
| `--<debug_level>`            | Set debug level (`off`, `error`, `warning`, `info`, `debug`, `trace`) |

---

### 📋 Available Rules

These rules can be passed via `--rule` or defined in profiles/settings.

| Rule                     | Description                                               |
| ------------------------ | --------------------------------------------------------- |
| `strict_variables`       | Require all variables to be explicitly defined            |
| `set_default_variables`  | Automatically assign defaults to undefined variables      |
| `trim_start`             | Trim leading whitespace from variable values              |
| `trim_end`               | Trim trailing whitespace from variable values             |
| `allow_env`              | Allow reading from environment variables                  |
| `allow_env_fallback`     | Use env vars as fallback for missing variables            |
| `debug_level`            | Set verbosity level (`ERROR`, `WARNING`, `INFO`, `DEBUG`) |
| `max_variable_length`    | Limit max length of variable values                       |
| `default_variable_value` | Provide default when variable is missing                  |
| `variable_prefix`        | Set prefix for variable names                             |
| `variable_suffix`        | Set suffix for variable names                             |
| `include_path`           | Directory path to resolve includes                        |
| `benchmark`              | Enable benchmarking (`NONE`, `TIME`, `ALL`)     |

## 💡 C++ API – Example

```cpp
#include <Prebyte.h>

int main() {
    Prebyte pre;
    pre.set_variable("user", "Ada");
    pre.set_profile("default");

    std::string input = "Welcome %%user%%!";
    std::string output = pre.process(input);

    std::cout << output << std::endl;
}
```

---

## 🧱 Profiles

Profiles can be defined in a settings file:

```yaml
profiles:
  default:
    variables:
      environment: production
    ignore:
      - debug_rule
    rules:
      strict_variables: false
      ignore_variables: true
```

Apply in code:

```cpp
pre.set_profile("default");
```
Apply via CLI:
```bash
prebyte input.txt -Pdefault
```
Apply inline:
```bash
%%#set profile default
```

---

## 🧰 Extensible by Design

* Define your own rules and profiles
* Create and execute macros
* Use file-based or string-based input/output
* Clean C++ API, ready for embedding

---

## 📄 License

This project is licensed under the MIT License – see the [LICENSE](./LICENSE) file for details.

---

## 🙋 FAQ

<details>
<summary>How does Prebyte process files internally?</summary>
It uses a central `Context` to hold all variables, rules, input/output, and logger configuration. Then it runs through the `Preprocessor`, which interprets macros, conditions, loops, and includes.
</details>

<details>
<summary>How do I use loops and conditions in my templates?</summary>
Use `%%for`, `%%if`, `%%macro`, and `%%include` directives, similar to templating systems like Mustache or Liquid.
</details>

---

## 🧪 Tests & Examples

Official tests and example templates are not yet part of this repository.

We plan to add them soon — feel free to contribute your own via pull request in the meantime!

---

## ✨ Contributing

Pull requests, bug reports, and ideas are always welcome. Open an issue or contact us directly.
