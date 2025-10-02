# Tutorial SpinalHDL 
!!! note "Nota"
    Este tutorial é uma introdução ao SpinalHDL, uma linguagem de descrição de hardware baseada em Scala. Ele cobre os conceitos básicos, principais características e fornece exemplos práticos para ajudar os iniciantes a começar a usar o SpinalHDL em seus projetos de design de hardware.

## Introdução ao SpinalHDL
SpinalHDL é uma linguagem de descrição de hardware (HDL) baseada em Scala, projetada para facilitar o desenvolvimento de circuitos digitais complexos. Ela combina a expressividade e a flexibilidade do Scala com conceitos específicos de design de hardware, permitindo que os engenheiros criem designs mais rapidamente e com menos erros.

Foi criada por Christophe de Dinechin, um engenheiro de hardware e software com vasta experiência em design de circuitos digitais. O SpinalHDL é mantido pela SpinalCom, uma empresa especializada em soluções de design de hardware. O motivo para a criação do SpinalHDL foi a necessidade de uma linguagem de descrição de hardware mais moderna e eficiente, que pudesse aproveitar os avanços na programação funcional e orientada a objetos oferecidos pelo Scala. O SpinalHDL é amplamente utilizado em projetos de design de hardware, especialmente em aplicações que exigem alta performance e flexibilidade, como FPGAs e ASICs.

Aplicações criadas com SpinalHDL incluem processadores personalizados, controladores de memória, interfaces de comunicação e sistemas embarcados. Como exemplos de projetos notáveis, podemos citar o VexRiscv, um processador RISC-V open-source.

### Informações Adicionais
- [Documentação Oficial do SpinalHDL](https://spinalhdl.github.io/SpinalDoc-RTD/master/index.html)
- [Repositório no GitHub](https://github.com/spinalhdl/SpinalHDL)


## Preparando o Ambiente de Desenvolvimento
Para instalar o SpinalHDL, você precisará ter o Scala e o SBT (Scala Build Tool) instalados em sua máquina. Siga os passos abaixo:

1. **Instale o Scala**: Você pode baixar o Scala do site oficial [scala-lang.org](https://www.scala-lang.org/download/). Siga as instruções de instalação para o seu sistema operacional.
2. **Instale o SBT**: O SBT é a ferramenta de build para projetos Scala. Você pode baixar o SBT do site oficial [sbt](https://www.scala-sbt.org/download.html). Siga as instruções de instalação para o seu sistema operacional.
3. **Crie um novo projeto SBT**: Abra um terminal e crie um novo diretório para o seu projeto. Navegue até esse diretório e execute o comando `sbt new scala/scala-seed.g8` para criar um novo projeto Scala. SBT é a ferramenta padrão para construir projetos Scala, e usar um projeto SBT permite que você facilmente adicione o SpinalHDL como uma dependência, gerencie versões e compile seu código de forma eficiente.
4. **Adicione a dependência do SpinalHDL**: No arquivo `build.sbt` do seu projeto, adicione a seguinte linha para incluir o SpinalHDL como dependência:
   ```scala
   libraryDependencies += "com.github.spinalhdl" %% "spinalhdl-core" % "1.8.0"
   ```
5. **Atualize o projeto**: No terminal, dentro do diretório do seu projeto, execute o comando `sbt update` para baixar as dependências do SpinalHDL.    

6. **Verifique a instalação**: Crie um arquivo Scala simples para testar se o SpinalHDL está funcionando corretamente. Dentro da pasta `src/main/scala`, crie um arquivo `MeuModulo.scala` com o seguinte conteúdo:
   ```scala
   import spinal.core._

   class MeuModulo extends Component {
     val entrada = in Bool()
     val saida = out Bool()

     saida := entrada
   }

   object MeuModuloVerilog {
     def main(args: Array[String]): Unit = {
       SpinalVerilog(new MeuModulo)
     }
   }
   ```
   Execute o comando `sbt run` no terminal. Se tudo estiver configurado corretamente, o SpinalHDL gerará o código Verilog para o módulo `MeuModulo`.

## Fundamentos da Sintaxe
A sintaxe do SpinalHDL é baseada em Scala, o que significa que você pode usar todas as funcionalidades da linguagem Scala ao escrever seu código de hardware. Aqui estão alguns conceitos básicos da sintaxe do SpinalHDL:
1. **Componentes**: Em SpinalHDL, um componente é uma unidade básica de design de hardware. Você define um componente criando uma classe que estende a classe `Component`.
   ```scala
   class MeuComponente extends Component {
     // Definição do componente
   }
   ```
2. **Portas**: As portas de um componente são definidas usando os métodos `in` e `out`. Você pode definir portas de entrada e saída para o seu componente.
   ```scala
   val entrada = in Bool()  // Porta de entrada do tipo booleano
   val saida = out Bool()   // Porta de saída do tipo booleano
   ```
3. **Tipos Básicos**: SpinalHDL suporta vários tipos de dados, incluindo `Bool`, `UInt`, `SInt`, e `Bits`. Você pode especificar a largura dos tipos inteiros usando o método `bits`.
   ```scala
   val meuUInt = UInt(8 bits)  // Inteiro sem sinal de 8 bits
   val meuSInt = SInt(16 bits) // Inteiro com sinal de 16 bits
   val meuBits = Bits(4 bits)  // Vetor de bits de 4 bits
   ```
   
4. **Atribuições**: As atribuições em SpinalHDL são feitas usando o operador `:=`. Você pode atribuir valores às portas e sinais dentro do seu componente.
   ```scala
   saida := entrada  // Atribui o valor da entrada à saída
   ```
O operador `=` é usado para atribuições de valor em variáveis, enquanto `:=` é usado para atribuições em circuitos digitais, onde a ordem das operações e a propagação de sinais são importantes. O uso de `:=` deixa claro que estamos lidando com lógica de hardware, onde as atribuições podem ser sensíveis ao tempo e à ordem de execução.

5. **Bundles**: Bundles são usados para agrupar várias portas ou sinais em uma única unidade. Você pode definir um Bundle criando uma classe que estende a classe `Bundle`.
   ```scala
   class MeuBundle extends Bundle {
     val sinal1 = Bool()
     val sinal2 = UInt(8 bits)
   }
   ```
    Exemplo de um módulo simples usando Bundle:
    ```scala
    import spinal.core._
    class MeuModuloComBundle extends Component {
    val io = new Bundle {
        val entrada_sinal1 = in Bool()
        val entrada_sinal2 = in UInt(8 bits)
        val saida_sinal1 = out Bool()
        val saida_sinal2 = out UInt(8 bits)
    }

    io.saida_sinal1 := io.entrada_sinal1
    io.saida_sinal2 := io.entrada_sinal2
    }
    ```
6. **Controle de Fluxo**: SpinalHDL suporta estruturas de controle de fluxo como `if`, `else`, `while`, e `for`, permitindo que você crie lógica condicional e loops em seu design.
   ```scala
   when(entrada) {
     saida := True
   } otherwise {
     saida := False
   }
   ```
7. **Sinais**: Sinais intermediários podem ser declarados usando `val` ou `var`, dependendo se o valor é constante ou variável.
   ```scala
   val sinalIntermediario = Bool()
   sinalIntermediario := entrada & True
   ```
8. **Clock e Reset**: Você pode definir sinais de clock e reset usando `ClockDomain` e `Reset`. Isso é útil para designs síncronos.
   ```scala
   val clock = ClockDomain.current.clock
   val reset = ClockDomain.current.reset
   ```
9. **Memórias**: SpinalHDL oferece suporte para a criação de memórias, como RAM e ROM, usando classes específicas.
   ```scala
   val ram = Mem(UInt(8 bits), 256) // RAM de 256x8 bits
   ```
10. **Hierarquia de modulos**: Você pode instanciar outros componentes dentro de um componente para criar hierarquias de design.
    ```scala
    val meuOutroComponente = new MeuOutroComponente()
    ```
## Criando os Primeiros Módulos

### Adder Parametrizavel Simples
```scala
import spinal.core._

class AdderParametrizavel(val largura: Int) extends Component {
  val io = new Bundle {
    val a = in UInt(largura bits)
    val b = in UInt(largura bits)
    val soma = out UInt(largura bits)
  }

  io.soma := io.a + io.b
}
```
- `class AdderParametrizavel(val largura: Int) extends Component`: Define um componente chamado `AdderParametrizavel` que é parametrizado pela largura dos sinais de entrada e saída.
- `val io = new Bundle { ... }`: Cria um bundle de entradas e saídas para o componente.
- `io.soma := io.a + io.b`: Define a lógica do somador, que é a soma das entradas `a` e `b`.
#### Como esse modulo seria em Verilog

```verilog
module AdderParametrizavel #(
  parameter LARGURA = 8  // Parâmetro para largura
)(
  input  wire [LARGURA-1:0] a,
  input  wire [LARGURA-1:0] b,
  output wire [LARGURA-1:0] soma
);
  
  assign soma = a + b;
  
endmodule
```

### ULA Simples
```scala
import spinal.core._
class UlaSimples(val largura: Int) extends Component {
  val io = new Bundle {
    val a = in UInt(largura bits)
    val b = in UInt(largura bits)
    val operacao = in UInt(2 bits) // 00: add, 01: sub, 10: and, 11: or
    val resultado = out UInt(largura bits)
  }

  io.resultado := 0.U // Funciona como valor padrão

  switch(io.operacao) {
    is(0.U) { io.resultado := io.a + io.b } // Adição
    is(1.U) { io.resultado := io.a - io.b } // Subtração
    is(2.U) { io.resultado := io.a & io.b } // AND
    is(3.U) { io.resultado := io.a | io.b } // OR
  }
}
```
- `class UlaSimples(val largura: Int) extends Component`: Define um componente chamado `UlaSimples` que é parametrizado pela largura dos sinais de entrada e saída.
- `val io = new Bundle { ... }`: Cria um bundle de entradas e saídas para o componente.
- `switch(io.operacao) { ... }`: Implementa a lógica da ULA, realizando diferentes operações com base no valor da entrada `operacao`.

#### Como esse modulo seria em Verilog

```verilog
module UlaSimples #(
  parameter LARGURA = 8  // Parâmetro para largura
)(
  input  wire [LARGURA-1:0] a,
  input  wire [LARGURA-1:0] b,
  input  wire [1:0] operacao,
  output reg  [LARGURA-1:0] resultado
);

always @(*) begin
  case (operacao)
    2'b00: resultado = a + b; // Adição
    2'b01: resultado = a - b; // Subtração
    2'b10: resultado = a & b; // AND
    2'b11: resultado = a | b; // OR
    default: resultado = 0;
  endcase
end

endmodule
```
### Registrador com Enable e Reset
```scala
import spinal.core._

class RegistradorComEnableReset(val largura: Int) extends Component {
  val io = new Bundle {
    val d = in UInt(largura bits)
    val clk = in Bool()
    val reset = in Bool()
    val enable = in Bool()
    val q = out UInt(largura bits)
  }

  io.q := 0.U

  when(io.reset) {
    io.q := 0.U
  } .elsewhen(io.enable) {
    io.q := io.d
  }
}
```
- `class RegistradorComEnableReset(val largura: Int) extends Component`: Define um componente chamado `RegistradorComEnableReset` que é parametrizado pela largura dos sinais de entrada e saída.
- `val io = new Bundle { ... }`: Cria um bundle de entradas e saídas para o componente.
- `when(io.reset) { ... } .elsewhen(io.enable) { ... }`: Implementa a lógica do registrador, que pode ser resetado ou atualizado com o valor de `d` quando `enable` está ativo.
#### Como esse modulo seria em Verilog

```verilog
module RegistradorComEnableReset #(
  parameter LARGURA = 8  // Parâmetro para largura
)(
  input  wire [LARGURA-1:0] d,
  input  wire clk,
  input  wire reset,
  input  wire enable,
  output reg  [LARGURA-1:0] q
);

always @(posedge clk or posedge reset) begin
  if (reset) begin
    q <= 0;
  end else if (enable) begin
    q <= d;
  end
end

endmodule
```

## Criando Maquina de Estados Finitos (FSM)
A maquina de estados apresenta 6 estados (A, B, C, D, E, F) e 3 entradas de controle (start, stop, back). A transição entre os estados é controlada pelas entradas de controle da seguinte forma: 001 (start) move para o próximo estado, 010 ou 011 (stop) mantém o estado atual, e 100 (back) retorna ao estado anterior. A saída da FSM é o estado atual representado por um número de 3 bits (1 a 6).

```scala
import spinal.core._

class FsmABCF extends Component {
  val io = new Bundle {
    val ctrl   = in UInt(3 bits)   // sinal de controle
    val estado = out UInt(3 bits)  // saída (1..6)
  }

  // Registrador de estado, inicia no A (1)
  val estadoReg = RegInit(U(1, 3 bits))

  switch(estadoReg) {
    is(U(1)) { // Estado A
      when(io.ctrl === U"3'b001") { estadoReg := U(2) }   // start -> B
      when(io.ctrl === U"3'b100") { estadoReg := U(1) }   // back -> fica em A
    }
    is(U(2)) { // Estado B
      when(io.ctrl === U"3'b001") { estadoReg := U(3) }   // start -> C
      when(io.ctrl === U"3'b100") { estadoReg := U(1) }   // back -> A
    }
    is(U(3)) { // Estado C
      when(io.ctrl === U"3'b001") { estadoReg := U(4) }   // start -> D
      when(io.ctrl === U"3'b100") { estadoReg := U(2) }   // back -> B
    }
    is(U(4)) { // Estado D
      when(io.ctrl === U"3'b001") { estadoReg := U(5) }   // start -> E
      when(io.ctrl === U"3'b100") { estadoReg := U(3) }   // back -> C
    }
    is(U(5)) { // Estado E
      when(io.ctrl === U"3'b001") { estadoReg := U(6) }   // start -> F
      when(io.ctrl === U"3'b100") { estadoReg := U(4) }   // back -> D
    }
    is(U(6)) { // Estado F
      when(io.ctrl === U"3'b001") { estadoReg := U(6) }   // já é o último, fica
      when(io.ctrl === U"3'b100") { estadoReg := U(5) }   // back -> E
    }
  }

  // stop (010 ou 011) = não faz nada -> mantém estado
  io.estado := estadoReg
}

```
- `class FsmABCF extends Component`: Define um componente chamado `FsmABCF`.
- `val io = new Bundle { ... }`: Cria um bundle de entradas e saídas para o componente.
- `val estadoReg = RegInit(U(1, 3 bits))`: Declara um registrador de estado inicializado para o estado A (1).
- `switch(estadoReg) { ... }`: Implementa a lógica da máquina de estados, definindo as transições entre os estados com base na entrada `ctrl`.

#### Como esse modulo seria em Verilog
```verilog
module FsmABCF (
    input  wire       clk,
    input  wire       reset,    // reset síncrono
    input  wire [2:0] ctrl,     // 001=start, 010/011=stop, 100=back
    output reg  [2:0] estado
);

  // Definição dos estados (codificação binária simples)
  localparam A = 3'b001;
  localparam B = 3'b010;
  localparam C = 3'b011;
  localparam D = 3'b100;
  localparam E = 3'b101;
  localparam F = 3'b110;

  // Registrador de estado
  reg [2:0] estado_reg, estado_next;

  // Lógica de transição combinacional
  always @(*) begin
    estado_next = estado_reg; // padrão = mantém
    case (estado_reg)
      A: begin
        if (ctrl == 3'b001) estado_next = B; // start -> B
        else if (ctrl == 3'b100) estado_next = A; // back -> fica
      end
      B: begin
        if (ctrl == 3'b001) estado_next = C; // start -> C
        else if (ctrl == 3'b100) estado_next = A; // back -> A
      end
      C: begin
        if (ctrl == 3'b001) estado_next = D; // start -> D
        else if (ctrl == 3'b100) estado_next = B; // back -> B
      end
      D: begin
        if (ctrl == 3'b001) estado_next = E; // start -> E
        else if (ctrl == 3'b100) estado_next = C; // back -> C
      end
      E: begin
        if (ctrl == 3'b001) estado_next = F; // start -> F
        else if (ctrl == 3'b100) estado_next = D; // back -> D
      end
      F: begin
        if (ctrl == 3'b001) estado_next = F; // já é o último
        else if (ctrl == 3'b100) estado_next = E; // back -> E
      end
      default: estado_next = A; // segurança
    endcase
  end

  // Atualização do estado (flip-flop com reset síncrono)
  always @(posedge clk) begin
    if (reset)
      estado_reg <= A;
    else
      estado_reg <= estado_next;
  end

  // Saída = estado atual
  always @(*) begin
    estado = estado_reg;
  end

endmodule
```
