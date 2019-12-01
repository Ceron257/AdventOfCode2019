cmake_minimum_required(VERSION 3.14)

project(AdventOfCode)

get_filename_component(inputPath ./inputs ABSOLUTE)
add_definitions(-DINPUTS_PATH="${inputPath}")

add_subdirectory(day1)