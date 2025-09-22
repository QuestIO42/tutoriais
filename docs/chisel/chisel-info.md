# Chisel

### Do Chisel ao Verilog: o fluxo de geração de hardware

O Chisel é uma linguagem de descrição de hardware *embedded* em Scala, ou seja, todo código escrito em Chisel é, na prática, um programa Scala que, quando executado, constrói uma árvore de hardware em memória. Esse processo funciona em diferentes etapas.

#### 1. Escrita do código em Chisel (Scala)  
Na etapa de escrita do código, o desenvolvedor descreve módulos, entradas, saídas e lógica combinacional ou sequencial usando classes e objetos Scala. Um exemplo simples seria um somador parametrizável, definido como uma classe que herda de `Module` e especifica seu `io`.

#### 2. Elaboração (Elaboration)  
Em seguida, ocorre a elaboração (elaboration). Quando o programa Scala é executado (via `sbt run` ou um teste), o Chisel interpreta a construção do hardware e gera uma representação intermediária chamada FIRRTL (*Flexible Intermediate Representation for RTL*). Nesse ponto, o hardware já está descrito em nível de registradores e conexões, mas ainda de forma mais abstrata que o Verilog.

#### 3. Transformações em FIRRTL
O compilador FIRRTL aplica então uma série de transformações e otimizações, que incluem verificações de tipo, simplificação de expressões e inferência de larguras de sinais. Esse processo garante que o circuito seja consistente, sem ambiguidades e pronto para síntese.

#### 4. Emissão de Verilog (Backend) 
Após esses passes, o FIRRTL é convertido em Verilog RTL, a linguagem padrão compreendida pelas ferramentas de síntese de FPGA e ASIC. O código gerado está pronto para ser utilizado em simuladores Verilog, como o Verilator, ou em ferramentas de síntese como Quartus, Vivado, Synopsys e Cadence.

#### 5. Integração com fluxo de síntese  
Por fim, o Verilog gerado pode ser integrado ao fluxo de síntese. Ele pode ser simulado para validação funcional, sintetizado para FPGA (mapeado em LUTs, flip-flops e blocos DSP) ou para ASIC (mapeado em portas da biblioteca de células padrão).

Em resumo, o fluxo consiste em escrever o código em Scala/Chisel, executar o programa para gerar FIRRTL, aplicar as transformações necessárias e emitir Verilog pronto para uso em fluxos de FPGA e ASIC.

---

### Benefícios do uso de uma linguagem de alto nível

O uso de uma linguagem de descrição de hardware de alto nível como o Chisel traz benefícios significativos em comparação às HDLs tradicionais.  

Uma das principais vantagens é que grande parte da complexidade de projeto pode ser tratada ainda na fase de elaboração, antes da geração do Verilog. Nesse estágio, o desenvolvedor pode explorar recursos como parametrização, reuso de código, composição modular e até mesmo metaprogramação em Scala para descrever circuitos de forma mais abstrata e expressiva.  

Um exemplo emblemático é o **Diplomacy**, utilizado no ecossistema Chipyard. Esse recurso resolve automaticamente a configuração e interconexão de barramentos complexos como TileLink, ajustando larguras, protocolos e topologias sem que o projetista precise lidar diretamente com fios e sinais de baixo nível.  

Dessa forma, o projetista trabalha em um nível conceitual mais próximo da arquitetura do sistema, enquanto o compilador Chisel/FIRRTL se encarrega de expandir essas descrições em Verilog detalhado. O resultado é uma redução significativa de erros, maior rapidez na exploração de alternativas de projeto e a possibilidade de construir sistemas complexos de maneira mais eficiente e confiável.

Outro benefício importante está na separação clara entre a descrição do comportamento e a elaboração do hardware. Ao projetista cabe descrever **o que o circuito deve fazer** em um nível mais abstrato, usando a linguagem de alto nível para capturar a lógica e a estrutura do sistema. À ferramenta especializada (FIRRTL e backends do Chisel) cabe a tarefa de **como gerar** o Verilog final, aplicando otimizações, inferências de largura e transformações que muitas vezes seriam trabalhosas ou propensas a erro se feitas manualmente. 

Dessa forma, o Verilog produzido tende a ser mais consistente, otimizado e menos sujeito a falhas humanas, permitindo que o projetista concentre esforços no comportamento funcional e arquitetural, enquanto a ferramenta assegura uma implementação robusta em baixo nível.

[Motivação ⟶](motivacao.md)

