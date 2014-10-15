cmake_minimum_required(VERSION 2.6)
project(qamster)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(qamster_SRCS qamster.cpp main.cpp)

qt4_automoc(${qamster_SRCS})
add_executable(qamster ${qamster_SRCS})
target_link_libraries(qamster ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS qamster RUNTIME DESTINATION bin)
