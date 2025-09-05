# Simulações e Test Benches 

Antes de implementar um circuito em FPGA ou ASIC, é fundamental testar seu funcionamento em simulação. Em Verilog, isso é feito usando um test bench, que nada mais é do que um código adicional não sintetizável, escrito para verificar o módulo que você deseja testar.

- O **design**, também chamado de *Design Under Test* (DUT) ou *Unit Under Test* (UUT) é o circuito que você escreveu;
- O **test bench** gera estímulos (sinais de entrada, como clock, reset, dados) e observa as respostas do DUT;
- Dessa forma, você pode verificar se o circuito se comporta corretamente antes de gastar tempo e recursos na síntese e na gravação em hardware.

Exemplo simples de testbench para um somador:

```verilog
module tb;
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

## Icarus Verilog (`iverilog`)

## Verilator

## DigitalJS