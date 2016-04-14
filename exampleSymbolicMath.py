#calculate some important equations symbolically in Python
import sympy as sp

#define the symbols we want
a1_1 = sp.Symbol('a1_1');
a1_2 = sp.Symbol('a1_2');
a1_3 = sp.Symbol('a1_3');
a1_4 = sp.Symbol('a1_4');
a1_5 = sp.Symbol('a1_5');
a2_1 = sp.Symbol('a2_1');
a2_2 = sp.Symbol('a2_2');
a2_3 = sp.Symbol('a2_3');
a2_4 = sp.Symbol('a2_4');
a2_5 = sp.Symbol('a2_5');
a3_1 = sp.Symbol('a3_1');
a3_2 = sp.Symbol('a3_2');
a3_3 = sp.Symbol('a3_3');
a3_4 = sp.Symbol('a3_4');
a3_5 = sp.Symbol('a3_5');
a4_1 = sp.Symbol('a4_1');
a4_2 = sp.Symbol('a4_2');
a4_3 = sp.Symbol('a4_3');
a4_4 = sp.Symbol('a4_4');
a4_5 = sp.Symbol('a4_5');
a5_1 = sp.Symbol('a5_1');
a5_2 = sp.Symbol('a5_2');
a5_3 = sp.Symbol('a5_3');
a5_4 = sp.Symbol('a5_4');
a5_5 = sp.Symbol('a5_5');

A = sp.Matrix([[a1_1, a1_2, a1_3, a1_4, a1_5], [a2_1, a2_2, a2_3, a2_4, a2_5], [a3_1, a3_2, a3_3, a3_4, a3_5], [a4_1, a4_2, a4_3, a4_4, a4_5], [a5_1, a5_2, a5_3, a5_4, a5_5]])
invA = A.inverse_ADJ()

#write the diagonal entries to a file in a way that is readable by expparser.c
f = open("exampleInput.txt", "w")
for i in range(4):
    f.write("[%s], " % invA[i, i])
f.write("[%s]" % invA[4, 4])
f.close()