# A Importância da Verificação Funcional: Por que testar é tão crucial quanto projetar

No desenvolvimento de hardware, existe um ditado famoso: **"Se você não o verificou, ele não funciona"**.

A verificação funcional é o processo de confirmar que o seu design (o "Design Under Test" ou DUT) faz exatamente o que ele foi especificado para fazer, em todas as condições possíveis.

Para um projeto de ULA de 32 bits, por exemplo, isso não significa apenas rodar uma simulação rápida para ver se 1+1=2. Significa provar, metodicamente, que o seu design está correto. A importância disso se resume a três pilares: Custo, Complexidade e Confiança.

## 1. O Custo do Erro

Em design de hardware, bugs não são fáceis de corrigir.

    Em ASICs (Chips Customizados): Um bug funcional encontrado após a fabricação (tape-out) pode custar milhões de dólares e meses de atraso para um respin (uma nova versão do chip).

    Em FPGAs: Embora corrigível, um bug que chega ao cliente final pode exigir recalls de produtos, atualizações de firmware complexas e, o pior de tudo, destruir a reputação da sua empresa.

A verificação funcional é a "rede de segurança" que captura esses bugs devastadores antes que eles saiam do ambiente de simulação.

## 2. O Desafio da Complexidade (Por que "Simular" não é o bastante)

Um design de 32 bits é muito mais complexo do que parece. A ULA em questão tem duas entradas de 32 bits. Isso significa que existem 2³²×2³²=2⁶⁴ combinações de entrada possíveis... apenas para a soma.

É literalmente impossível testar todas as combinações manualmente (isso levaria bilhões de anos).

Uma simulação simples (chamada de happy path ou "caminho feliz") pode testar 10+5 ou 100/10. Mas a verificação funcional se preocupa com o que acontece nos extremos, os casos de canto (corner cases), que é onde 99% dos bugs se escondem.

## 3. O Papel da Verificação: Confiança através da Adversidade

A verificação funcional muda a mentalidade de "Como eu mostro que funciona?" para "Como eu tento quebrar isso?".

No contexto de um projeto de ULA de 32 bits (unsigned), o ambiente de verificação é fundamental para levantar e responder questões que um teste simples jamais abordaria:

    Soma (ADD): O que acontece quando você soma 32'hFFFFFFFF + 32'h1? O resultado deve ser 32'h00000000 e um Carry deve ser gerado. O seu design trata esse carry corretamente? Ou ele simplesmente dá um resultado errado?

    Subtração (SUB): O que acontece em 32'h0 - 32'h1? Isso é um underflow. O resultado deve ser 32'hFFFFFFFF e um Borrow (empréstimo) deve ser sinalizado. Seu design faz isso?

    Multiplicação (MUL): O que acontece ao multiplicar 32'hFFFFFFFF * 32'hFFFFFFFF? O resultado real tem 64 bits. O seu design trunca o resultado para 32 bits? Ele armazena corretamente os 64 bits (nos registradores Hi e Lo, por exemplo)? Se você não verificar isso, pode estar produzindo resultados massivamente incorretos em multiplicações grandes sem nem saber.

    Divisão (DIV): Este é o caso clássico de exceção. O que acontece se B for zero (A/0)? O design trava? Ele entra em um loop infinito? Ou ele sinaliza um flag de "Divisão por Zero", como deveria?


## 4. O Valor do Ambiente de Verificação

A verificação funcional não é um passo opcional; em muitos projetos, ela consome mais de 70% do tempo total de desenvolvimento.

Ter um ambiente de verificação permite que você automatize a busca por corner cases. Você pode rodar milhões de transações aleatórias durante a noite e ter a certeza de que seu Golden Model (a "fonte da verdade") pegará qualquer discrepância, garantindo que a ULA que você projetou é robusta, correta e pronta para o mundo real.

## 5. Mão na Massa: Construindo um Ambiente de Verificação para uma ULA de 32 bits

Não esquecer de escrever a intrução.

### 5.1. DUT (Design Under Test): A ULA em Verilog que queremos testar

Não esquecer de explicar essa parte. Modelo simples unsigned e com apenas 4 operações, não é uma ULA usada realmente.


```verilog
// Interface da nossa ULA (Design Under Test)
module alu_32_bit (
    input clk,
    input rst,

    // Entradas
    input [1:0]  opcode_in, // 00:ADD, 01:SUB, 10:MUL, 11:DIV
    input [31:0] A_in,
    input [31:0] B_in,
    
    // Saídas
    output reg [31:0] result_out_low,
    output reg [31:0] result_out_hi,
    output reg carry, // 1 se result_out_low > 0xFFFFFFFF
    output reg borrow, // 1 se result_out_low < 0
    output reg error_out  // 1 se B=0 na divisão
);

    // O 'coração' da ULA (Implementação simplificada)
    always @(posedge clk) begin
        if (rst) begin
            result_out_low <= 32'd0;
            result_out_hi <= 32'd0;
            carry <= 1'b0;
            borrow <= 1'b0;
            error_out  <= 1'b0;
        end else begin
            // Reseta os erros
            carry <= 1'b0;
            borrow <= 1'b0;
            error_out <= 1'b0;

            case (opcode_in)
                // ADD: {Carry, Soma}
                2'b00: {carry, result_out_low} <= A_in + B_in;
                
                // SUB: {Borrow, Subtração}
                2'b01: {borrow, result_out_low} <= A_in - B_in;

                // MUL: Resultado de 64 bits
                2'b10: {result_out_hi, result_out_low} <= A_in * B_in;

                // DIV: {Resto, Quociente}
                2'b11: begin
                    if (B_in == 32'd0) begin
                        error_out <= 1'b1; // Erro: Divisão por zero!
                        result_out_low <= 32'hX; // Indefinido
                        result_out_hi <= 32'hX; // Indefinido
                    end else begin
                        result_out_low <= A_in / B_in; // Quociente
                        result_out_hi <= A_in % B_in; // Resto
                    end
                end
                
                default: begin
                    result_out_low <= 32'hX;
                    result_out_hi <= 32'hX;
                end
            endcase
        end
    end

endmodule
```

### 5.2. Tabela de Verificação (Corner Cases)

| Operação        | Entradas críticas              | O que verificar                             |
|-----------------|--------------------------------|---------------------------------------------|
| **Soma**        | `0xFFFFFFFF + 1`               | `carry = 1`, resultado = `0`                |
| **Soma**        | `0 + 0`                        | resultado = `0`                             |
| **Subtração**   | `A < B`                        | `borrow = 1`                                |
| **Subtração**   | `A - A`                        | resultado = `0`                             |
| **Multiplicação** | `0xFFFFFFFF * 0x2`           | truncamento correto                         |
| **Multiplicação** | `0xFFFFFFFF * 0xFFFFFFFF`    | bits altos na saída HI corretamente         |
| **Divisão**     | `x / 0`                        | flag ou comportamento definido              |
| **Divisão**     | `0 / x`                        | resultado = `0`                             |
| **Divisão**     | `x / 1`                        | resultado = `x`                             |
| **Divisão**     | `x / x`                        | resultado = `1`                             |
| **Divisão**     | `A / B em que A < B`           | verificar o valor do resto                  |

Também serão gerados 10.000 estímulos aleatórios, além dos casos de canto (corner cases), com o objetivo de avaliar o comportamento da ULA em condições intermediárias e garantir uma cobertura funcional mais ampla.

### 5.3. Ambiente de Verificação baseado na Metodologia UVM (Universal Verification Methodology)

Não entrar em detalhes sobre a metodologia, isso daria um curso inteiro.
Deixar o link para o repositório do código.

#### 5.3.1. Golden Model

#### 5.3.2. Sequencer

#### 5.3.3. Scoreboard

#### 5.3.4. Coverage

### 5.4. Resultados

Mostrar as waveforms para cada corner case.

## Considerações Finais