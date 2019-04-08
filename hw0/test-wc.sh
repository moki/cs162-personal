#!/bin/sh

subject="./wc.out"

subject_args="main.c"

test='"$subject" "$subject_args"'

eval "$test"

expected='wc "$subject_args"'

eval "$expected"

