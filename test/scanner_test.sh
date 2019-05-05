#!/usr/bin/env bash

function scanner_test() {
  echo "Testing $1"
  ./scanner test/data/$1 2>&1 >/dev/null
  diff output.txt test/result/$2
}


scanner_test "sc.c" "sc.txt"
scanner_test "mc.c" "mc.txt"
scanner_test "prep.c" "prep.txt"
scanner_test "flot.c" "flot.txt"
scanner_test "inte.c" "inte.txt"
scanner_test "iden.c" "iden.txt"
scanner_test "char.c" "char.txt"
scanner_test "str.c" "str.txt"
