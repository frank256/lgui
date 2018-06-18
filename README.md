lgui
===================

lgui is a C++ 11/14 GUI currently coming with an Allegro 5 backend.

Features
========

- easy to use signals & slots to connect widgets (just register something callable - a std::function)
- two pass layout system, Android style; separate widgets and layouts, nestable layouts
- drawing done in separate style classes
- internal drag and drop
- modal widgets, GUI stacks
- event filters
- 9-patch images

Dependencies
============

You need to have Allegro5 and CMake installed; for the documentation, doxygen
is needed as well.

The documentation has to be generated explicitly by issuing the "make doc"
command from within your build directory. 

Feel free to report any issues you might have. Or fix them right away ;) 
-- pull requests are welcome.
