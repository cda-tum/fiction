TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

HEADERS += \
    ../source/graph_search_iterator.h \
    ../source/undirected_graph.h \
    ../source/undirected_pair.h
