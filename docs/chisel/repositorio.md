# Repositório

Todo o código apresentado ao longo deste tutorial está disponível em um **repositório template** preparado para facilitar o uso. (link)

Esse repositório já contém:  

- Estrutura básica de projeto em **Chisel**.  
- Configuração do **sbt** com todas as dependências necessárias.  
- Exemplos de módulos e testbenches em **ScalaTest**.  

O objetivo é permitir que o leitor possa **clonar o repositório e começar a programar imediatamente**, sem precisar se preocupar com a configuração manual do ambiente.  

> **Nota:** Como o repositório foi configurado como *template*, você pode criar o seu próprio projeto clicando em **"Use this template"** diretamente no GitHub. Isso gera um repositório pessoal com a mesma estrutura, pronto para ser adaptado aos seus experimentos.

## Ferramentas utilizadas

Fazemos o uso de um conjunto de ferramentas já consolidadas no ecossistema do Chisel:

- **Chisel 6**: linguagem de descrição de hardware de alto nível, embutida em Scala, que permite descrever circuitos digitais de forma parametrizável, reutilizável e expressiva. É a base para todas as implementações apresentadas.
- **sbt (Scala Build Tool)**: ferramenta de automação de build para projetos Scala. É utilizada para compilar o código, gerenciar dependências e executar testes de forma integrada.
- **ScalaTest**: biblioteca de testes que possibilita escrever testbenches de maneira clara e estruturada, permitindo validar o comportamento dos módulos de hardware descritos em Chisel.

> Aviso importante

> Durante o desenvolvimento e execução dos exemplos apresentados neste tutorial, é possível que ocorram **erros de compilação, execução ou configuração**. Isso faz parte natural do processo de aprendizado ao lidar com linguagens de descrição de hardware e ferramentas de automação.

> Longe de ser um obstáculo, esses erros devem ser vistos como **oportunidades de aprendizado**. Encorajamos o leitor a investigar as mensagens de erro, consultar a documentação, revisar o código e aplicar correções. Esse processo fortalece a familiaridade com o projeto, melhora a compreensão das ferramentas e aproxima a experiência prática do que ocorre em projetos reais de hardware.

## Testbench em Scala/Chisel

Em Chisel, os testes são escritos em **Scala** utilizando o **ScalaTest** junto com a biblioteca **chiseltest**. A estrutura típica é colocar os arquivos de teste em `src/test/scala/`, nomeando-os como `*Spec.scala`. Cada teste instancia o DUT (*Device Under Test*), aplica estímulos nas portas (`poke`), avança o relógio (`clock.step`) e verifica as saídas (`expect`).

