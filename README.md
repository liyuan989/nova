# nova

A compiler for tiny language, which is simple practice for [Compiler Construction Principles and Practice](http://www.amazon.com/Compiler-Construction-Principles-Kenneth-Louden/dp/0534939724)

### BNF Grammer

```
program  ->  stmt-sequence
stmt-sequence  ->  stmt-sequence; statement | statement
statement  ->  if-stmt | repeat-stmt | assign-stmt | read-stmt | write-stmt
if-stmt  ->  if exp then stmt-sequence end
             | if exp then stmt-sequence else stmt-sequence end
repeat-stmt  ->  repeat stmt-sequence until exp
assign-stmt  ->  identifier := exp
exp  ->  simple-exp | simple-exp comparison-op simple-exp
comparison-op  ->  < | =
simple-exp  ->  simple-exp addop term | term
addop  ->  + | -
term  ->  term mulop factor | factor
mulop  ->  * | /
factor  ->  (exp) | numver | identifier
```
