add_platform{
  name = "native",
  lib_path = "-L/usr/local/lib",
  flags = "",
};

add_platform{
  name = "coverage",
  lib_path = "-L/usr/local/lib",
  flags = "--coverage",
};

add_platform{
  name="zubu",
  tool_prefix="x86_64-zubu-linux-gnu-",
};

add_platform{
  name="zapi",
  tool_prefix="arm-zapi-linux-gnueabihf-",
};
