module structural_mux2x1 (
  input a, b, sel,
  output y);
  
  wire not_sel, and_a, and_b;
  not (not_sel, sel);
  and (and_a, a, not_sel);
  and (and_b, b, sel);
  or (y, and_a, and_b);
endmodule