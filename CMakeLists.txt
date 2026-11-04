cmake_minimum_required(VERSION 3.21)
project(application_tracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql Test)
qt_standard_project_setup()

add_library(tracker_core STATIC
    src/application.h
    src/logic.h
    src/logic.cpp
    src/applicationstore.h
    src/applicationstore.cpp
    src/applicationmodel.h
    src/applicationmodel.cpp
)
target_include_directories(tracker_core PUBLIC src)
target_link_libraries(tracker_core PUBLIC Qt6::Core Qt6::Gui Qt6::Sql)

add_executable(tst_logic tests/tst_logic.cpp)
target_link_libraries(tst_logic PRIVATE tracker_core Qt6::Test)

add_executable(tst_store tests/tst_store.cpp)
target_link_libraries(tst_store PRIVATE tracker_core Qt6::Test)

add_executable(tst_model tests/tst_model.cpp)
target_link_libraries(tst_model PRIVATE tracker_core Qt6::Test)

enable_testing()
add_test(NAME logic COMMAND tst_logic)
add_test(NAME store COMMAND tst_store)
add_test(NAME model COMMAND tst_model)
