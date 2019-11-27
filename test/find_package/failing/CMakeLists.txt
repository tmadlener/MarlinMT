
cmake_minimum_required( VERSION 3.14 )
project( simple )

find_package( Marlin COMPONENTS LCIOPlugins REQUIRED )

add_executable( simple simple.cc )
# This should fail, as Marlin::LCIOPlugins is a module not a shared library
target_link_libraries( simple Marlin::Core Marlin::LCIOPlugins )

