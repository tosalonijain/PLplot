# findlib META file for ocaml-plplot
requires = ""
description = "PLplot library bindings"
version = "@VERSION@"
browse_interfaces = " Plplot "
linkopts = "-ccopt \"-L@SHLIB_DIR@\""
archive(byte) = "plplot.cma"
archive(native) = "plplot.cmxa"

