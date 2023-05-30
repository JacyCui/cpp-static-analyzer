#!/bin/zsh
clang -cc1 -analyze -analyzer-checker=debug.DumpCFG main.cpp
