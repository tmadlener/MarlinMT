
cmake_minimum_required( VERSION 3.14 )
project( simple )

find_package( ILCUTIL COMPONENTS streamlog REQUIRED )
find_package( Marlin REQUIRED )

add_executable( simple simple.cc )
target_include_directories( simple SYSTEM PUBLIC 
  $<TARGET_PROPERTY:Marlin::MarlinCore,INCLUDE_DIRECTORIES> 
  ${streamlog_INCLUDE_DIRS}
)
target_link_libraries( simple Marlin::MarlinCore )

