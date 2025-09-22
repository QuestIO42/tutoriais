## 4. Somadores e Subtratores

### 4.1 Somadores de 1 bit a 16 bits

Para iniciar a exploração prática em Chisel, vamos construir um somador a partir do bloco mais básico: o **Full Adder** (somador completo de 1 bit). Em seguida, mostraremos como reutilizar esse bloco para formar um somador de 16 bits.

O código em Chisel se assemelha a uma mistura entre Scala (linguagem de alto nível) e operadores de hardware. Cada parte do código tem um papel específico na descrição do circuito.

```scala
// Definição de um somador completo de 1 bit
class FullAdder extends Module {
  // A palavra-chave "extends Module" indica que essa classe descreve um módulo de hardware
  val io = IO(new Bundle {
    // Entradas: dois bits (a e b) e um bit de carry-in (cin)
    val a    = Input(Bool())
    val b    = Input(Bool())
    val cin  = Input(Bool())
    // Saídas: soma (sum) e carry-out (cout)
    val sum  = Output(Bool())
    val cout = Output(Bool())
  })

  // Descrição da lógica: XOR para a soma
  io.sum  := io.a ^ io.b ^ io.cin
  // Expressão booleana para o carry-out
  io.cout := (io.a & io.b) | (io.a & io.cin) | (io.b & io.cin)
}
```

- **`class FullAdder`** cria uma nova classe que representa o circuito.  
- **`extends Module`** indica que essa classe descreve um **módulo de hardware** em Chisel.  
- **`val io = IO(new Bundle {...})`** define as portas de entrada e saída do módulo.  
- **`Input(Bool())`** e **`Output(Bool())`** representam fios de 1 bit.  
- Os operadores **`^`**, **`&`** e **`|`** descrevem lógica combinacional (XOR, AND, OR).  
- O símbolo **`:=`** é usado para conectar sinais em Chisel (atribuição de hardware).  

A partir desse bloco básico, podemos montar um **somador de 16 bits**, encadeando 16 instâncias do `FullAdder`.


```scala
// Somador de 16 bits construído a partir de FullAdders
class Adder16 extends Module {
  val io = IO(new Bundle {
    val a   = Input(UInt(16.W))   // entrada de 16 bits
    val b   = Input(UInt(16.W))   // entrada de 16 bits
    val cin = Input(Bool())       // carry-in inicial
    val sum = Output(UInt(16.W))  // saída de 16 bits
    val cout= Output(Bool())      // carry-out final
  })

  // Vetores auxiliares (wires) para propagar os valores de soma e carry
  val sums  = Wire(Vec(16, Bool()))
  val carry = Wire(Vec(17, Bool()))
  carry(0) := io.cin

  // Instancia 16 Full Adders, um para cada bit
  for (i <- 0 until 16) {
    val fa = Module(new FullAdder())
    fa.io.a   := io.a(i)
    fa.io.b   := io.b(i)
    fa.io.cin := carry(i)
    sums(i)   := fa.io.sum
    carry(i+1):= fa.io.cout
  }

  // Conecta o resultado final
  io.sum  := sums.asUInt
  io.cout := carry(16)
}
```

- **`UInt(16.W)`** define um fio com largura de 16 bits.  
- **`Wire(Vec(...))`** cria vetores de sinais intermediários, aqui usados para armazenar somas e a propagação do carry.  
- **`for (i <- 0 until 16)`** mostra como Chisel permite usar estruturas de controle da linguagem Scala para **gerar hardware repetitivo**, evitando escrever manualmente 16 instâncias de `FullAdder`.  
- A função **`asUInt`** converte o vetor de bits individuais em um único sinal de 16 bits.  

### 4.2 Somadores completos parametrizados

No somador de 16 bits, a largura era fixa no código. Para torná-lo mais flexível, podemos parametrizar o número de bits utilizando um argumento na definição da classe. Assim, o mesmo código pode gerar somadores de qualquer largura.

```scala
class Adder(val nBits: Int) extends Module {
```

Nesta linha a largura do somador passa a ser definida pelo parâmetro nBits, informado na instanciação do módulo, em vez de estar fixada em 16 bits.

```scala
val sums  = Wire(Vec(nBits, Bool()))
val carry = Wire(Vec(nBits + 1, Bool()))
```

Aqui os vetores auxiliares sums e carry assumem tamanhos variáveis de acordo com nBits. O vetor carry precisa de uma posição extra, já que a propagação do carry gera uma saída adicional.

```scala
for (i <- 0 until nBits) {
```

O laço de repetição percorre exatamente nBits posições, instanciando automaticamente o número correto de FullAdders conforme a largura especificada.


Para instanciar um somador de 32 bits a partir da classe parametrizada, basta escrever:

```scala
val adder32 = Module(new Adder(32))
```

### 4.3 Operadores Aritméticos de Alto Nível

Um ponto importante a destacar é o uso preferencial dos operadores de alto nível, como `+` e `-`, para implementar soma e subtração. Em **FPGAs**, esses operadores são automaticamente mapeados pelas ferramentas de síntese em blocos internos altamente otimizados, como *carry chains* e unidades aritméticas dedicadas, que fazem parte da lógica intrínseca do dispositivo. Isso garante implementações eficientes em termos de área e desempenho, sem exigir que o projetista especifique manualmente a arquitetura do somador. Já no caso de **ASICs**, a situação é diferente: não existem blocos aritméticos pré-definidos, sendo necessário escolher explicitamente a arquitetura a ser utilizada (ripple-carry, carry-lookahead, Kogge-Stone, entre outras). Essa escolha envolve um trade-off complexo entre área, latência e consumo de energia, e abre espaço para uma discussão muito mais profunda — que foge ao escopo deste tutorial.

A partir deste ponto, utilizaremos preferencialmente os operadores aritméticos de alto nível (`+` e `-`) para implementar somadores e subtratores. Por exemplo, a classe `CombParamSInt` pode ser vista como uma versão avançada do uso direto de `+` e `-`, pois além de realizar soma e subtração, também verifica automaticamente se houve **overflow** durante as operações.
 
```scala
  val aExt = io.a.pad(nBits + 1) // SInt(nBits+1)
  val sumWide = Wire(SInt((nBits + 1).W))
  sumWide := aExt +& bExt
  io.OutSum      := sumWide(nBits - 1, 0).asSInt
  io.addOverflow := (sumWide(nBits) ^ sumWide(nBits - 1)).asBool
```

- **`pad(nBits + 1)`**  
  Extende o operando `SInt` em **1 bit adicional**, preservando o sinal (sign-extend). Isso cria “folga” para capturar corretamente situações de overflow sem perder o bit de sinal.  

- **`Wire(SInt((nBits + 1).W))`**  
  Cria um fio temporário (`sumWide`) com largura expandida em **nBits + 1**, onde será armazenado o resultado da operação antes de ser truncado.  

- **`+&` (soma com expansão)**  
  Diferente do operador `+`, o `+&` preserva o bit extra gerado pela operação, garantindo que o resultado não perca a informação de overflow.  

- **`sumWide(nBits - 1, 0).asSInt`**  
  Seleciona apenas os **nBits menos significativos** do resultado expandido, convertendo de volta para o tamanho original do módulo. Esse é o valor real da soma que será exposto em `io.OutSum`.  

- **`(sumWide(nBits) ^ sumWide(nBits - 1)).asBool`**  
  Faz o XOR entre os dois bits mais significativos do resultado (`MSB extra` e `MSB real`), técnica padrão para detecção de **overflow em operações com sinal**. O resultado lógico (`Bool`) é disponibilizado em `io.addOverflow`.
