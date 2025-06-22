# CLance
CLance is a **simple programming language compiler** written in C. It translates `.lance` files into C code and compiles them using `gcc`.

## Features

- **Lexer**: Tokenizes `.lance` source files.
- **Parser**: Builds an abstract syntax tree (AST) from tokens.
- **Assembler**: Generates C code from the AST.
- **Automatic Build**: Instantly compiles generated C code with `gcc`.
- **Command-line Options**:
    - `-i, --input`: Input `.lance` file
    - `-o, --output`: Output C file
    - `-a, --accept`: Auto-accept file overwrites
    - `-f, --format`: Format `.lance` code
    - `-b, --build`: Build the project
    - `-r, --run`: Run the compiled program
    - `--verbose`: Verbose/debug mode

---

## Installation

```sh
git clone https://github.com/redystum/CLance.git
cd CLance
make
```

## Usage

```sh
./lance [options] -i <input_file.lance> -o <output_file_name.c>
```

### Example

**main.lance**
```lance
print("Hello World\n")
int a = input("Enter a number: ")
int b = 10;

if (a > b) {
    print("a is greater than b\n")
}

return 0;
```

**Compile and run:**
```sh
./lance -i main.lance -o my_program -r
```
- Generates `out/my_program.c`
- Compiles it with `gcc` automatically
- Runs the resulting executable `my_program`

---

## Project Structure

- `main.c` — Entry point
- `lexer.c`/`lexer.h` — Lexer
- `parser.c`/`parser.h` — Parser/AST
- `assembler.c`/`assembler.h` — C code generator
- `utils.c`/`utils.h` — Utilities (strings, logging, arrays)
- `args.c`/`args.h` — Command-line parsing (via `gengetopt`)


## Build Targets

- `make` — Build
- `make debugon` — Build with debug info (same as using --verbose)
- `make clean` — Clean artifacts
- `make run` — Build & run with defaults
- `make debug` — Build & run with valgrind
- `make build` — Build & run, build & run the output file

## License

Licensed under the [MIT License](LICENSE).


## Author

Rúben Alves (aka redystum)