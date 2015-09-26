function [inputf, outputf] = makesamples(S)
  persistent sample_width = 120;
  
  input = zeros(1,sample_width);
  output = zeros(1,sample_width);
  
  for i = 1:length(S)
    if (mod(i,10))
      disp(i);
    end
    s = S{i};
    n = length(s);
    for j = 1:sample_width:n
      input_begin = j + fix(rand()*10);
      input_end = input_begin - 1 + sample_width;
      output_begin = j + fix(rand()*10);
      output_end = output_begin - 1 + sample_width;
      if (input_end > n || output_end > n)
        continue;
      end
      input_sample = s(input_begin:input_end);
      output_sample = s(output_begin:output_end);
      input = [input; input_sample];
      output = [output; output_sample];
    end
  end
  inputf = input;
  outputf = output;
  
end
