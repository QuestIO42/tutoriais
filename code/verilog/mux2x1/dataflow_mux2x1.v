module dataflow_mux2x1 (
  input a, b, sel,
  output y);
  
  assign y = a & ~sel | b & sel;
  
  // Alternativamente:
// assign y = sel ? b : a;

endmodule