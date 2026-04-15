# Simulações e *Test Benches* 

Antes de implementar um circuito em FPGA ou ASIC, é fundamental testar seu funcionamento em simulação. Em Verilog, isso é feito usando um *test bench*, que nada mais é do que um código adicional não sintetizável, escrito para verificar o módulo que você deseja testar.

Considere o seguinte código para um somador de 4 bits:

```verilog
module adder(
  input [3:0] a, b,
  output [4:0] sum);
  assign sum = a + b;
endmodule
```

Este código resulta em um somador simples de 4 bits, que pode ser sintetizado em hardware. Agora, vamos construir um *test bench* simples para testá-lo. O código do teste serve para simular o comportamento do somador, aplicando diferentes valores de entrada e observando a saída. 

- O **design**, também chamado de *Design Under Test* (DUT) ou *Unit Under Test* (UUT) é o circuito que você escreveu, ele será instanciado dentro do *test bench*;
- O **test bench** gera estímulos (sinais de entrada, como clock, reset, dados) e observa as respostas do DUT;
- Dessa forma, você pode verificar se o circuito se comporta corretamente antes de gastar tempo e recursos na síntese e na gravação em hardware.

Exemplo simples de *test bench* para um somador:

```verilog
module tb_adder;
  reg [3:0] a, b;
  wire [4:0] sum;

  adder uut (.a(a), .b(b), .sum(sum));

  initial begin
    a = 4'd3; b = 4'd2;
    #10 a = 4'd7; b = 4'd8;
    #10 $finish;
  end
endmodule
```

Aqui o `uut` (unit under test) recebe valores, e a simulação permite observar se sum tem o resultado esperado.

## DigitalJS

Um simulador simples, mas interessante para quem está começando é o [DigitalJS](https://menotti.pro.br/ld/digitaljs/). Nele você pode interagir gráficamente com o circuito gerado sem a necessidade de criar um *test bench*. 

## Icarus Verilog (`iverilog`)

O [Icarus Verilog](https://github.com/steveicarus/iverilog) é um simulador Verilog de código aberto. Ele ainda não cobre todas as funcionalidades da linguagem, mas é bastante usado por sua simplicidade e facilidade de instalação. 

Usando nosso exemplo do somador acima, podemos executar os seguintes comandos para realizar sua simulação e observar sua saída:

```zsh
 % iverilog *.v && ./a.out
tb_adder.v:10: $finish called at 20 (1s)
```

Ele é capaz de identificar automaticamente a hierarquia entre a implementação e o test bench, mas apenas indica que a simulação terminou quando encontrou a função `$finish`. No entanto, ele não nos dá nenhuma informação útil da simulação, então vamos modificar o test bench para incluir uma chamadas às funções `$display` e `$monitor`. A primeira imprime uma única vez, então usamos para fazer um cabeçalho. A segunda, monitora os sinais e imprime sempre que um deles é modificado. 

```verilog
module tb_adder;
  reg [3:0] a, b;
  wire [4:0] sum;

  adder uut (.a(a), .b(b), .sum(sum));

  initial begin
    $display("Time\t a\t b\t sum");
    $monitor("%3t\t%d\t%d\t%3d", $time, a, b, sum);
    a = 4'd3; b = 4'd2;
    #10 a = 4'd7; b = 4'd8;
    #10 $finish;
  end
endmodule
```

A saída agora nos mostra os valores dos sinais ao longo da simulação:

```zsh
 % iverilog *.v && ./a.out
Time     a       b       sum
  0      3       2        5
 10      7       8       15
tb_adder.v:12: $finish called at 20 (1s)
```

Embora a saída na console possa ser útil em alguns casos, podemos gerar também um arquivo do forma de onda (_waveform_) .VCD para explorar a simulação visualmente. Para isso, vamos incluir a função `$dumpvars`, que resulta na geração deste arquivo. Ele pode ser aberto usando um leitor como o [GTKWave](https://gtkwave.github.io/gtkwave/) ou extensões do VSCode como [WaveTrace](https://marketplace.visualstudio.com/items?itemName=wavetrace.wavetrace) ou [surfer](https://marketplace.visualstudio.com/items?itemName=surfer-project.surfer).

![surfer.png](img/surfer.png)

Para conhecer técnicas mais avançadas de simulação e testes, assista a esta [sequência de vídeos](https://www.youtube.com/playlist?list=PLhaFCmjMNuYZCoXbLDGi4-gqSBaKaEMsV) sobre o assunto. 

## Verilator

