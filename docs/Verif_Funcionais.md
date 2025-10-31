# A Importância da Verificação Funcional: Por que testar é tão crucial quanto projetar

No desenvolvimento de hardware, existe um ditado famoso: **"Se você não o verificou, ele não funciona"**.

A verificação funcional é o processo de confirmar que o seu design (o "Design Under Test" ou DUT) faz exatamente o que ele foi especificado para fazer, em todas as condições possíveis.

Para um projeto de ULA de 32 bits, por exemplo, isso não significa apenas rodar uma simulação rápida para ver se 1+1=2. Significa provar, metodicamente, que o seu design está correto. A importância disso se resume a três pilares: Custo, Complexidade e Confiança.

## 1. O Custo do Erro

Em design de hardware, bugs não são fáceis de corrigir.

- **Em ASICs (Chips Customizados)**: Um bug funcional encontrado após a fabricação (tape-out) pode custar milhões de dólares e meses de atraso para um respin (uma nova versão do chip).

- **Em FPGAs**: Embora corrigível, um bug que chega ao cliente final pode exigir recalls de produtos, atualizações de firmware complexas e, o pior de tudo, destruir a reputação da sua empresa.

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

Depois de entender por que a verificação é a etapa mais crítica e demorada no design de hardware, vamos colocar a mão na massa. Nesta seção, construiremos um ambiente de verificação completo para uma ULA de 32 bits.

Usaremos uma abordagem que combina Cocotb (para simulação em Python) e pyuvm (uma implementação Python da Universal Verification Methodology - UVM).

Nosso objetivo é criar um testbench automatizado, reutilizável e auto-verificável que:

1. Use um DUT (Design Under Test) escrito em Verilog.

2. Defina um plano de verificação focado em **corner cases**.

3. Implemente um **Golden Model** em Python como nossa "fonte da verdade".

4. Use um **Sequencer** (UVM) para gerar estímulos (tanto os corner cases quanto milhares de testes aleatórios).

5. Use um **Scoreboard** (UVM) para comparar automaticamente os resultados do DUT com os do Golden Model.

6. Colete **Coverage** (UVM) para garantir que todos os nossos cenários de teste foram, de fato, executados.

### 5.1. DUT (Design Under Test): A ULA em Verilog que queremos testar

O primeiro passo é ter o design que queremos testar. O nosso DUT é o módulo **alu_32_bit** escrito em Verilog.

É fundamental entender que este é um modelo educacional simplificado. Ele não é uma ULA de nível de produção. Suas principais características (e limitações) são:

- Opera apenas com números unsigned (sem sinal). Uma ULA real implementaria aritmética de complemento de dois.

- Possui apenas 4 operações básicas, selecionadas pela entrada opcode_in:
    - `2'b00`: **ADD** (Soma)

    - `2'b01`: **SUB** (Subtração)

    - `2'b10`: **MUL** (Multiplicação)

    - `2'b11`: **DIV** (Divisão)

- A Multiplicação (**MUL**) produz um resultado de 64 bits, que é dividido entre as saídas **result_out_hi** (32 bits mais significativos) e **result_out_low** (32 bits menos significativos).

- A Divisão (DIV) retorna o Quociente em **result_out_low** e o Resto (módulo) em **result_out_hi**.

- Possui saídas de status dedicadas: **carry** (para estouro de soma), **borrow** (para estouro de subtração) e **error_out** (especificamente para divisão por zero).


```verilog
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
            result_out_low <= 32'd0;
            result_out_hi <= 32'd0;
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

Antes de escrever qualquer código de teste, precisamos de um plano de verificação. A parte mais importante desse plano é identificar os "casos de canto" (corner cases) — os cenários extremos onde os bugs adoram se esconder.

Como pensar para montar esta tabela? O processo mental é:

1. **Limites (Zeros)**: O que acontece quando as entradas são zero? (0+0, A-A, A*0, 0/B, B/0).

2. **Limites (Máximos)**: O que acontece quando as entradas são o valor máximo (0xFFFFFFFF)?

3. **Estouros (Overflow/Underflow)**: O que acontece quando o resultado ultrapassa os 32 bits? (0xFFFFFFFF + 1). O que acontece quando subtraímos um número maior de um menor? (0 - 1).

4. **Casos Especiais (Identidades)**: Quais são as identidades matemáticas? (A / 1, A / A).

5. **Exceções (Erros)**: Quais operações são ilegais? (A / 0).

Com base nesse pensamento, foi montada a seguinte tabela de casos de teste direcionados:

| Operação        | Entradas críticas              | O que verificar                             |
|-----------------|--------------------------------|---------------------------------------------|
| **Soma**        | `0xFFFFFFFF + 1`               | `carry = 1`, resultado = `0`                |
| **Soma**        | `0 + 0`                        | resultado = `0`                             |
| **Subtração**   | `A < B`                        | `borrow = 1`                                |
| **Subtração**   | `A - A`                        | resultado = `0`                             |
| **Multiplicação** | `0xFFFFFFFF * 0x2`           | truncamento correto                         |
| **Multiplicação** | `0xFFFFFFFF * 0xFFFFFFFF`    | bits altos na saída HI corretamente         |
| **Divisão**     | `x / 0`                        | flag ou comportamento definido              |
| **Divisão**     | `0 / x`                        | resto = `0`, resultado = `0`                |
| **Divisão**     | `x / 1`                        | resto = `0`, resultado = `x`                |
| **Divisão**     | `x / x`                        | resto = `0`, resultado = `1`                |
| **Divisão**     | `A / B em que A < B`           | resto = `A`, resultado = `0`                |

Além desses casos de canto direcionados, também geraremos 10.000 estímulos aleatórios. O objetivo dos testes aleatórios é avaliar o comportamento da ULA em condições intermediárias (ex: 273648 * 87263) que nosso cérebro não pensaria em testar, garantindo uma cobertura funcional muito mais ampla.

### 5.3. Ambiente de Verificação baseado na Metodologia UVM (Universal Verification Methodology)

Não vamos construir nosso testbench "do zero" ou de forma ad-hoc. Embora os componentes sejam escritos "na mão" em Python, faremos isso adotando a arquitetura da UVM (Universal Verification Methodology). A UVM é o padrão da indústria para criar ambientes de verificação robustos, modulares e reutilizáveis.

Isso nos dá uma arquitetura padronizada com componentes claros: **Sequencer** (gera dados), **Driver** (dirige o DUT), **Monitor** (coleta dados), **Scoreboard** (verifica dados) e **Coverage** (mede o progresso).

Não entraremos nos detalhes profundos da metodologia, pois isso preencheria um curso inteiro. O código-fonte completo deste ambiente está disponível no repositório abaixo.

Repositório: https://github.com/MarceloDaEnc/ALU_Verification

Vamos analisar os 4 componentes Python mais importantes:

#### 5.3.1. Golden Model

Este é o nosso oráculo, a nossa "fonte da verdade". É uma função em Python que replica perfeitamente o comportamento esperado da ULA, bit a bit. Note como ele usa máscaras (**MASK_32**) para simular a aritmética de 32 bits do hardware, tratando estouros (carry, borrow) e casos de 64 bits (multiplicação) exatamente como o Verilog deveria fazer.

O Scoreboard usará este modelo para comparar a saída do DUT.

```python
def alu_32_bit_golden_model(opcode_in, A_in, B_in):
    MASK_32 = 0xFFFFFFFF
    A = A_in & MASK_32
    B = B_in & MASK_32
    
    # Inicialização das saídas
    result_out_low = 0
    result_out_hi = 0
    carry = 0
    borrow = 0
    error_out = 0
    
    # ---------------------------------
    # Simulação do 'case (opcode_in)'
    # ---------------------------------
    
    if opcode_in == 0b00:  # ADD: {carry, result_out_low} <= A_in + B_in
        full_sum = A + B
        result_out_low = full_sum & MASK_32
        carry = (full_sum >> 32) & 1
        
    elif opcode_in == 0b01:  # SUB: {borrow, result_out_low} <= A_in - B_in
        result_out_low = (A - B) & MASK_32
        borrow = 1 if A < B else 0
        
    elif opcode_in == 0b10:  # MUL: {result_out_hi, result_out_low} <= A_in * B_in
        full_mul = A * B
        result_out_low = full_mul & MASK_32
        result_out_hi = (full_mul >> 32) & MASK_32
        
    elif opcode_in == 0b11:  # DIV: {Resto, Quociente}
        if B == 0:
            # Erro: Divisão por zero
            error_out = 1
            result_out_low = None 
            result_out_hi = None
        else:
            result_out_low = (A // B) & MASK_32
            result_out_hi = (A % B) & MASK_32
            
    else:  # default:
        result_out_low = None
        result_out_hi = None

    # Retorna um dicionário para facilitar a verificação
    return {
        "result_out_low": result_out_low,
        "result_out_hi": result_out_hi,
        "carry": carry,
        "borrow": borrow,
        "error_out": error_out
    }
```

#### 5.3.2. Sequencer

O Sequencer é o cérebro do testbench. Ele é responsável por gerar as transações de teste (**SeqItem**) e enviá-las ao **Driver**. Nossa sequência principal, **ULACoverageSeq**, faz duas coisas:

1. Primeiro, ela envia manualmente (um por um) todos os 11 corner cases que identificamos em nossa tabela de verificação.

2. Depois, ela entra em um loop e gera 10.000 transações com valores de A, B e opcode totalmente aleatórios.

```python

import random
from pyuvm import uvm_sequence
from .seq_item import SeqItem

class ULACoverageSeq(uvm_sequence):
    """
    Generates a sequence that covers all bins in CoverageBins.
    """
    async def body(self):

        item = SeqItem("carry_sum", 4294967295, 1, 0, 0)
        await self.start_item(item)
        await self.finish_item(item)

        item = SeqItem("sum_of_zeros", 0, 0, 0, 1)
        await self.start_item(item)
        await self.finish_item(item)

        item = SeqItem("negative_subtraction", 0, 1, 1, 2)
        await self.start_item(item)
        await self.finish_item(item)

        item = SeqItem("self_subtraction", 10, 10, 1, 3)
        await self.start_item(item)
        await self.finish_item(item)

        item = SeqItem("mul_truncation", 4294967295, 2, 2, 4)
        await self.start_item(item)
        await self.finish_item(item)

        item = SeqItem("mul_full_precision", 4294967295, 4294967295, 2, 5)
        await self.start_item(item)
        await self.finish_item(item)

        item = SeqItem("div_by_zero", 7, 0, 3, 6)
        await self.start_item(item)
        await self.finish_item(item)

        item = SeqItem("div_zero_numerator", 0, 10, 3, 7)
        await self.start_item(item)
        await self.finish_item(item)

        item = SeqItem("div_by_one", 8, 1, 3, 8)
        await self.start_item(item)
        await self.finish_item(item)

        item = SeqItem("div_self", 20, 20, 3, 9)
        await self.start_item(item)
        await self.finish_item(item)

        item = SeqItem("div_small_numerator", 10, 20, 3, 10)
        await self.start_item(item)
        await self.finish_item(item)

        for _ in range (10000):
            item = SeqItem("random", random.randint(0, 4294967295), random.randint(0, 4294967295), random.randint(0, 3), 0)
            await self.start_item(item)
            await self.finish_item(item)
```

#### 5.3.3. Scoreboard

O Scoreboard é o juiz. Ele recebe as transações que foram enviadas ao DUT e também os resultados que foram coletados do DUT.

Seu trabalho é, para cada transação:

1. Pegar os dados de entrada (item.opcode, item.A, item.B).

2. Calcular o resultado esperado usando o alu_32_bit_golden_model.

3. Pegar o resultado real que veio do DUT (actual_out_low, actual_out_hi, ...).

4. Comparar os dois.

5. Imprimir **PASSED** ou **FAILED** e contar o número de falhas.

Ele também trata de forma inteligente os valores **'X'** (indefinidos) do Verilog, que ocorrem na divisão por zero, comparando-os com o None do Python.

```python
import cocotb
from pyuvm import *
from .utils import alu_32_bit_golden_model

class Scoreboard(uvm_component):
    """
    Compares expected results with actual DUT results.
    """
    def build_phase(self):
        self.cmd_fifo = uvm_tlm_analysis_fifo("cmd_fifo", self)
        self.result_fifo = uvm_tlm_analysis_fifo("result_fifo", self)
        self.cmd_get_port = uvm_get_port("cmd_get_port", self)
        self.result_get_port = uvm_get_port("result_get_port", self)
        self.cmd_export = self.cmd_fifo.analysis_export
        self.result_export = self.result_fifo.analysis_export
        self.fail_count = 0

    def connect_phase(self):
        self.cmd_get_port.connect(self.cmd_fifo.get_export)
        self.result_get_port.connect(self.result_fifo.get_export)

    async def run_phase(self):
        while True:
            item = await self.cmd_get_port.get()
            (actual_out_low, actual_out_hi, actual_carry, actual_borrow, actual_error) = await self.result_get_port.get()
            if (actual_out_low == 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx' or actual_out_hi == 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx' or actual_carry == 'x' or actual_borrow == 'x' or actual_error == 'x'):
                actual_out_low = actual_out_hi = None
                actual_carry = 0
                actual_borrow = 0
                actual_error = 1
            else:
                actual_out_low   = int(actual_out_low)
                actual_out_hi    = int(actual_out_hi)
                actual_carry     = int(actual_carry)
                actual_borrow    = int(actual_borrow)
                actual_error     = int(actual_error)
            golden_outputs = alu_32_bit_golden_model(item.opcode, item.A, item.B)
            if (golden_outputs["result_out_low"] == actual_out_low and golden_outputs["result_out_hi"] == actual_out_hi and golden_outputs["carry"] == actual_carry
                and golden_outputs["borrow"] == actual_borrow and golden_outputs["error_out"] == actual_error):
                self.logger.info(f"✅ PASSED: {item}")
            else:
                self.logger.error(f"❌ FAILED: {item}")
                self.logger.error(
                    f"    └─ Expected: Result_Low={golden_outputs["result_out_low"]}, Result_Hi={golden_outputs["result_out_hi"]}, Carry={golden_outputs["carry"]}, Borrow={golden_outputs["borrow"]}, Error={golden_outputs["error_out"]}\n"
                    f"       Got:      Result_Low={actual_out_low}, Result_Hi={actual_out_hi}, Carry={actual_carry}, Borrow={actual_borrow}, Error={actual_error}"
                )
                self.fail_count += 1

    
    def report_phase(self):
        """Prints a final summary of the test results."""
        cocotb.log.info(f"\n+--------------------+")
        cocotb.log.info(f"| Final Fail Count: {self.fail_count:d} |")
        cocotb.log.info(f"+--------------------+")
        if self.fail_count > 0:
            assert False, f"{self.fail_count} failures detected in scoreboard"
```

#### 5.3.4. Coverage

Este é o componente final e crucial. Ele responde à pergunta: "Nós testamos tudo o que planejamos testar?".

Ele não se importa se o teste passou ou falhou (esse é o trabalho do Scoreboard). Ele apenas rastreia quais tipos de transações (nossos corner cases) foram executados.

Ele "escuta" os itens que passam e os adiciona a um **set()** do Python. No final da simulação (**report_phase**), ele compara o set de itens que vimos com a lista **CoverageBins** de itens que queríamos ver. Se algum corner case não foi executado, o teste falha, mesmo que todos os outros 10.000 testes aleatórios tenham passado.

```python
from pyuvm import *
from .seq_item import SeqItem
from .defs import Operation

CoverageBins = [
    Operation.CARRY_SUM,
    Operation.SUM_OF_ZEROS,
    Operation.NEGATIVE_SUBTRACTION,
    Operation.SELF_SUBTRACTION,
    Operation.MUL_TRUNCATION,
    Operation.MUL_FULL_PRECISION,
    Operation.DIV_ZERO_NUMERATOR,
    Operation.DIV_BY_ZERO,
    Operation.DIV_BY_ONE,
    Operation.DIV_SELF,
    Operation.DIV_SMALLER_NUMERATOR
]

class Coverage(uvm_subscriber):
    """
    Collects and verifies functional coverage for the PIE environment.
    """
    def end_of_elaboration_phase(self):
        self.cvg = set()

    def write(self, item):
        if isinstance(item, SeqItem):
            coverage_bin = (
                item.type
            )
            self.cvg.add(coverage_bin)

    def report_phase(self):
        try:
            disable_errors = ConfigDB().get(self, "", "DISABLE_COVERAGE_ERRORS")
        except UVMConfigItemNotFound:
            disable_errors = False

        if not disable_errors:
            coverage_bins_set = set(CoverageBins)
            missed_bins = coverage_bins_set - self.cvg
            if len(missed_bins) > 0:
                self.logger.error("Functional coverage error!")
                self.logger.error(f"  -> Bins not covered: {missed_bins}")
                assert False
            else:
                self.logger.info("✅ Functional coverage reached all bins.")
                assert True
```

### 5.4. Resultados

A execução do ambiente de verificação gera logs detalhados, como mostra o snippet abaixo. Podemos ver o **Scoreboard** validando com **✅ PASSED** cada uma das 10.011 transações (os 11 corner cases e as 10.000 aleatórias), o **Coverage** reportando que todos os bins (casos de canto) foram atingidos, e o sumário final do **cocotb** indicando **Final Fail Count: 0**.

```bash    
5200780.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0x696B5381 | B: 0x1663E737 | OpCode: 0     
5201300.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0x984E1486 | B: 0x7CD362B5 | OpCode: 3     
5201820.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0xF86AFB3C | B: 0xE33138D6 | OpCode: 2     
5202340.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0xF096B0F8 | B: 0xE1B58BD  | OpCode: 2     
5202860.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0xF0AAF75D | B: 0xF587E1D9 | OpCode: 1     
5203380.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0x1D81C37B | B: 0xA7DDF220 | OpCode: 0     
5203900.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0x9908E520 | B: 0x16818DDC | OpCode: 3     
5204420.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0x1494B710 | B: 0xEBEBBA8C | OpCode: 2     
5204940.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0x54CBDF2  | B: 0xE4378B68 | OpCode: 3     
5205460.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0x738B957A | B: 0x1B4552B4 | OpCode: 0     
5205980.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0x6F26EB06 | B: 0x7EB17FA6 | OpCode: 3     
5206500.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0x2D9E521A | B: 0x2687A9F  | OpCode: 1     
5207020.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0x909AC5DA | B: 0xAE02F006 | OpCode: 2     
5207540.00ns INFO     ..ULA/components/scoreboard.py(40) [uvm_test_top.env.scoreboard]: ✅ PASSED: A: 0xFF926233 | B: 0xA8DE4009 | OpCode: 0     
5307540.00ns INFO     ..M_ULA/components/coverage.py(47) [uvm_test_top.env.coverage]: ✅ Functional coverage reached all bins.
5307540.00ns INFO     cocotb                             
                                                         +--------------------+
5307540.00ns INFO     cocotb                             | Final Fail Count: 0 |
5307540.00ns INFO     cocotb                             +--------------------+
5307540.00ns INFO     cocotb.regression                  ULATest passed
5307540.00ns INFO     cocotb.regression                  **************************************************************************************
                                                         ** TEST                          STATUS  SIM TIME (ns)  REAL TIME (s)  RATIO (ns/s) **
                                                         **************************************************************************************
                                                         ** tests.test_ULA.ULATest         PASS     5307540.00           6.77     784140.29  **
                                                         **************************************************************************************
                                                         ** TESTS=1 PASS=1 FAIL=0 SKIP=0            5307540.00           6.82     778361.96  **
                                                         **************************************************************************************

```

Além da saída de log, a simulação também gera um arquivo **alu_32_bit.fst**. Este arquivo de waveform (forma de onda) permite uma inspeção visual ciclo-a-ciclo do comportamento do DUT.

Para analisar visualmente os corner cases (e qualquer outra transação), provando que o DUT se comportou exatamente como o Golden Model previu, o arquivo **alu_32_bit.fst** pode ser acessado no repositório do projeto e aberto usando um visualizador de waveforms gratuito, como o GtkWave.

## Considerações Finais

Nesta jornada, passamos da filosofia da verificação ("Por que testar?") para a prática ("Como testar?"). Construímos um ambiente de verificação robusto e moderno usando Python, Cocotb e a metodologia UVM.

Os pilares deste ambiente foram:

1. Um **Golden Model** em Python, que atua como a "fonte da verdade" inquestionável.

2. Um **Sequencer** que combina testes direcionados (os corner cases) com a força bruta de milhares de testes aleatórios.

3. Um **Scoreboard** automatizado, que atua como um juiz incansável, comparando o DUT com o Golden Model a cada ciclo.

4. Uma medição de **Coverage**, que garante que não apenas testamos muito, mas que testamos as coisas certas.

Ao rodar mais de 10.000 testes e focar especificamente nos pontos de falha (overflow, underflow, divisão por zero, limites de 64 bits), alcançamos um nível de confiança no design que seria impossível de obter com simulações manuais simples.

Isso demonstra o princípio central da verificação: não basta provar que o design funciona no "caminho feliz"; é preciso provar, metodicamente, que ele não falha em nenhum dos milhares de "caminhos infelizes". Só então podemos afirmar que o design está, de fato, correto.