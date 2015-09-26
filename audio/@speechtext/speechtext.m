function st = speechtext (id)
  if (nargin != 1)
    error("usage: speechtext id");
  end
  
  function part = getpart(id, partname)
    persistent program = "/home/zos/robo/.whee/programs/native/audio/get_speechtext";
    cmd = strjoin({program, num2str(id), partname}, " ");
    [status, part] = system(cmd);
    if (status != 0)
      error([cmd, ": ", status]);
    end
  end
  
  if (strcmp(class(id), "speechtext"))
    st = id;
  else
    st.id = id;
    st.written = getpart(id, "written");
    raw_spoken = getpart(id,"spoken");
    int16_spoken = typecast(raw_spoken, "int16");
    st.spoken = double(int16_spoken) / 2**15;
    st = class(st, "speechtext");
  end
end