function z = wavey(x,y)
  r = sin(x^2 + y^2) + eps;
  z = sin(r) / r;
endfunction
