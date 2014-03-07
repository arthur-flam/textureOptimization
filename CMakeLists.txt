cmake_minimum_required(VERSION 2.8)
project( DisplayImage )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( textureOptimization main.cpp energy_optimization.cpp source_optimization.cpp multiscale.cpp NL/nl_single_file.c)
target_link_libraries( textureOptimization ${OpenCV_LIBS} )

