# TinyLisp

TinyLisp is a lisp-like programing language, interpreting a small subset of Lisp expressions.

This project contains TinyLisp interpreter, including garbage collector, lexer, parser, compiler and virtual SEDC machine.

It can read expressions from file, or run in REPL mode.

## Language
Note: false is represented as number 0, true as every other integer.

### List
List can be constructed with `()`, cons cell can be constructed with `( cons car cdr )` or as `( car . cdr )`. Empty list can be entered with `nil`.

Members of list are accessed with `car` and `cdr`, `consp` tests if argument is cons cell (or list) or not.

### Basic operators
TinyLisp supports basic binary arithmetic operations (`+`, `-`, `*`, `/`) and relation operators `<` and `>`. Equality of two numbers, symbols or cons cells can be tested with `eq`.

### Lambda expression
Lambda expression is in a form of `(lambda (args) body)`.

### If expresion
If expression can be constructed as `(if condition true-case false-case)`. If condition is evaluated as true, true-case is evaluated, otherwise false-case is evaluated.

### Quoting
TinyLisp supports quotes and quasiquotes.

Argument of `quote` is not evaluated, argument of `quasiquote` is also not evaluated, but sub-expressions marked with `unquote` are evaluated.

As syntactic sugar, quote can be replaced with ` ' `, quasiquote with ``` ` ``` and unquote with `,`.

### Let and letrec
TinyLisp supports declaring local variables with `let` and `letrec` (for recursive definition).

Syntax for both let and letrec is `( let ( var1 var2 var3 ... varN ) body)`, where each var is in the form of `(name value)`.

### Defun
Defun can be only used on a global scope, defining global function. Form is `(defun name (args) body)`.

### Input and output
With `(read)`, TinyLisp waits for user input and parses that into symbol or integer.

`print` outputs its arguments to standard output.