cmake_minimum_required(VERSION 3.23)
project(PocoHttpServer)

find_package(Poco REQUIRED Foundation Net Util)
set(CMAKE_CXX_STANDARD 20)

add_executable(PocoHttpServer main.cpp)
target_link_libraries(PocoHttpServer Poco::Foundation Poco::Net Poco::Util Poco::XML Poco::JSON)