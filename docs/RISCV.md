# RISC-V 

Os processadores são ***hardware* de propósito geral**, controlados por um programa armazenado na memória, manipulando dados também contidos na memória. Apesar de serem ineficientes – se comparados aos circuitos projetados para um propósito específico – eles são extremamente versáteis, pois podemos desenvolver programas para resolver os mais diversos problemas e usar sempre o mesmo *hardware* para executá-los.

Um processador *softcore* é aquele que, descrito em uma HDL, pode ser implementado diretamente em um FPGA. Existem muitos *cores* disponíveis atualmente. Neste tutorial vamos apresentar algumas ferramentas e primeiros passos de alguns projetos usados no nosso grupo de pesquisa, baseados no processador RISC-V. 

Os processadores RISC (*Reduced Instruction Set Computer*) possuem instruções simples, que são combinadas para executar tarefas mais complexas. O diagrama abaixo fornece uma idéia básica de como o RISC-V funciona, segundo algumas premissas: 

- Instruções e Dados podem estar na mesma memória ([von Neumann](https://pt.wikipedia.org/wiki/Arquitetura_de_von_Neumann)) ou em memórias separadas ([Harvard](https://pt.wikipedia.org/wiki/Arquitetura_Harvard)) como no diagrama a seguir.
- Operações lógicas e artiméticas possuem três operandos em registradores e são executadas na ALU (*Arithmetic Logic Unit*);
    * Instruções do **Tipo R** recebem dois registradores (`rs1` e `rs2`) como entrada e gravam o resultado em um terceiro (`rd`); 
    * Instruções do **Tipo I** recebem um registador apenas (`rs1`) e um imediato codificado na própria instrução como entrada. 
- Instruções **Load** / **Store** são usadas para trazer / levar dados da / para a memória;

```mermaid
flowchart TD
    PC[PC] e1@==> |instr. address| IM; 
    e1@{ animate: true }
    IM[Instructions 
    Memory] ==> |instruction| ID([Instruction Decode]);
    ID --> |ra1| R;
    ID --> |ra2| R;
    ID --> |rd| R;
    R[Register
      Bank]
    ID --> |immediate| E;
    E@{ shape: manual-input, label: "extension"};
    E ==> MA;
    ID --> |funct| A
    R ==> |rs1| A;
    R ==> |rs2| MA;
    MA[\mux/] ==> A[\ALU/];
    A ==> |data address| DM[Data 
                       Memory];
    A ==> |write back| MR;
    MR[\mux/] ==> |wd3|R;
    R ==> |store| DM;
    DM ==> |load| MR;
```

Seu banco de registadores possui três portas independentes e pode ser implementado da seguinte maneira:

```verilog
module regfile(
    input clk, we3,            // write enable
    input [4:0] ra1, ra2, wa3, // rs1, rs2, rd (addr.)
    input [31:0] wd3,          // rd (data)
    output [31:0] rd1, rd2);   // rs1, rs2 (data)

  reg [31:0] rf [0:31];

  always@(posedge clk)
    if (we3) 
       rf[wa3] <= wd3;	

  assign rd1 = (ra1 != 0) ? rf[ra1] : 0;
  assign rd2 = (ra2 != 0) ? rf[ra2] : 0;
endmodule
```

Note que o registrador 0 (zero) é sempre zero, mas nesta implementação isso é tratado nas leituras ao invés da escrita. 

Se você quer se aprofundar na construção de um processador RISC-V, sugerimos este [excelente tutorial](https://github.com/BrunoLevy/learn-fpga/blob/master/FemtoRV/TUTORIALS/FROM_BLINKER_TO_RISCV/) do Prof. Bruno Levy.

## Geradores de SOCs

### Chipyard

### Litex

O framework LiteX fornece uma infraestrutura integrada para criar SoCs e integrar periféricos. Sendo capaz de criar sistemas completos baseados em FPGA.

```txt
                                      +---------------+
                                      |FPGA toolchains|
                                      +----^-----+----+
                                           |     |
                                        +--+-----v--+
                       +-------+        |           |
                       | Migen +-------->           |
                       +-------+        |   LiteX   +
              +----------------------+  |           |
              |LiteX Cores Ecosystem +-->           |
              +----------------------+  +-^-------^-+
               (Eth, SATA, DRAM, USB,     |       |
                PCIe, Video, etc...)      +       +
                                         board   target
                                         file    file
```

O LiteX fornece todos os componentes comuns necessários para criar facilmente um núcleo/SoC FPGA:

- Barramentos (Wishbone, AXI, Avalon-ST).
- Núcleos simples: RAM, ROM, Timer, UART, JTAG, etc.
- Núcleos complexos através do ecossistema de núcleos: LiteDRAM, LitePCIe, LiteEth, LiteSATA, etc...
- Suporte a linguagens mistas com recursos de integração VHDL/Verilog/Migen/Spinal-HDL/etc...
- Infraestrutura de depuração através de diversas Bridges ou Litescope.
- Simulação simplificada através do Verilator.
- SoC Linux multinúcleo baseado em CPU VexRiscv-SMP, LiteDRAM e LiteSATA (https://github.com/litex-hub/linux-on-litex-vexriscv).

Para trabalhar com códigos RISC-V é preciso compilar a toolchain gnu:
```sh
git clone https://github.com/riscv/riscv-gnu-toolchain && cd riscv-gnu-toolchain
sudo apt-get install autoconf automake autotools-dev curl python3 python3-pip python3-tomli libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev libslirp-dev
./configure --prefix=/opt/riscv --enable-multilib --enable-newlib --enable-linux --enable-debug-info --with-arch=rv32gc --with-abi=ilp32d
make -j $(nproc) && sudo make install
```

Para instalar o litex basta criar um python virtual environment e depois executar o script de instalação:
```sh
python3 -m venv litex-env
source litex-env/bin/activate
pip3 install ninja meson
```

```sh
mkdir litex && cd litex
wget https://raw.githubusercontent.com/enjoy-digital/litex/master/litex_setup.py
chmod +x litex_setup.py
./litex_setup.py --init --install --config standard
```
Executando `litex_sim --help` diversas opções interessantes podem ser encontradas para configuração do SoC:

Para executar uma simulação simples com vexriscv vamos instalar as dependências, o `verilator`, `sbt` e o `openjdk`.
```sh
sudo apt install verilator sbt openjdk
```

Gerar o SoC:
```sh
litex_sim --integrated-main-ram-size=0x10000 --cpu-type=vexriscv --no-compile-gateware
```

Para executar um código simples simulado como exemplo vamos executar o `donut`:
```sh
litex_sim --integrated-sram-size=0x2000 --integrated-main-ram-size=0x10000 --cpu-type=vexriscv --cpu-variant=full --no-compile-gateware
litex_bare_metal_demo --build-path=build/sim/
litex_sim --integrated-sram-size=0x2000 --integrated-main-ram-size=0x10000 --cpu-type=vexriscv --cpu-variant=full --ram-init=demo.bin
```

Um console como esse será iniciado o SoC irá executar o código carregado:
```sh
        __   _ __      _  __
       / /  (_) /____ | |/_/
      / /__/ / __/ -_)>  <
     /____/_/\__/\__/_/|_|
   Build your hardware, easily!

 (c) Copyright 2012-2025 Enjoy-Digital
 (c) Copyright 2007-2015 M-Labs

 BIOS built on Sep 21 2025 00:35:41
 BIOS CRC passed (160356a0)

 LiteX git sha1: 51e4f2e65

--=============== SoC ==================--
CPU:            VexRiscv @ 1MHz
BUS:            wishbone 32-bit @ 4GiB
CSR:            32-bit data
ROM:            128.0KiB
SRAM:           8.0KiB
MAIN-RAM:       64.0KiB
```

Para execução na FPGA vamos usar como exemplo a `Tang Primer 20k`. O download da toolchain pode ser feito no link:
- https://cdn.gowinsemi.com.cn/Gowin_V1.9.11.03_Education_Linux.tar.gz

```sh
mkdir -p gowin && cd gowin
wget https://cdn.gowinsemi.com.cn/Gowin_V1.9.11.03_Education_Linux.tar.gz
tar -xvf Gowin_V1.9.11.03_Education_Linux.tar.gz
export LD_PRELOAD=/usr/lib64/libfreetype.so.6 # ou /lib/x86_64-linux-gnu/libfreetype.so
```

Vamos compilar usando o módulo `litex-boards`, a lista de placas suportadas pode ser encontrada em:
- https://github.com/litex-hub/litex-boards

```sh
# python3 -m litex_boards.targets.<board> --help
python3 -m litex_boards.targets.sipeed_tang_primer_20k --build
litex_bare_metal_demo --build-path=build/sipeed_tang_primer_20k/
python3 -m litex_boards.targets.sipeed_tang_primer_20k --load
```

Os códigos para serem executados no Core podem ser compilados na árvore:
- https://github.com/enjoy-digital/litex/tree/master/litex/soc/software/demo

E carregados no SoC de variadas formas, vamos usar um exemplo via serial:
- https://github.com/enjoy-digital/litex/wiki/Load-Application-Code-To-CPU

```sh
litex_term /dev/ttyUSBX --kernel=demo.bin
```

## Cores

### LightRiscv

### VexRiscv
O VexRiscv é uma implementação RISC-V escrita em SpinalHDL, derivação do NaxRiscv. Dentre os recursos suportados estão:

- Conjunto de instruções RV32I[M][A][F[D]][C].
- Pipeline de 2 a 5+ estágios ([Buscar*X], Decodificar, Executar, [Memória], [Gravar]).
- Otimizado para FPGA, não utiliza nenhum bloco IP/primitivo específico do prorpietário.
- Otimizado para AXI4, Avalon e wishbone.
- Extensões MUL/DIV opcionais / FPU F32/F64 opcional / MMU opcional.
- Extensão de depuração opcional que permite a depuração do Eclipse por meio de uma conexão GDB >> openOCD >> JTAG
- Interrupções e tratamento de exceções opcionais com os modos de execução [Supervisor] e [Usuário], conforme definido no RISC-V Privileged ISA Especificação v1.10.
- Compatível com Linux (SoC: https://github.com/enjoy-digital/linux-on-litex-vexriscv)
- Ports para Zephyr e FreeRTOS.

Para gerar um core é preciso instalar as dependências, `verilator` (3.9+), `sbt` e o `openjdk` (8):
```sh
# JAVA JDK 8
sudo add-apt-repository -y ppa:openjdk-r/ppa
sudo apt-get update
sudo apt-get install openjdk-8-jdk -y
sudo update-alternatives --config java
sudo update-alternatives --config javac

# Install SBT - https://www.scala-sbt.org/
echo "deb https://repo.scala-sbt.org/scalasbt/debian all main" | sudo tee /etc/apt/sources.list.d/sbt.list
echo "deb https://repo.scala-sbt.org/scalasbt/debian /" | sudo tee /etc/apt/sources.list.d/sbt_old.list
curl -sL "https://keyserver.ubuntu.com/pks/lookup?op=get&search=0x2EE0EA64E40A89B84B2DF73499E82A75642AC823" | sudo apt-key add
sudo apt-get update
sudo apt-get install sbt

# Verilator (for sim only, really needs 3.9+, in general apt-get will give you 3.8)
sudo apt-get install git make autoconf g++ flex bison
git clone http://git.veripool.org/git/verilator   # Only first time
unsetenv VERILATOR_ROOT  # For csh; ignore error if on bash
unset VERILATOR_ROOT  # For bash
cd verilator
git pull        # Make sure we're up-to-date
git checkout v4.216
autoconf        # Create ./configure script
./configure
make
sudo make install
```

Basta agora gerar o SoC:
```sh
sbt "runMain vexriscv.demo.GenFull"
```

Vários cores pré-configurados como (Murax, Briey, Linux, GenFull, GenSmallest) podem ser encontrados em:
https://github.com/SpinalHDL/VexRiscv/tree/master/src/main/scala/vexriscv/demo

Existem vários plugins que podem ser adicionados e criados, alguns exemplos dos que já existem são:

- HazardSimplePlugin
- BranchPlugin
- MulPlugin
- DivPlugin
- CsrPlugin
- DebugPlugin
- EmbeddedRiscvJtag
- FpuPlugin


### NaxRiscv

O NaxRiscv é uma implementação RISC-V escrita em SpinalHDL. Dentre os recursos suportados estão:

- Execução fora de ordem com renomeação de registradores.
- Superscalar (ex: 2 decodificadores, 3 unidades de execução, 2 desativadas).
- (RV32/RV64)IMAFDCSU (Linux/Buildroot funciona em hardware).
- HDL portátil, mas FPGA com RAM distribuída.
- Elaboração de hardware descentralizada (nível superior vazio parametrizado com plugins).
- Frontend implementado em torno de uma estrutura de pipeline para facilitar a personalização.
- MMU com hardware reabastecido (SV32, SV39).
- Visualização do pipeline por meio de simulação do Verilator e Konata.
- Suporte a JTAG / OpenOCD / GDB implementando o RISCV External Debug Support v. 0.13.2

No exemplo que se segue vamos usar o `VexRiscv` uma derivação do `NaxRiscv`, para isso temos que instalar o `verilator`, `sbt` e o `openjdk`:
```sh
git clone https://github.com/SpinalHDL/NaxRiscv.git --recursive
cd NaxRiscv
export NAXRISCV=${PWD}
make install-toolchain
```
Basta configurar a env `PATH` corretamente e estamos prontos para executar uma simulação.

Para gerar o SoC:
```sh
export NAXRISCV=${PWD}
(cd ext/NaxSoftware && ./init.sh)
# Generate NaxRiscv
cd $NAXRISCV
sbt "runMain naxriscv.Gen"
```

Para executar a simulação é preciso compilar as dependências e depois o simulador:
```sh
# Install SDL2, allowing the simulation to display a framebuffer
sudo apt-get install libsdl2-2.0-0 libsdl2-dev

# Compile the simulator
cd $NAXRISCV/src/test/cpp/naxriscv
make compile
./obj_dir/VNaxRiscv
```

Para executar linux em uma simulação:
```sh
cd $NAXRISCV/src/test/cpp/naxriscv
export LINUX_IMAGES=$NAXRISCV/ext/NaxSoftware/buildroot/images/rv32ima
./obj_dir/VNaxRiscv \
    --load-bin $LINUX_IMAGES/fw_jump.bin,0x80000000 \
    --load-bin $LINUX_IMAGES/linux.dtb,0x80F80000 \
    --load-bin $LINUX_IMAGES/Image,0x80400000 \
    --load-bin $LINUX_IMAGES/rootfs.cpio,0x81000000 
```
