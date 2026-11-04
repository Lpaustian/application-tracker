cmake_minimum_required(VERSION 3.21)
project(application_tracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql Test)
qt_standard_project_setup()
set(CMAKE_AUTORCC ON)

add_library(tracker_core STATIC
    src/application.h
    src/logic.h
    src/logic.cpp
    src/applicationstore.h
    src/applicationstore.cpp
    src/applicationmodel.h
    src/applicationmodel.cpp
    src/editdialog.h
    src/editdialog.cpp
    src/datepicker.h
    src/datepicker.cpp
    src/style.h
    src/style.cpp
    src/applicationdelegate.h
    src/applicationdelegate.cpp
)
target_include_directories(tracker_core PUBLIC src)
target_link_libraries(tracker_core PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)

qt_add_executable(tracker WIN32
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    resources.qrc
)
target_link_libraries(tracker PRIVATE tracker_core Qt6::Widgets)

add_executable(tst_logic tests/tst_logic.cpp)
target_link_libraries(tst_logic PRIVATE tracker_core Qt6::Test)

add_executable(tst_store tests/tst_store.cpp)
target_link_libraries(tst_store PRIVATE tracker_core Qt6::Test)

add_executable(tst_model tests/tst_model.cpp)
target_link_libraries(tst_model PRIVATE tracker_core Qt6::Test)

add_executable(tst_editdialog tests/tst_editdialog.cpp)
target_link_libraries(tst_editdialog PRIVATE tracker_core Qt6::Test)

add_executable(tst_datepicker tests/tst_datepicker.cpp)
target_link_libraries(tst_datepicker PRIVATE tracker_core Qt6::Test)

enable_testing()
add_test(NAME logic COMMAND tst_logic)
add_test(NAME store COMMAND tst_store)
add_test(NAME model COMMAND tst_model)
add_test(NAME editdialog COMMAND tst_editdialog)
add_test(NAME datepicker COMMAND tst_datepicker)
