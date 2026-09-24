# 42sh

A POSIX-inspired Unix shell written from scratch in C99.

42sh reads shell scripts from a string, a file or standard input, parses them into an AST and executes them: pipelines, redirections, control flow, functions, expansions and a set of builtins.

> Academic project developed at EPITA (C/Unix, 2nd-year engineering cycle).
> The official subject is internal to the school and is therefore **not** included in this repository.

---

## Table of contents

- [Features](#features)
- [Getting started](#getting-started)
- [Usage](#usage)
- [Architecture](#architecture)
- [Testing](#testing)
- [Project layout](#project-layout)
- [Constraints](#constraints)
- [Known limitations](#known-limitations)
- [Authors](#authors)

---

## Features

### Language

- [x] Simple commands and command lists (`;`, newlines)
- [x] Conditions: `if` / `elif` / `else`
- [x] Loops: `while`, `until`, `for`, with `break` and `continue`
- [x] `case` ... `esac`
- [x] Logical operators `&&`, `||` and negation `!`
- [x] Pipelines (`|`)
- [x] Redirections (`>`, `<`, `>>`, `>&`, `<&`, `>|`, `<>`)
- [x] Command blocks `{ ...; }` and subshells `( ... )`
- [x] Function definitions and calls, with arguments
- [x] Comments

### Quoting and expansions

- [x] Single quotes, double quotes and escape character
- [x] Variables: `$name`, `${name}`, assignments
- [x] Special parameters: `$?`, `$$`, `$#`, `$@`, `$*`, `$1`...`$n`, `$RANDOM`, `$UID`
- [x] `$PWD`, `$OLDPWD`, `$IFS`
- [x] Command substitution: `$(...)`
- [x] Field splitting
- [x] Aliases

### Builtins

`echo` (`-n`, `-e`, `-E`), `true`, `false`, `exit`, `cd` (including `cd -`), `export`, `unset`, `break`, `continue`, `.` (dot), `alias`, `unalias`

### Bonus

- [ ] Tilde expansion
- [ ] Pathname expansion (`*`, `?`, `[...]`)
- [ ] Arithmetic expansion `$(( ))`
- [ ] Here-documents (`<<`, `<<-`)
- [ ] Interactive prompt (`PS1`, `PS2`)
- [ ] Job control (`&`, `jobs`, `wait`)

<!-- Tick or remove the items above so that they match what your shell really supports. -->

---

## Getting started

### Prerequisites

- A C compiler (`gcc` or `clang`)
- GNU Autotools: `autoconf`, `automake`
- `make`

### Build

```sh
autoreconf --install
./configure
make
```

The binary is produced at `src/42sh`.

### Install

```sh
./configure --prefix=/path/to/install
make install
# -> /path/to/install/bin/42sh
```

### Clean

```sh
make clean
```

---

## Usage

```
42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]
```

42sh reads its input from one of three sources:

```sh
# 1. A string
./src/42sh -c 'echo "Hello from a string"'

# 2. A file
./src/42sh script.sh

# 3. Standard input
echo 'echo "Hello from stdin"' | ./src/42sh
./src/42sh < script.sh
```

| Option | Description |
| ------ | ----------- |
| `-c SCRIPT` | Interpret the argument as a shell script |
| `--pretty-print` | Print the parsed AST in a re-parsable form (debug) |

<!-- Remove --pretty-print if you did not implement it, or adapt the name / env variable. -->

### Exit status

| Code | Meaning |
| ---- | ------- |
| `0` | Success |
| `1` | Runtime error / failing command |
| `2` | Invalid command-line argument or grammar (syntax) error |

### Example

```sh
$ cat demo.sh
greet() {
    echo "Hello, $1!"
}

for name in Ada Linus Grace; do
    greet "$name"
done | tr a-z A-Z

if [ -z "$UNSET_VAR" ] || false; then
    echo "done"
fi

$ ./src/42sh demo.sh
HELLO, ADA!
HELLO, LINUS!
HELLO, GRACE!
done
```

---

## Architecture

Each input goes through a classic interpreter pipeline:

```
 input (-c / file / stdin)
          |
          v
   +-------------+     tokens     +-------------+      AST      +--------------+
   |    Lexer    | -------------> |   Parser    | ------------> |  Executor    |
   +-------------+                +-------------+               +--------------+
     quoting,                       recursive                     expansions,
     keywords,                      descent,                      redirections,
     aliases                        one rule per                  fork/exec,
                                    grammar rule                  builtins
```

- **Lexer**: turns the character stream into tokens (words, operators, keywords, newlines). It is context-aware (quotes, nested substitutions) and applies alias substitution at the token level.
- **Parser**: recursive descent parser following the POSIX shell grammar. It builds an AST with one node type per construct (command, list, pipeline, `if`, loops, functions...).
- **Executor**: walks the AST, performs the expansions, sets up redirections (saving and restoring file descriptors for builtins), then either runs a builtin in-process or `fork` + `execvp`s an external command.
- **Pretty-printer**: dumps the AST as re-parsable shell code, which made debugging the parser much easier.

<!-- Add here the design choices that are specific to your implementation (memory management of function bodies, error handling strategy, lexer/expansion sharing...). Recruiters like this part. -->

---

## Testing

The testsuite is driven by `make check` and is configured through environment variables:

```sh
# Regular mode: functional tests only
BIN_PATH="$(pwd)/src/42sh" OUTPUT_FILE="$(pwd)/out" make check

# Coverage mode: unit + functional tests
BIN_PATH="$(pwd)/src/42sh" COVERAGE="yes" make check
```

| Variable | Role |
| -------- | ---- |
| `BIN_PATH` | Absolute path of the shell under test |
| `OUTPUT_FILE` | File in which the percentage of passed tests is written |
| `COVERAGE` | Set to `yes` to also run unit tests with coverage |

Because the binary is a parameter, the same testsuite can be run against a reference shell to check that the tests themselves are correct:

```sh
BIN_PATH="$(which bash)" OUTPUT_FILE="$(pwd)/out" make check
```

Expected behaviours are compared with `bash --posix` (stdout, stderr and exit code).

---

## Project layout

```
.
├── configure.ac        # Autoconf configuration
├── Makefile.am         # Top-level Automake file
├── src/
│   ├── Makefile.am
│   └── ...             # lexer, parser, AST, execution, builtins, utils
└── tests/
    ├── Makefile.am
    └── ...             # test scripts, grouped by category
```

<!-- Replace the "..." lines by your real module tree. -->

---

## Constraints

The project follows strict rules from the course:

- Compiled with `-std=c99 -pedantic -Werror -Wall -Wextra -Wvla`
- No GNU extensions (`_GNU_SOURCE`)
- Some functions are forbidden: `glob`, `regexec`, `wordexp`, `popen`, `syscall`, `system`
- Builtins run inside the shell process (no `exec` for them)
- Every allocation must be freed, no crash on any input
- Built exclusively with GNU Autotools, with no external build dependency

---

## Known limitations

<!-- Be honest, it is a strength. Examples: -->

- Features not implemented (see the unchecked boxes above)
- Behaviours that intentionally differ from Bash or POSIX because the course specification took precedence

---

## Authors

- **Your Name**: [@TanguyDOPLER](https://github.com/TanguyDOPLER)
- Teammate(s): Dylan.de-araujo, Maxime.Durival

## Acknowledgments

- The EPITA Assistants team for the project and their guidance
- The [POSIX Shell Command Language](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html) specification
- [Crafting Interpreters](https://craftinginterpreters.com/contents.html) by Robert Nystrom
