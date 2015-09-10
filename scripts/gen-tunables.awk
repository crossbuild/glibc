BEGIN {
  ns=""
  top_ns=""
}

$2 == "{" {
  if (top_ns == "") {
    top_ns = $1
  }
  else if (ns == "") {
    ns = $1
  count = 0
  }
  else {
    printf ("Unexpected occurrence of '{' inside a namespace: %s:%d\n",
	    FILENAME, FNR)
    exit 1
  }

  next
}

$1 == "}" {
  if (ns != "") {
    ns = ""
  }
  else if (top_ns != "") {
    top_ns = ""
  }
  else {
    printf ("syntax error: extra }: %s:%d\n", FILENAME, FNR)
    exit 1
  }
  next
}

{
  if (ns == "") {
    print "Invalid tunable outside a namespace"
    exit 1
  }
  val[top_ns][ns][count] = $1
  count = count + 1
}

END {
  if (ns != "") {
    print "Unterminated namespace.  Is a closing brace missing?"
    exit 1
  }

  print "/* Print a full tunable enum name.  */"
  print "#define TUNABLE_ENUM_NAME(top,ns,id) TUNABLE_ENUM_NAME1 (top,ns,id)"
  print "#define TUNABLE_ENUM_NAME1(top,ns,id) top ## _ ## ns ## _ ## id\n"

  print "/* Full name for a tunable is top_ns.tunable_ns.id.  */"
  print "#define TUNABLE_NAME_S(top,ns,id) #top \".\" #ns \".\" #id\n"

  print "typedef enum"
  print "{"
  for (t in val) {
    for (n in val[t]) {
      for (c in val[t][n]) {
        printf ("  TUNABLE_ENUM_NAME(%s, %s, %s),\n", t, n, val[t][n][c]);
      }
    }
  }
  print "} tunable_id_t;\n"
  print "#ifdef TUNABLES_INTERNAL"
  print "static tunable_t tunable_list[] = {"
  for (t in val) {
    for (n in val[t]) {
      for (c in val[t][n]) {
        printf ("  {TUNABLE_NAME_S(%s, %s, %s), NULL, NULL, false},\n",
		t, n, val[t][n][c], t, n, val[t][n][c]);
      }
    }
  }
  print "};"
  printf "#endif"
}
