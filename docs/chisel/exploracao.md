## 3. Exploração do problema
$$
\begin{bmatrix}
a_{11} & a_{12} \\
a_{21} & a_{22}
\end{bmatrix}
\cdot
\begin{bmatrix}
x_{1} \\
x_{2}
\end{bmatrix}
=
\begin{bmatrix}
f_{1} \\
f_{2}
\end{bmatrix}
\;\;\Longrightarrow\;\;
\begin{bmatrix}
a_{11} & a_{12} \\
0      & a'_{22}
\end{bmatrix}
\cdot
\begin{bmatrix}
x_{1} \\
x_{2}
\end{bmatrix}
=
\begin{bmatrix}
f_{1} \\
f'_{2}
\end{bmatrix}
$$

$$
\begin{aligned}
m_{21} &= \tfrac{a_{21}}{a_{11}} &\qquad x_2 &= \tfrac{f'_2}{a'_{22}} \\
a'_{22} &= a_{22} - m_{21}a_{12} &\qquad x_1 &= \tfrac{f_1 - a_{12}x_2}{a_{11}} \\
f'_2 &= f_2 - m_{21}f_1
\end{aligned}
$$

O método direto aplicado aqui é a **eliminação gaussiana**, que funciona eliminando uma variável a cada iteração até transformar o sistema em uma forma triangular superior. A partir desse formato, a solução é obtida por retrossubstituição. No caso do sistema 2×2 mostrado acima, é necessária apenas uma iteração para eliminar a variável \(a_{21}\), reduzindo o problema a duas equações simples. Assim, a resolução completa se resume a calcular as expressões de \(x_2\) e \(x_1\) apresentadas, evidenciando como operações básicas de divisão, multiplicação e subtração permitem encontrar a solução final. Além disso, as soluções do sistema podem ser analisadas pelo **determinante de \(A\)**: se \(\det(A) \neq 0\), existe exatamente uma solução; caso contrário, \(\det(A) = 0\) implica que \(a'_{22} = 0\), o que leva a um sistema singular sem solução única.

> **Nota:** Nesta breve exploração assumimos que os pivôs nunca são nulos, de modo que não há necessidade de realizar pivotamento.


