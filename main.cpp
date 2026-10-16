// hi-bye lang: mini interpreter (progress demo)
// Supports: hi/bye, remember, say, pucho (input), agar/warna (if/else)
// Build: g++ -std=c++11 main.cpp -o bro   (purane compilers pe bhi chalega)
// Run:   ./bro demo.hb

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cstdlib>
#include <cctype>
#include <stdexcept>
using namespace std;

// ---------- Value & errors ----------
// Value: ya to number hai ya string (std::variant ki jagah simple struct,
// taaki purane compilers (C++11) pe bhi compile ho)
struct Value {
    bool isNum;
    double num;
    string str;
    Value() : isNum(true), num(0) {}
    Value(double d) : isNum(true), num(d) {}
    Value(const string& s) : isNum(false), num(0), str(s) {}
    bool operator==(const Value& o) const {
        if (isNum != o.isNum) return false;
        return isNum ? num == o.num : str == o.str;
    }
};

// make_unique C++14 mein aaya, isliye apna helper
template <class T, class... A>
unique_ptr<T> mk(A&&... a) { return unique_ptr<T>(new T(std::forward<A>(a)...)); }

struct LangError : runtime_error {
    using runtime_error::runtime_error;
};

string toStr(const Value& v) {
    if (!v.isNum) return v.str;
    ostringstream o;
    o << v.num;
    return o.str();
}

bool truthy(const Value& v) {
    if (v.isNum) return v.num != 0;
    return !v.str.empty();
}

string at(int line) {
    ostringstream o;
    o << "[line " << line << "] ";
    return o.str();
}

// ---------- Tokens ----------
enum class T {
    HI, BYE, REMEMBER, SAY, PUCHO, AGAR, WARNA,
    IDENT, NUMBER, STRING,
    EQ, NEQ, LT, GT, PLUS, MINUS, STAR, SLASH,
    LPAREN, RPAREN, LBRACE, RBRACE, SEMI, END
};

struct Token {
    T type;
    string text;
    double num;
    int line;
};

// ---------- Scanner (Ch 4) ----------
class Scanner {
    string src;
    vector<Token> out;
    size_t i = 0;
    int line = 1;
    static const map<string, T> kw;

    void add(T ty, string text = "", double num = 0) {
        out.push_back({ty, text, num, line});
    }

public:
    Scanner(string s) : src(std::move(s)) {}

    vector<Token> scan() {
        while (i < src.size()) {
            char c = src[i++];
            switch (c) {
                case ' ': case '\t': case '\r': break;
                case '\n': line++; break;
                case '(': add(T::LPAREN); break;
                case ')': add(T::RPAREN); break;
                case '{': add(T::LBRACE); break;
                case '}': add(T::RBRACE); break;
                case ';': add(T::SEMI); break;
                case '+': add(T::PLUS); break;
                case '-': add(T::MINUS); break;
                case '*': add(T::STAR); break;
                case '<': add(T::LT); break;
                case '>': add(T::GT); break;
                case '=':
                    if (i < src.size() && src[i] == '=') i++;   // '=' aur '==' dono chalenge
                    add(T::EQ);
                    break;
                case '!':
                    if (i < src.size() && src[i] == '=') { i++; add(T::NEQ); }
                    else throw LangError(at(line) + "Unexpected '!'");
                    break;
                case '/':
                    if (i < src.size() && src[i] == '/') {      // comment
                        while (i < src.size() && src[i] != '\n') i++;
                    } else add(T::SLASH);
                    break;
                case '"': {
                    string s;
                    while (i < src.size() && src[i] != '"') {
                        if (src[i] == '\n') line++;
                        s += src[i++];
                    }
                    if (i >= src.size()) throw LangError(at(line) + "Unterminated string");
                    i++;  // closing "
                    add(T::STRING, s);
                    break;
                }
                default:
                    if (isdigit((unsigned char)c)) {
                        size_t s = i - 1;
                        while (i < src.size() && (isdigit((unsigned char)src[i]) || src[i] == '.')) i++;
                        string txt = src.substr(s, i - s);
                        add(T::NUMBER, txt, strtod(txt.c_str(), nullptr));
                    } else if (isalpha((unsigned char)c) || c == '_') {
                        size_t s = i - 1;
                        while (i < src.size() && (isalnum((unsigned char)src[i]) || src[i] == '_')) i++;
                        string txt = src.substr(s, i - s);
                        auto it = kw.find(txt);
                        add(it == kw.end() ? T::IDENT : it->second, txt);
                    } else {
                        throw LangError(at(line) + "Unexpected character '" + string(1, c) + "'");
                    }
            }
        }
        add(T::END);
        return out;
    }
};

const map<string, T> Scanner::kw = {
    {"hi", T::HI}, {"bye", T::BYE}, {"remember", T::REMEMBER},
    {"say", T::SAY}, {"pucho", T::PUCHO},
    {"agar", T::AGAR}, {"warna", T::WARNA}
};

// ---------- Environment (Ch 8) ----------
class Env {
    map<string, Value> vars;
public:
    void set(const string& name, Value v) { vars[name] = std::move(v); }
    Value get(const string& name, int line) {
        auto it = vars.find(name);
        if (it == vars.end()) throw LangError(at(line) + "Undefined variable '" + name + "'");
        return it->second;
    }
};

// ---------- AST: Expressions ----------
struct Expr {
    virtual ~Expr() = default;
    virtual Value eval(Env& env) = 0;
};
using ExprPtr = unique_ptr<Expr>;

struct Literal : Expr {
    Value v;
    Literal(Value v) : v(std::move(v)) {}
    Value eval(Env&) override { return v; }
};

struct Var : Expr {
    string name;
    int line;
    Var(string n, int l) : name(std::move(n)), line(l) {}
    Value eval(Env& env) override { return env.get(name, line); }
};

struct Binary : Expr {
    ExprPtr l, r;
    T op;
    int line;
    Binary(ExprPtr l, T op, ExprPtr r, int line)
        : l(std::move(l)), r(std::move(r)), op(op), line(line) {}

    Value eval(Env& env) override {
        Value a = l->eval(env);
        Value b = r->eval(env);
        bool nums = a.isNum && b.isNum;

        if (op == T::EQ)  return a == b ? 1.0 : 0.0;
        if (op == T::NEQ) return a == b ? 0.0 : 1.0;
        if (op == T::PLUS) {
            if (nums) return Value(a.num + b.num);
            return toStr(a) + toStr(b);   // string concat
        }

        if (!nums) throw LangError(at(line) + "Operands must be numbers");
        double x = a.num, y = b.num;
        switch (op) {
            case T::MINUS: return x - y;
            case T::STAR:  return x * y;
            case T::SLASH:
                if (y == 0) throw LangError(at(line) + "Division by zero");
                return x / y;
            case T::LT: return x < y ? 1.0 : 0.0;
            case T::GT: return x > y ? 1.0 : 0.0;
            default: return 0.0;
        }
    }
};

// ---------- AST: Statements ----------
struct Stmt {
    virtual ~Stmt() = default;
    virtual void exec(Env& env) = 0;
};
using StmtPtr = unique_ptr<Stmt>;
using Block = vector<StmtPtr>;

void runBlock(Block& b, Env& env) {
    for (auto& s : b) s->exec(env);
}

struct RememberStmt : Stmt {
    string name;
    ExprPtr init;
    RememberStmt(string n, ExprPtr e) : name(std::move(n)), init(std::move(e)) {}
    void exec(Env& env) override { env.set(name, init->eval(env)); }
};

struct SayStmt : Stmt {
    ExprPtr e;
    SayStmt(ExprPtr e) : e(std::move(e)) {}
    void exec(Env& env) override { cout << toStr(e->eval(env)) << "\n"; }
};

struct InputStmt : Stmt {
    string name;
    InputStmt(string n) : name(std::move(n)) {}
    void exec(Env& env) override {
        string line;
        getline(cin, line);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const char* start = line.c_str();   // number hai to number, warna string
        char* endp = nullptr;
        double d = strtod(start, &endp);
        if (endp != start && *endp == '\0') { env.set(name, Value(d)); return; }
        env.set(name, Value(line));
    }
};

struct IfStmt : Stmt {
    ExprPtr cond;
    Block thenB, elseB;
    void exec(Env& env) override {
        if (truthy(cond->eval(env))) runBlock(thenB, env);
        else runBlock(elseB, env);
    }
};

// ---------- Parser (Ch 5-6) ----------
// program    -> "hi" statement* "bye"
// statement  -> remember | say | pucho | agar
// expression -> additive ( ("=" | "!=" | "<" | ">") additive )?
// additive   -> term ( ("+" | "-") term )*
// term       -> unary ( ("*" | "/") unary )*
// unary      -> "-" unary | primary
// primary    -> NUMBER | STRING | IDENT | "(" expression ")"
class Parser {
    vector<Token> t;
    size_t p = 0;

    const Token& peek() { return t[p]; }
    bool check(T ty) { return peek().type == ty; }
    bool match(T ty) { if (check(ty)) { p++; return true; } return false; }
    const Token& advance() { return t[p++]; }
    const Token& expect(T ty, const string& msg) {
        if (!check(ty)) throw LangError(at(peek().line) + msg);
        return advance();
    }

    StmtPtr statement() {
        if (match(T::REMEMBER)) {
            string name = expect(T::IDENT, "Variable ka naam chahiye after 'remember'").text;
            expect(T::EQ, "Expected '=' after variable name");
            auto e = expression();
            expect(T::SEMI, "Expected ';'");
            return mk<RememberStmt>(name, std::move(e));
        }
        if (match(T::SAY)) {
            auto e = expression();
            expect(T::SEMI, "Expected ';' after say");
            return mk<SayStmt>(std::move(e));
        }
        if (match(T::PUCHO)) {
            string name = expect(T::IDENT, "Variable ka naam chahiye after 'pucho'").text;
            expect(T::SEMI, "Expected ';'");
            return mk<InputStmt>(name);
        }
        if (match(T::AGAR)) {
            auto s = mk<IfStmt>();
            expect(T::LPAREN, "Expected '(' after 'agar'");
            s->cond = expression();
            expect(T::RPAREN, "Expected ')' after condition");
            s->thenB = block();
            if (match(T::WARNA)) s->elseB = block();
            return s;
        }
        throw LangError(at(peek().line) + "Unknown statement");
    }

    Block block() {
        expect(T::LBRACE, "Expected '{'");
        Block b;
        while (!check(T::RBRACE)) {
            if (check(T::END) || check(T::BYE)) throw LangError(at(peek().line) + "Missing '}'");
            b.push_back(statement());
        }
        advance();  // }
        return b;
    }

    ExprPtr expression() {
        auto left = additive();
        if (check(T::EQ) || check(T::NEQ) || check(T::LT) || check(T::GT)) {
            Token op = advance();
            auto right = additive();
            return mk<Binary>(std::move(left), op.type, std::move(right), op.line);
        }
        return left;
    }

    ExprPtr additive() {
        auto left = term();
        while (check(T::PLUS) || check(T::MINUS)) {
            Token op = advance();
            left = mk<Binary>(std::move(left), op.type, term(), op.line);
        }
        return left;
    }

    ExprPtr term() {
        auto left = unary();
        while (check(T::STAR) || check(T::SLASH)) {
            Token op = advance();
            left = mk<Binary>(std::move(left), op.type, unary(), op.line);
        }
        return left;
    }

    ExprPtr unary() {
        if (check(T::MINUS)) {
            Token op = advance();
            return mk<Binary>(mk<Literal>(0.0), T::MINUS, unary(), op.line);
        }
        return primary();
    }

    ExprPtr primary() {
        if (check(T::NUMBER)) return mk<Literal>(advance().num);
        if (check(T::STRING)) return mk<Literal>(Value(advance().text));
        if (check(T::IDENT)) { Token n = advance(); return mk<Var>(n.text, n.line); }
        if (match(T::LPAREN)) {
            auto e = expression();
            expect(T::RPAREN, "Expected ')'");
            return e;
        }
        throw LangError(at(peek().line) + "Expression expected");
    }

public:
    Parser(vector<Token> tokens) : t(std::move(tokens)) {}

    Block parseProgram() {
        expect(T::HI, "Program 'hi' se start hona chahiye");
        Block b;
        while (!check(T::BYE)) {
            if (check(T::END)) throw LangError(at(peek().line) + "Missing 'bye' at end");
            b.push_back(statement());
        }
        advance();  // bye
        expect(T::END, "'bye' ke baad kuch nahi likh sakte");
        return b;
    }
};

// ---------- main ----------
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <file.hb>\n";
        return 1;
    }
    ifstream f(argv[1]);
    if (!f) { cerr << "File nahi khuli: " << argv[1] << "\n"; return 1; }
    stringstream ss;
    ss << f.rdbuf();

    try {
        Scanner scanner(ss.str());
        Parser parser(scanner.scan());
        Block program = parser.parseProgram();
        Env env;
        runBlock(program, env);
    } catch (const LangError& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}