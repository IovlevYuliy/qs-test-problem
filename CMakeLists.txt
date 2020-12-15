cmake_minimum_required(VERSION 3.5)

project(treeview LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(INCLUDES include/tree-item.h
             include/treemodel.h
             include/mainwindow.h)

set(SOURCES main.cpp
            source/tree-item.cpp
            source/treemodel.cpp
            source/mainwindow.cpp
            source/mainwindow.ui)
add_executable(treeview ${SOURCES} ${INCLUDES})

target_link_libraries(treeview PRIVATE Qt5::Widgets)
