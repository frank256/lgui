cmake_minimum_required (VERSION 3.0)

PROJECT(lguitest)

set (CMAKE_CXX_FLAGS "-Wall -W -Wextra -O2 -std=c++14")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories("src")

set(sources_lgui
src/lgui/basiccontainer.cpp
src/lgui/basiccontainer.h
src/lgui/buttongroup.h
src/lgui/buttongroup.cpp
src/lgui/cursorblinkhelper.h
src/lgui/cursorblinkhelper.cpp
src/lgui/dragdropevent.cpp
src/lgui/dragdropevent.h
src/lgui/dragrepresentation.cpp
src/lgui/dragrepresentation.cpp
src/lgui/eventfilter.cpp
src/lgui/eventfilter.h
src/lgui/gui.cpp
src/lgui/gui.h
src/lgui/focusevent.h
src/lgui/focusevent.cpp
src/lgui/ieventlistener.cpp
src/lgui/ieventlistener.h
src/lgui/ikeylistener.h
src/lgui/ilayoutelement.h
src/lgui/iwidgetlistener.h
src/lgui/inputevent.h
src/lgui/keyevent.cpp
src/lgui/keyevent.h
src/lgui/mouseevent.cpp
src/lgui/mouseevent.h
src/lgui/lgui.h
src/lgui/lgui.cpp
src/lgui/signal.h
src/lgui/timertickevent.cpp
src/lgui/timertickevent.h
src/lgui/vector_utils.h
src/lgui/widget.h
src/lgui/widget.cpp
src/lgui/widgetpc.h
src/lgui/widgetpc.cpp
src/lgui/wordwrappedtext.h
src/lgui/wordwrappedtext.cpp
src/lgui/internal/dragdroptrackhelper.cpp
src/lgui/internal/dragdroptrackhelper.h
src/lgui/internal/eventdistributor.cpp
src/lgui/internal/eventdistributor.h
src/lgui/internal/eventhandler.cpp
src/lgui/internal/eventhandler.h
src/lgui/internal/eventhandlerbase.cpp
src/lgui/internal/eventhandlerbase.h
src/lgui/internal/focusmanager.h
src/lgui/internal/focusmanager.cpp
src/lgui/internal/mousehandler.cpp
src/lgui/internal/mousehandler.h
src/lgui/internal/mousestate.cpp
src/lgui/internal/mousestate.h
src/lgui/internal/mousetrackhelper.h
src/lgui/internal/mousetrackhelper.cpp
src/lgui/internal/timerhandler.h
src/lgui/internal/timerhandler.cpp
src/lgui/internal/trackhelper.h
src/lgui/internal/trackhelper.cpp
src/lgui/style/abstractstyle.h
src/lgui/style/style.h
src/lgui/style/styleargs.h
src/lgui/style/defaultstyle.h
src/lgui/style/defaultstyle.cpp
src/lgui/style/defaultstyle2ndborder.h
src/lgui/style/defaultstyle2ndborder.cpp
src/lgui/style/defaultstylecolorscheme.h
src/lgui/style/defaultstylecolorscheme.cpp
src/lgui/widgets/abstractbutton.h
src/lgui/widgets/abstractbutton.cpp
src/lgui/widgets/checkbox.h
src/lgui/widgets/checkbox.cpp
src/lgui/widgets/container.h
src/lgui/widgets/container.cpp
src/lgui/widgets/dropdown.h
src/lgui/widgets/dropdown.cpp
src/lgui/widgets/frame.h
src/lgui/widgets/frame.cpp
src/lgui/widgets/pushbutton.h
src/lgui/widgets/pushbutton.cpp
src/lgui/widgets/radiobutton.h
src/lgui/widgets/radiobutton.cpp
src/lgui/widgets/scrollarea.cpp
src/lgui/widgets/scrollarea.h
src/lgui/widgets/scrollbar.cpp
src/lgui/widgets/scrollbar.h
src/lgui/widgets/slider.cpp
src/lgui/widgets/slider.h
src/lgui/widgets/spinner.cpp
src/lgui/widgets/spinner.h
src/lgui/widgets/stackedcontainer.h
src/lgui/widgets/stackedcontainer.cpp
src/lgui/widgets/textfield.h
src/lgui/widgets/textfield.cpp
src/lgui/widgets/textbox.h
src/lgui/widgets/textbox.cpp
src/lgui/widgets/wrapwidget.h
src/lgui/widgets/wrapwidget.cpp
src/lgui/widgets/buttons/helperbutton.h
src/lgui/widgets/buttons/helperbutton.cpp
src/lgui/widgets/buttons/scaledimagebutton.h
src/lgui/widgets/buttons/scaledimagebutton.cpp
src/lgui/widgets/buttons/imagebutton.h
src/lgui/widgets/buttons/imagebutton.cpp
src/lgui/widgets/labels/imagelabel.cpp
src/lgui/widgets/labels/imagelabel.h
src/lgui/widgets/labels/scaledimagelabel.cpp
src/lgui/widgets/labels/scaledimagelabel.h
src/lgui/widgets/labels/textlabel.cpp
src/lgui/widgets/labels/textlabel.h
src/lgui/widgets/labels/wordwraptextlabel.cpp
src/lgui/widgets/labels/wordwraptextlabel.h
src/lgui/widgets/listbox/stringlistview.h
src/lgui/widgets/listbox/stringlistview.cpp
src/lgui/widgets/listbox/stringlistmodel.h
src/lgui/widgets/listbox/listmodellistener.h
src/lgui/widgets/listbox/abstractlistmodel.cpp
src/lgui/widgets/listbox/abstractlistmodel.h
src/lgui/widgets/listbox/listbox.h
src/lgui/widgets/tabs/tab.h
src/lgui/widgets/tabs/tab.cpp
src/lgui/widgets/tabs/tabbar.h
src/lgui/widgets/tabs/tabbar.cpp
src/lgui/widgets/tabs/tabwidget.h
src/lgui/widgets/tabs/tabwidget.cpp
src/lgui/layout/alignlayout.h
src/lgui/layout/alignlayout.cpp
src/lgui/layout/boxlayout.h
src/lgui/layout/boxlayout.cpp
src/lgui/layout/hboxlayout.h
src/lgui/layout/vboxlayout.h
src/lgui/layout/flowlayout.h
src/lgui/layout/flowlayout.cpp
src/lgui/layout/layout.h
src/lgui/layout/layout.cpp
src/lgui/layout/layoutitem.h
src/lgui/layout/layoutitem.cpp
src/lgui/layout/relativelayout.h
src/lgui/layout/relativelayout.cpp
src/lgui/layout/relativelayouthelpers.h
src/lgui/layout/relativelayouthelpers.cpp
src/lgui/layout/simpletablelayout.h
src/lgui/layout/simpletablelayout.cpp
src/lgui/layout/sortedflowlayout.h
src/lgui/layout/sortedflowlayout.cpp
src/platform/bitmap.h
src/platform/color.h
src/platform/events.h
src/platform/error.h
src/platform/error.cpp
src/platform/font.h
src/platform/font.cpp
src/platform/graphics.h
src/platform/graphics.cpp
src/platform/ninepatch.h
src/platform/ninepatchbase.h
src/platform/ninepatchbase.cpp
src/platform/primhelper.h
src/platform/stringfmt.h
src/platform/stringfmt.cpp
src/platform/utf8.h
src/platform/utf8.cpp
src/platform/a5/a5bitmap.cpp
src/platform/a5/a5bitmap.h
src/platform/a5/a5color.h
src/platform/a5/a5events.cpp
src/platform/a5/a5events.h
src/platform/a5/a5error.h
src/platform/a5/a5error.cpp
src/platform/a5/a5font.cpp
src/platform/a5/a5font.h
src/platform/a5/a5graphics.cpp
src/platform/a5/a5graphics.h
src/platform/a5/a5ninepatch.cpp
src/platform/a5/a5ninepatch.h
src/platform/a5/a5primhelper.cpp
src/platform/a5/a5primhelper.h
)

set (sources_lgui_test
src/tests/test_common.cpp
src/tests/boxlayouttest.cpp
src/tests/dragdroptest.cpp
src/tests/dropdowntest.cpp
src/tests/listboxtest.cpp
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
src/tests/windowframetest.cpp
src/tests/wordwraptest.cpp
src/tests/lguitest.cpp)


add_executable(lguitest ${sources_lgui} ${sources_lgui_test})

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

target_link_libraries (lguitest allegro allegro_font allegro_image allegro_ttf allegro_dialog allegro_primitives allegro_main)
