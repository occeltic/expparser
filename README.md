# expparser
A program for parsing multiple infix algebraic expressions and simplifying them into a small number of operations using expression trees. Currently recognized operations are:
+x: POSITIVE
-x: NEGATIVE
x+y: ADDITION
x-y: SUBTRACTION
x*y: MULTIPLICATION
x/y: DIVISION
x^y: POWER

(), {}, and [] are all interchangable. Multiple expressions can be simplified simultaneously. Each expression should be enclosed in a set of brackets, with multiple expressions separated by commas.

To use expparser.c, download and uncompress the .zip, navigate to this directory, and call make. You will either need to have gcc installed, or else alter the makefile to support the C-compiler of your choice.

Once compiled, syntax is as follows:

~$ ./expparser < inputfile.txt > outputfile.txt

inputfile.txt should contain a comma separated list of bracketed  infix algebraic expressions, i.e.:
[(x+y)^2+(x+z)^3], [2*(x+z)^2 - (x+y)^3], [-(x+y)^2 - (z-x)^3]

outputfile.txt will contain a series of operations and substitutions followed by a list telling you which lines refer to the elements in the initial expression list. For the simple example above it would look like:

\#0 = x+y

\#1 = #0^2

\#2 = x+z

\#3 = #2^3

\#4 = #1+#3

\#5 = #2^2

\#6 = 2*#5

\#7 = #0^3

\#8 = #6-#7

\#9 = -#1

\#10 = z-x

\#11 = #10^3

\#12 = #9-#11
---Assembly Token---


---Operator Stack---

---Operand Stack---

---Expression List---

---Expression 0---

\#4

---Expression 1---

\#8

---Expression 2---

\#12

How expparser.c does this is by building tree representations of each algebraic expression, and then iteratively substituting for the most common subtree in the collection of trees. There is support for the associative property (i.e. x+y is equivalent to y+x) but it won't do "smarter" things like expanding powers or algebraicly simplifying expressions.

Having this type of machine process is often useful in the context of symbolic math libraries, where closed form solutions can be found to complex problems (i.e. many matrix problems) but where the resulting formulae are so long as to be incomprehensible to a human reader. With minimal effort, expparser.c can turn this unintelligible string of symbols into usuable machine instructions which can be copy/pasted and find/changed to fit virtually any programming language.

For a simple example of this process, let's say we were interested in calculating the diagonal values of the inverse matrix of an invertible 5 x 5 matrix. exampleSymbolicMath.py uses Python's sympy library to solve for these entries and writes them to exampleInput.txt in a usable format. If we then call ./expparser < exampleInput.txt > exampleOutput.txt we simplify the 5 expressions down to 431 sequential operations.
