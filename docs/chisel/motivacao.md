
## Sistemas Lineares

A resolução de sistemas lineares é um problema clássico em ciência e engenharia, e sua implementação em hardware envolve a utilização de diversas estruturas fundamentais. Ao explorar esse problema, é possível abordar de forma prática os principais blocos que compõem circuitos digitais e entender como eles se organizam em arquiteturas mais complexas.  

Em primeiro lugar, operações básicas como soma e subtração podem ser mapeadas em **circuitos combinacionais**, servindo de exemplo inicial para entender a construção de operadores aritméticos e como eles se conectam em módulos maiores. Na sequência, operações mais elaboradas como multiplicação e divisão exigem a presença de **circuitos sequenciais**, que introduzem o conceito de latência, temporização e controle de dados ao longo do tempo.  

Além disso, ao se utilizar **máquinas de estado finitas (FSMs)** para organizar métodos diretos ou iterativos de solução, o estudante tem contato com uma das estruturas mais importantes em projeto digital, responsável por controlar o fluxo de execução e coordenar as diferentes etapas de cálculo. Esse ponto é particularmente relevante quando se considera a implementação de algoritmos de solução iterativa, como métodos de Jacobi ou Gauss-Seidel.  

Outro aspecto motivador é que a solução de sistemas lineares envolve operações que também aparecem em **processadores modernos**, como o uso de **operações em ponto flutuante**, manipulação de **vetores** para representar equações, e sinais de controle que coordenam a execução em arquiteturas mais sofisticadas. Assim, o estudo desse problema permite não apenas praticar a construção de módulos de hardware isolados, mas também visualizar como esses blocos estão presentes em projetos reais de CPUs, GPUs e aceleradores de propósito específico.  

Por fim, ao longo deste tutorial utilizaremos **exemplos reais de classes implementadas no processador Rocket**, não apenas para nos familiarizar com a descrição de hardware em Chisel, mas também para desenvolver a capacidade de ler e entender como projetos sérios e amplamente utilizados na academia e na indústria aplicam a linguagem na construção de hardware complexo.

## Exploração do problema
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

[Começo do Tutorial ⟶](somadores.md)
