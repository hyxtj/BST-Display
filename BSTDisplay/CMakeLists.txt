cmake_minimum_required(VERSION 3.16)
project(QAppWithMusic LANGUAGES CXX)

if(MSVC)
    add_compile_options("/utf-8")
else()
    add_compile_options("-finput-charset=UTF-8" "-fexec-charset=UTF-8")
endif()

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
)

find_package(Qt6 REQUIRED COMPONENTS Multimedia)

qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    TreeNode.h
    TreeNode.cpp
    BSTWindow.h
    BSTWindow.cpp
    BSTWindow.ui
    BSTView.h
    BSTView.cpp
    BinarySearchTree.h
    BinarySearchTree.cpp
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt6::Multimedia
)
