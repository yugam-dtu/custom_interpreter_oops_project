hi
// Arithmetic Precedence and Expression Evaluation Demo
say "--- Arithmetic & Expression Precedence ---";

remember a = 10;
remember b = 20;
remember c = 5;

say "a = " + a;
say "b = " + b;
say "c = " + c;

// Demonstrating precedence: multiplication/division before addition/subtraction
remember result1 = a + b * c;
say "a + b * c = " + result1; // Expected: 10 + (20 * 5) = 110

// Demonstrating parenthesized expressions
remember result2 = (a + b) * c;
say "(a + b) * c = " + result2; // Expected: (10 + 20) * 5 = 150

// Division and unary negation
remember result3 = (b / c) - (-a);
say "(b / c) - (-a) = " + result3; // Expected: (20 / 5) - (-10) = 4 + 10 = 14
bye
