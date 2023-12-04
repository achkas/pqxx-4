cmake_minimum_required (VERSION 3.20)

set (postgreSQL_additional_versions"16")

project (testbasepsol)

add_subdirectory ("D:\\libpqxx-7.8.1" libpqxx-BUILD)

add_executable (testbasepsol main.cpp)

target_compile_features (testbasepsol PRIVATE cxx_std_17)

target_link_libraries (testbasepsol PRIVATE pqxx)



