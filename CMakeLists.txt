# Copyright (c) 2017 Sébastien Rombauts (sebastien.rombauts@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT)

cmake_minimum_required(VERSION 2.8.12) # first version with add_compile_options()
project(VulkanTutorial C CXX)

# Backported FindPackages scripts from CMake 3.7.2 (for Boost 1.63) :
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Verbose Makefile
#set (CMAKE_VERBOSE_MAKEFILE ON)

# Print some standard CMake variables
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "CMAKE_SYSTEM_NAME/_VERSION      '${CMAKE_SYSTEM_NAME}' '${CMAKE_SYSTEM_VERSION}'")
message(STATUS "CMAKE_CXX_COMPILER_ID/_VERSION  '${CMAKE_CXX_COMPILER_ID}' '${CMAKE_CXX_COMPILER_VERSION}'")
if (NOT MSVC)
    message(STATUS "CMAKE_BUILD_TYPE                '${CMAKE_BUILD_TYPE}'")
endif (NOT MSVC)

# Define useful variables to handle OS/Compiler differences
if (MSVC)
    set(CPPLINT_ARG_OUTPUT      "--output=vs7")
    set(CPPCHECK_ARG_TEMPLATE   "--template=vs")
    set(DEV_NULL                "NUL")
    add_definitions(/D_CRT_SECURE_NO_WARNINGS)
else (MSVC)
    set(CPPLINT_ARG_OUTPUT      "--output=eclipse")
    set(CPPCHECK_ARG_TEMPLATE   "--template=gcc")
    set(DEV_NULL                "/dev/null")

    # C++11 :
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    
     # GCC/Clang flags
    add_compile_options(-rdynamic -fstack-protector)
endif (MSVC)
set(CPPLINT_ARG_LINELENGTH "--linelength=120")
set(CPPLINT_ARG_VERBOSE    "--verbose=1")


# Search for Vulkan SDK
message(STATUS "ENV{VULKAN_SDK}: '$ENV{VULKAN_SDK}'")
find_package(Vulkan)
if (NOT VULKAN_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else ()
    message(STATUS "Vulkan_INCLUDE_DIRS: '${Vulkan_INCLUDE_DIRS}' Vulkan_LIBRARIES: '${Vulkan_LIBRARIES}'" )
endif ()
include_directories(SYSTEM ${Vulkan_INCLUDE_DIRS})

# GLM (vector/matrix math) is a header only library, no need to build it, so not using add_subdirectory(glm)
include_directories(SYSTEM glm)

# Compole GLFW, before activation of maximum warnings bellow 
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
include_directories(SYSTEM glfw/include)
set(glfw_LIBRARIES glfw)


# List all source/header files
set(source_files
 ${CMAKE_SOURCE_DIR}/src/Main.cpp
 ${CMAKE_SOURCE_DIR}/src/HelloTriangleApplication.h
)
source_group(src      FILES ${source_files})

# List script files
set(script_files
 ${CMAKE_SOURCE_DIR}/.travis.yml
 ${CMAKE_SOURCE_DIR}/appveyor.yml
 ${CMAKE_SOURCE_DIR}/build.bat
 ${CMAKE_SOURCE_DIR}/build.sh
 ${CMAKE_SOURCE_DIR}/Doxyfile
)
source_group(scripts  FILES ${script_files})

# List doc files
set(doc_files
 ${CMAKE_SOURCE_DIR}/README.md
 ${CMAKE_SOURCE_DIR}/LICENSE.txt
)
source_group(doc      FILES ${doc_files})

# add the application executable
add_executable(VulkanTutorial ${source_files} ${doc_files} ${script_files} ${examples_files})
target_link_libraries(VulkanTutorial ${glfw_LIBRARIES} ${Vulkan_LIBRARIES})


# Optional additional targets:

option(RUN_CPPLINT "Run cpplint.py tool for Google C++ StyleGuide." ON)
if (RUN_CPPLINT)
    find_package(PythonInterp 2.6)
    if (PYTHONINTERP_FOUND)
        if(EXISTS "${PROJECT_SOURCE_DIR}/cpplint/cpplint.py")
            # add a cpplint target to the "all" target
            add_custom_target(cpplint
             ALL
             COMMAND python ${PROJECT_SOURCE_DIR}/cpplint/cpplint.py ${CPPLINT_ARG_OUTPUT} ${CPPLINT_ARG_LINELENGTH} ${CPPLINT_ARG_VERBOSE} ${source_files}
            )
        else ()
            message(STATUS "cpplint submodule missing")
        endif ()
    else ()
        message(STATUS "Python not found, not runing cpplint")
    endif (PYTHONINTERP_FOUND)
else (RUN_CPPLINT)
    message(STATUS "RUN_CPPLINT OFF")
endif (RUN_CPPLINT)

option(RUN_CPPCHECK "Run cppcheck C++ static analysis tool." ON)
if (RUN_CPPCHECK)
    find_program(CPPCHECK_EXECUTABLE NAMES cppcheck)
    if (CPPCHECK_EXECUTABLE)
        # add a cppcheck target to the "all" target
        add_custom_target(cppcheck
         ALL
         COMMAND cppcheck -j 4 --enable=style --suppress=unusedPrivateFunction --quiet ${CPPCHECK_ARG_TEMPLATE} ${PROJECT_SOURCE_DIR}/src
        )
    else (CPPCHECK_EXECUTABLE)
        message(STATUS "Could NOT find cppcheck")
    endif (CPPCHECK_EXECUTABLE)
else (RUN_CPPCHECK)
    message(STATUS "RUN_CPPCHECK OFF")
endif (RUN_CPPCHECK)

option(RUN_DOXYGEN "Run Doxygen C++ documentation tool." ON)
if (RUN_DOXYGEN)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # add a Doxygen target to the "all" target
        add_custom_target(doxygen
         ALL
         COMMAND doxygen Doxyfile > ${DEV_NULL}
         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        )
    endif (DOXYGEN_FOUND)
else (RUN_DOXYGEN)
    message(STATUS "RUN_DOXYGEN OFF")
endif (RUN_DOXYGEN)

