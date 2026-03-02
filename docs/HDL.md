# Linguagens de Descrição de Hardware (HDL)

Hoje em dia, o projeto de hardware quase não é mais feito com diagramas esquemáticos, mas com Linguagens de Descrição de Hardare (HDL[^1]). Neste tutorial você vai aprender o básico de Verilog que é provavelmente e linguagem mais usada atualmente. Também serão usadas linguagens mais modernas, que posteriormente geram Verilog sintetizável. 

Em Verilog há três formas diferentes de se especificar hardware, cada uma delas mais ou menos adequadas a determinadas necessidades ou nível de abstração. A seguir, vamos usar um exemplo simples - um multiplexador 2x1 - para ilustrar cada uma dessas formas.


=== "Estrutural"
    ```verilog
    module structural_mux2x1 (
      input a, b, sel,
      output y);
      
      wire not_sel, and_a, and_b;
      not (not_sel, sel);
      and (and_a, a, not_sel);
      and (and_b, b, sel);
      or (y, and_a, and_b);
    endmodule
    ```
    A forma estrutural é a mais próxima do nível de portas lógicas e, portanto, a menos abstrata. Nela você instancia componentes primitivos e conecta suas entradas e saídas, embora isso possa tornar o código mais verboso. 
    Além de instanciar primitivas básicas da linguagem, você também pode instanciar módulos definidos anteriormente, permitindo a construção hierárquica de designs complexos a partir de blocos menores e reutilizáveis. Este costuma ser o método mais usado nos arquivos de mais alto nível de um projeto (top level). 

=== "Dataflow"
    ```verilog
    module dataflow_mux2x1 (
      input a, b, sel,
      output y);
      
      assign y = a & ~sel | b & sel;

      // Alternativamente:
    // assign y = sel ? b : a;

    endmodule
    ```
    A forma dataflow é uma maneira de descrever o hardware em termos de fluxo de dados entre registradores e portas lógicas, usando expressões contínuas. Ela é mais abstrata que a forma estrutural e permite descrever o comportamento do circuito de forma mais concisa, inferindo o hardware a partir de expressões entre operandos.
    Ela é bastante usada para descrever circuitos combinacionais e é útil para expressar operações lógicas e aritméticas de maneira clara. Também é chamada de funcional, pois cada saída é uma função das entradas.

=== "Comportamental"
    ```verilog
    module behavioral_mux2x1 (
      input a, b, sel,
      output reg y);
      
      always @(*) 
        if (sel)
          y = b;
        else
          y = a;
    endmodule
    ```
    A forma comportamental é a mais abstrata das três. Ela permite descrever o comportamento do circuito usando blocos `always`, que são avaliados em resposta a mudanças nas entradas. 
    Essa forma é muito útil para descrever circuitos sequenciais, onde o estado do circuito depende de eventos anteriores, como flip-flops e contadores. Ela também pode ser usada para descrever circuitos combinacionais, mas é mais comum em designs que envolvem lógica sequencial.

!!! warning
    O importante é entender que todas essas formas são apenas diferentes maneiras de descrever o mesmo hardware. O Verilog sintetizável é aquele que pode ser convertido em um circuito físico por ferramentas de síntese, independentemente da forma usada para descrevê-lo. A escolha da forma depende do nível de abstração desejado e da clareza do código, mas é importante endender que nenhuma delas é um programa a ser executado, mas sim a descrição de um circuito a ser implementado em hardware.


[^1]: *Hardware Description Language* 

