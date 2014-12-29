project(qamster)
cmake_minimum_required(VERSION 2.8.12)
find_package(Qt5Core 5.2.1 REQUIRED)
find_package(Qt5Widgets 5.2.1 REQUIRED)
find_package(Qt5Sql 5.2.1 REQUIRED)
find_package(Qt5PrintSupport 5.2.1 REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(qamster_SRCS
  src/activitycompletermodel.cpp
  src/activityitemdelegate.cpp
  src/editdialog.cpp
  src/helperfunctions.cpp
  src/histogram.cpp
  src/category-histogram.cpp
  src/history.cpp
  src/qamster.cpp
  src/settings.cpp
  src/timedatabase.cpp
  src/timetablemodel.cpp
  src/timetableview.cpp
  src/main.cpp
  
  lib/qcustomplot.cpp
)
SET(qamster_FORMS
  ui/qamster.ui
  ui/editdialog.ui
  ui/history.ui
  ui/settings.ui
)
QT5_WRAP_UI(qamster_FORMS_HDRS ${qamster_FORMS})

list(APPEND CMAKE_CXX_FLAGS " -std=c++11 -march=native -Wall -pedantic -Wextra -Wundef -Wcast-align -Wwrite-strings -Wunreachable-code -Winit-self ")

add_executable(qamster ${qamster_SRCS}
		       ${qamster_FORMS_HDRS})
target_link_libraries(qamster ${QT_QTCORE_LIBRARY} Qt5::Widgets Qt5::Sql Qt5::PrintSupport)
install(TARGETS qamster RUNTIME DESTINATION bin)
