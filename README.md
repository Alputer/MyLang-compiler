# Cmpe230-Project1

In this project, we developed a compiler called mylang2IR for a language called MyLang. This compiler compiles a simplified high level language MyLang to LLVM language.


# Running The Code:

This will create executable file with name "mylang2ir":
```bash
g++ -std=c++14 curr.cpp -o mylang2ir
```

Say MyLang code is in file "file.my", then execute this command to convert MyLang to LLVM:
```bash
mylang2ir file.my
```

The output LLVM code will be in the file named "file.ll". (this file is overwritten if it exists. Otherwise, it is created.)




# Features of MyLang:
1. Variables will be integer variables. Their default value will be 0 (i.e. if they are not initialized).
2. Executable statements will consist of one-line statements, while-loop, and if compound
statements. Note that no nested while-loops or if statements are allowed.
3. One-line statements are either assignment statements or print statements that print the
value of an expression.
4. There is a function choose(expr1,expr2,expr3,expr4)which returns expr2 if expr1 is
equal to 0, returns expr3 if expr1 is positive and returns expr4 if expr1 is negative.
5. As operations in expressions, you are required to implement only multiplication, division
addition, and subtraction: *, /, +, - . These are binary operand operations. Unary minus
operation is not supported but parentheses are allowed. Operator precedence needs to be
implemented as in other programming languages, such as C or Java.
6. On a line, everything after the # sign is considered as comments.
7. If statement will have the following format:
if (expr) {
.....
.....
}
If expr has a nonzero value, it means true. If expr has zero value, it means false. Note
that there is no else part. There are no nested if statements.
8. While loop will have the following format:
while (expr) {
.....
.....
}
If expr has a nonzero value, it means true. If expr has zero value, it means false. There are
no nested while statements.
9. print(id) statement, prints the value of variable id.
10. In case of a syntax error in line x, print “Line x: syntax error”


# Further Information
For further details, read "description.pdf".
