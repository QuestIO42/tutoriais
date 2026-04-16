#include <stdlib.h>
#include <iostream>
#include "verilated_vcd_c.h"
#include "verilated.h"
#include "Vadder.h"

int main(int argc, char **argv) {
    Vadder *dut = new Vadder;
    Verilated::traceEverOn(true);
    VerilatedVcdC *m_trace = new VerilatedVcdC;
    dut->trace(m_trace, 0);
    m_trace->open("dump.vcd");    
    for (int i = 0; i < 10; i++) {
        dut->a = rand() % 16;
        dut->b = rand() % 16;
        dut->eval();
        m_trace->dump(i);
    }
    m_trace->close();
    delete dut;
    return 0;
}
