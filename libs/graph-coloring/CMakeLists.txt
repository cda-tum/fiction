CMAKE_MINIMUM_REQUIRED(VERSION 3.9)

PROJECT(graph-coloring)

SET(CMAKE_CXX_STANDARD 11)

ADD_LIBRARY(graph-coloring SHARED
    Source/coloring_algorithm.cpp
    Source/dsatur.cpp
    Source/mcs.cpp
    Source/lmxrlf.cpp
    Source/tabucol.cpp
    Source/hybrid_dsatur.cpp
    Source/hybrid_lmxrlf.cpp
)
