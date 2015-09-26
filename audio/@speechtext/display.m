function display (p)
  id = p.id;
  written = p.written;
  spoken = p.spoken;
  fprintf("%s = %d. %s (%d)\n", inputname(1), id, written, length(spoken) / 12000);
endfunction
