cmake_minimum_required (VERSION 3.0)

PROJECT(lguitest)

set (CMAKE_CXX_FLAGS "-Wall -W -Wextra -O2 -std=c++14")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src/lib)

set (sources_lgui_test
src/tests/all_tests.cpp
src/tests/test_common.cpp
src/tests/boxlayouttest.cpp
src/tests/dragdroptest.cpp
src/tests/dropdowntest.cpp
src/tests/listboxtest.cpp
src/tests/layoutanimationtest.cpp
src/tests/ninepatchtest.cpp
src/tests/popuptest.cpp
src/tests/radiotest.cpp
src/tests/relativelayouttest.cpp
src/tests/roundedrecttest.cpp
src/tests/scrolltest.cpp
src/tests/simpletablelayouttest.cpp
src/tests/slidertest.cpp
src/tests/tabtest.cpp
src/tests/textboxtest.cpp
src/tests/textfieldbuttontest.cpp
src/tests/transformationtest.cpp
src/tests/windowframetest.cpp
src/tests/wordwraptest.cpp
src/tests/lguitest.cpp
)

add_executable(lguitest ${sources_lgui_test})
target_include_directories(lguitest PRIVATE src/lib)

set (sources_simple_example src/example/simple_example.cpp  src/example/shared.cpp)

add_executable(simple_example ${sources_simple_example})
target_include_directories(simple_example PRIVATE src/lib)
target_link_libraries (simple_example lgui allegro allegro_font allegro_ttf allegro_dialog allegro_primitives allegro_main)

set (sources_custom_widget_example src/example/custom_widget.cpp  src/example/shared.cpp)

add_executable(custom_widget_example ${sources_custom_widget_example})
target_include_directories(custom_widget_example PRIVATE src/lib)
target_link_libraries (custom_widget_example lgui allegro allegro_font allegro_ttf allegro_dialog allegro_primitives allegro_main)


# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

target_link_libraries (lguitest lgui allegro allegro_font allegro_image allegro_ttf allegro_dialog allegro_primitives allegro_main)
