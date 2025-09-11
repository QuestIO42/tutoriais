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

Vamos construir um *test bench* simples para testá-lo:

- O **design**, também chamado de *Design Under Test* (DUT) ou *Unit Under Test* (UUT) é o circuito que você escreveu;
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

O [Icarus Verilog](https://github.com/steveicarus/iverilog) é um simulador Verilog de código aberto. 

## Verilator

