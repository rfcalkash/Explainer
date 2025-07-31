#!/bin/bash
grep -E '^(## [0-9]+\. .*|[^#].*)' words.txt | \
awk '
BEGIN {
  print "{"
  print "  \"data\": ["
}
/^## / {
  if (name != "") {
    print "      \"words\": [" words "]"
    print "    },"
  }
  name = substr($0, 5)
  sub(/^[0-9]+\. /, "", name)
  name = "\"" name "\""
  words = ""
  print "    {"
  print "      \"name\": " name ","
  next
}
{
  split($0, a, ", ")
  for (i=1; i<=length(a); i++) {
    words = words "\"" a[i] "\""
    if (i < length(a)) {
      words = words ", "
    }
  }
}
END {
  print "      \"words\": [" words "]"
  print "    }"
  print "  ]"
  print "}"
}
' | jq .