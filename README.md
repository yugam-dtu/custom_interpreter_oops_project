# Custom Interpreter (Hi-Bye Lang) — OOPS Project 🚀

[![Language: C++11](https://img.shields.io/badge/Language-C%2B%2B11-00599C.svg?style=flat-square&logo=c%2B%2B)](https://en.wikipedia.org/wiki/C%2B%2B11)
[![Build: Passing](https://img.shields.io/badge/Build-Passing-brightgreen.svg?style=flat-square)](Makefile)
[![Architecture: Tree--Walk AST](https://img.shields.io/badge/Architecture-Tree--Walk%20AST-blueviolet.svg?style=flat-square)](#-architecture--pipeline)
[![Paradigm: OOP](https://img.shields.io/badge/Paradigm-Object--Oriented%20Design-orange.svg?style=flat-square)](#-object-oriented-programming-oops-foundations)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](LICENSE)

An end-to-end, tree-walk programming language interpreter engineered from scratch in modern **C++ (C++11)**. This project serves as an in-depth implementation of **Compiler & Interpreter Design Principles** grounded firmly in **Object-Oriented Programming (OOPS)** paradigms, with zero external dependencies (no Flex/Bison, no Boost).

The language, **Hi-Bye Lang**, offers an expressive, friendly Hinglish syntax designed to illustrate how high-level programmatic logic—from variable definitions and arithmetic evaluation to conditional branching and dynamic I/O—translates into runtime operations.

---

## 📑 Table of Contents

- [Motivation & Objectives](#-motivation--objectives)
- [Object-Oriented Programming (OOPS) Foundations](#-object-oriented-programming-oops-foundations)
- [Architecture & Pipeline](#-architecture--pipeline)
- [Language Specification & Grammar](#-language-specification--grammar)
- [Directory Structure](#-directory-structure)
- [Build & Execution](#-build--execution)
- [Code Showcase & Examples](#-code-showcase--examples)
- [Error Handling & Diagnostics](#-error-handling--diagnostics)
- [Roadmap](#-roadmap)
- [Author & License](#-author--license)

---

## 🎯 Motivation & Objectives

### What Have We Created?
A fully functioning, standalone domain-specific interpreter that ingests `.hb` source files, tokenizes them, constructs a strongly-typed Abstract Syntax Tree (AST), and executes the code via recursive tree evaluation against an execution environment (Symbol Table).

### What Are We Trying To Do?
1. **Bridge Theory and Engineering**: Demystify how programming languages work under the hood without relying on black-box parser generators.
2. **Apply Advanced OOP in C++**: Demonstrate real-world Object-Oriented patterns:
   - Dynamic Polymorphism & Virtual Function Tables
   - Abstract Data Types (ADT)
   - RAII (Resource Acquisition Is Initialization)
   - Exception hierarchies for fault isolation
3. **Showcase Clean Architecture**: Maintain strict separation of concerns between lexical analysis, syntactic verification, AST modeling, and runtime evaluation.

---

## 🧩 Object-Oriented Programming (OOPS) Foundations

This project is built around fundamental Object-Oriented paradigms:

### 1. Dynamic Polymorphism & Abstract Interfaces
- **`Expr`**: Pure abstract base class defining the contract for value evaluation:
  ```cpp
  struct Expr {
      virtual ~Expr() = default;
      virtual Value eval(Env& env) = 0;
  };
  ```
- **`Stmt`**: Pure abstract base class defining the contract for statement execution:
  ```cpp
  struct Stmt {
      virtual ~Stmt() = default;
      virtual void exec(Env& env) = 0;
  };
  ```
- Concrete classes (`Literal`, `Var`, `Binary`, `RememberStmt`, `SayStmt`, `InputStmt`, `IfStmt`) inherit from these bases and override the virtual methods to provide specialized runtime behavior.

### 2. Encapsulation & Separation of Concerns
- **`Scanner`**: Encapsulates cursor position, lexeme extraction, line-tracking, and keyword mapping.
- **`Parser`**: Encapsulates grammar enforcement, precedence tracking, and AST creation.
- **`Env`**: Encapsulates variable state (Symbol Table) using an internal associative store (`std::map`), exposing clean `get()` and `set()` interfaces.

### 3. Memory Safety & RAII via Smart Pointers
- Memory ownership of AST nodes is strictly managed using `std::unique_ptr<Expr>` (`ExprPtr`) and `std::unique_ptr<Stmt>` (`StmtPtr`).
- When a syntax tree goes out of scope, the recursive destruction cascade safely deallocates every node, eliminating memory leaks and dangling pointers without a garbage collector.

### 4. Custom Exception Hierarchy
- Runtime diagnostics are isolated using `LangError`, which inherits from `std::runtime_error`:
  ```cpp
  struct LangError : std::runtime_error {
      using std::runtime_error::runtime_error;
  };
  ```
  Every syntax and runtime error captures line metadata for clear compiler diagnostics.

---

## 🏗️ Architecture & Pipeline

The interpreter executes source files through a classical four-stage pipeline:

```
+-------------------------------------------------------------+
|                     Source Code (.hb)                       |
+-------------------------------------------------------------+
                               |
                               v
+-------------------------------------------------------------+
|                  Phase 1: Lexical Analyzer                  |
|                 (Scanner / Tokenization)                    |
|       Converts raw characters into a stream of Tokens       |
+-------------------------------------------------------------+
                               |
                               v
+-------------------------------------------------------------+
|              Phase 2: Recursive Descent Parser              |
|        Validates grammar and builds the AST hierarchy       |
+-------------------------------------------------------------+
                               |
                               v
+-------------------------------------------------------------+
|               Phase 3: Abstract Syntax Tree (AST)           |
|      Polymorphic node hierarchy (Expressions & Statements)  |
+-------------------------------------------------------------+
                               |
                               v
+-------------------------------------------------------------+
|              Phase 4: Runtime Evaluator & Env               |
|      Executes AST nodes against the Environment / Symbol    |
+-------------------------------------------------------------+
                               |
                               v
+-------------------------------------------------------------+
|                        Program Output                       |
+-------------------------------------------------------------+
```

---

## 📖 Language Specification & Grammar

### Formal Grammar (EBNF)

```ebnf
program        ::= "hi" statement* "bye" ;
statement      ::= rememberStmt | sayStmt | inputStmt | ifStmt ;

rememberStmt   ::= "remember" IDENTIFIER "=" expression ";" ;
sayStmt        ::= "say" expression ";" ;
inputStmt      ::= "pucho" IDENTIFIER ";" ;
ifStmt         ::= "agar" "(" expression ")" block ( "warna" block )? ;
block          ::= "{" statement* "}" ;

expression     ::= equality ;
equality       ::= comparison ( ( "=" | "==" | "!=" ) comparison )* ;
comparison     ::= additive ( ( "<" | ">" ) additive )* ;
additive       ::= term ( ( "+" | "-" ) term )* ;
term           ::= unary ( ( "*" | "/" ) unary )* ;
unary          ::= "-" unary | primary ;
primary        ::= NUMBER | STRING | IDENTIFIER | "(" expression ")" ;
```

### Keywords & Operations

| Keyword / Symbol | Meaning | Example |
| :--- | :--- | :--- |
| `hi` | Program Entrypoint | Starts the program block |
| `bye` | Program Exit | Terminates execution |
| `remember` | Variable Declaration / Assignment | `remember count = 10;` |
| `say` | Standard Output (Print) | `say "Result: " + count;` |
| `pucho` | Standard Input (Prompt) | `pucho username;` |
| `agar ( ... )` | Conditional If | `agar (age > 18) { ... }` |
| `warna` | Else Branch | `warna { ... }` |
| `+`, `-`, `*`, `/` | Arithmetic Operators | `(10 + 5) * 2` |
| `<`, `>`, `=`, `!=` | Relational / Comparison | `score > 75` |
| `+` | String Concatenation | `"Hello, " + name` |

---

## 📂 Directory Structure

```
.
├── CMakeLists.txt        # Cross-platform CMake build configuration
├── Makefile              # Automated build, test, and clean script
├── LICENSE               # MIT Open Source License
├── README.md             # Project documentation and architectural guide
├── main.cpp              # Full interpreter engine (Scanner, Parser, AST, Env)
├── demo.hb               # Baseline showcase demo
└── examples/             # Categorized test and reference scripts
    ├── 01_hello.hb       # Basic output and string operations
    ├── 02_math_expressions.hb # Operator precedence & arithmetic
    ├── 03_conditionals.hb     # Nested branching logic (agar/warna)
    └── 04_interactive.hb      # Dynamic user input via stdin (pucho)
```

---

## ⚡ Build & Execution

### Prerequisites
- A modern C++ compiler (`g++` or `clang++`) supporting C++11 or higher.
- `make` or `cmake` (optional, for automated builds).

### Method 1: Using `make` (Recommended)

```bash
# Build the executable
make

# Run the baseline demo
make run

# Run full automated test suite across all examples
make test

# Clean build artifacts
make clean
```

### Method 2: Using Modern CMake

```bash
cmake -B build
cmake --build build
./build/interpreter demo.hb
```

### Method 3: Direct Compilation

```bash
g++ -std=c++11 -Wall -Wextra -O2 main.cpp -o interpreter
./interpreter demo.hb
```

---

## 💻 Code Showcase & Examples

### 1. Hello World & String Concatenation ([`examples/01_hello.hb`](file:///Users/yugrov/Desktop/imp%20files/examples/01_hello.hb))
```text
hi
say "Hello, World!";
remember author = "Yugam";
say "Interpreter written by: " + author;
bye
```

### 2. Math & Operator Precedence ([`examples/02_math_expressions.hb`](file:///Users/yugrov/Desktop/imp%20files/examples/02_math_expressions.hb))
```text
hi
remember a = 10;
remember b = 20;
remember c = 5;

// Multiplication takes precedence over addition (10 + 100 = 110)
remember res = a + b * c;
say "Result: " + res;
bye
```

### 3. Branching with Conditionals ([`examples/03_conditionals.hb`](file:///Users/yugrov/Desktop/imp%20files/examples/03_conditionals.hb))
```text
hi
remember score = 85;

agar (score > 90) {
  say "Grade: A+";
} warna {
  agar (score > 75) {
    say "Grade: A";
  } warna {
    say "Grade: Needs Improvement";
  }
}
bye
```

---

## 🛡️ Error Handling & Diagnostics

The interpreter features robust syntax and semantic verification, providing informative error reporting with line numbers:

- **Missing Program Delimiters**:
  ```text
  Error: [line 1] Program 'hi' se start hona chahiye
  ```
- **Undefined Variables**:
  ```text
  Error: [line 14] Undefined variable 'counter'
  ```
- **Type Mismatches**:
  ```text
  Error: [line 8] Operands must be numbers
  ```
- **Zero Division Safety**:
  ```text
  Error: [line 12] Division by zero
  ```

---

## 🗺️ Roadmap

- [x] Lexical Analyzer & Tokenizer
- [x] Recursive Descent Parser & Precedence Hierarchy
- [x] Dynamic Polymorphic AST
- [x] Scope-Aware Symbol Table (Environment)
- [x] Hinglish Keywords (`hi`, `bye`, `remember`, `say`, `pucho`, `agar`, `warna`)
- [ ] Loop Structures (`jabtak` / While loop support)
- [ ] User-Defined Functions (`kaam` / Functions with arguments and return values)
- [ ] Dynamic Arrays / Lists
- [ ] Bytecode Virtual Machine (VM) Compiler Target

---

## 👤 Author & License

- **Author**: Yugam Grover ([@yugam-dtu](https://github.com/yugam-dtu))
- **License**: Released under the [MIT License](LICENSE).
