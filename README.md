# Hi-Bye Lang Interpreter 🚀

A lightweight interpreter written in C++ (C++11 compatible) for **Hi-Bye Lang**, a fun Hinglish toy programming language.

## 📌 Features

- **Program Structure**: Programs begin with `hi` and conclude with `bye`.
- **Variables**: Declare and initialize variables using `remember <var> = <expr>;`.
- **I/O Operations**:
  - Print values or strings using `say <expr>;`.
  - Accept user input with `pucho <var>;`.
- **Control Flow**:
  - Conditional branching with `agar (<condition>) { ... } warna { ... }` (if/else).
- **Expressions & Operators**:
  - Arithmetic: `+`, `-`, `*`, `/`
  - Relational: `<`, `>`, `=`, `==`, `!=`
  - String concatenation via `+`
  - Parenthesized sub-expressions `( ... )`
- **Architecture**:
  - Custom Tokenizer/Scanner
  - Recursive Descent Parser & AST Construction
  - Scope-aware Environment & AST Evaluator

---

## 🛠️ Build & Run

### Prerequisites
- Any C++ compiler supporting C++11 or higher (e.g., `g++` / `clang++`).

### Build
```bash
g++ -std=c++11 main.cpp -o bro
```

### Run
Run the interpreter by passing a `.hb` source file:
```bash
./bro demo.hb
```

---

## 📝 Example (`demo.hb`)

```text
hi
remember x = 5;
say x;

agar (x = 5)
{
  say "x is five";
}
warna
{
  say "not found";
}

pucho naam;
say "Hello " + naam;
remember y = (x + 3) * 2;
say y;
agar (y > 20) { say "big"; } warna { say "small"; }
bye
```

---

## 📂 Project Structure

```
.
├── main.cpp     # Scanner, Parser, AST, and Evaluator implementation
├── demo.hb      # Sample Hi-Bye source code
└── README.md    # Project documentation
```
