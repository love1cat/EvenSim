cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

SET(AppName evensim)

project(EvenSimProject)

# FIND_PACKAGE(boost)

INCLUDE_DIRECTORIES("~/Tools/IBM/ILOG/CPLEX_Studio1251/cplex/include")
INCLUDE_DIRECTORIES("~/Tools/IBM/ILOG/CPLEX_Studio1251/concert/include") 

link_directories("~/Tools/IBM/ILOG/CPLEX_Studio1251/cplex/lib/x86-64_sles10_4.1/static_pic/")
link_directories("~/Tools/IBM/ILOG/CPLEX_Studio1251/concert/lib/x86-64_sles10_4.1/static_pic/")

add_definitions(-DIL_STD)

add_executable(${AppName} alg_input.cc alg_input.h algbase.cc algbase.h
    datatypes.h exception.h geom.h
    grdalg_base.cc grdalg_base.h grdalg_generic.cpp
    grdalg_generic.h input.cc input.h limited_optalg.cc
    limited_optalg.h lp_alg.cc lp_alg.h main.cc output.cc output.h
    random_generator.cc random_generator.h simulation.h simulation.cpp
    enhance_algbase.h enhance_alg1.h enhance_alg1.cpp enhance_alg2.h
    enhance_alg2.cpp)

target_link_libraries(${AppName} ilocplex concert cplex m pthread)
      
