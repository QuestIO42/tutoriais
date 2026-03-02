module behavioral_mux2x1 (
  input a, b, sel,
  output reg y);

  always @(*) 
    if (sel)
      y = b;
    else
      y = a;
endmodule