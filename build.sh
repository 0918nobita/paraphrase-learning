#!/bin/sh -ev

g++ -std=c++17 -Wall -Wextra -lboost_program_options -o para main.cpp
