

The build system of MarlinMT is CMake. After a successful installation of MarlinMT, a set of CMake configuration files are installed with the software, listed (non-exaustive):

- MarlinConfig.cmake
- MarlinConfigVersion.cmake
- MarlinConfig-targets.cmake

This page describes how to write CMake files to compile your project against MarlinMT and create the needed libraries to run your MarlinMT processors.
In the following, we assume that Marlin was installed in `/home/$USER/MarlinMT`, installation path refereed as `$Marlin_DIR` later-on. 

## Finding the MarlinMT package

### Pointing to a Marlin installation

To point to a specific Marlin installation, you need to tell CMake where the configuration files are. 
The CMake documentation [about this](https://cmake.org/cmake/help/v3.14/manual/cmake-packages.7.html) may be more complete than the information in this section.
The common way to point to configuration files is to provide the `<PACKAGE>_DIR` variable, in our case `Marlin_DIR`:

```shell
# from the build directory, assume your
# CMakeLists.txt file is in .. 
cmake -DMarlin_DIR=$Marlin_DIR/lib/Marlin-<version> ..
``` 

If you use version 1.16.0, then use:

```shell
cmake -DMarlin_DIR=$Marlin_DIR/lib/Marlin-1.16.0 ..
``` 

### Use Marlin CMake configuration files

The command to find a package using CMake is the following:

```cmake
# Minimal call to find Marlin
find_package( Marlin CONFIG REQUIRED )
```

This call will look for MarlinConfig.cmake in your system. If CMake couldn't not find it, an error is raised and CMake stops processing.
On success, a few targets will be imported from configuration files, allowing to access include directories, libraries, etc... to use for compiling and linking in your project. Using the call has shown above, only the target `Marlin::Core` is imported. This target holds the core framework include directories (normally `$Marlin_DIR/include`) and the library `MarlinCore`. 

Your project may require the use of other components. For example, you want to use LCIO in your processors as the main event handle or the DD4hep geometry. 

We first need to differentiate two component cases:

- a **toolkit** library: a shared library which comes with include directories installed along with the library. The user can link against this library and use the includes for its development. For example, the `MarlinBook` library is such a library.
- a **plugin** library: a shared library that will be loaded at runtime by an application (Marlin in our case). This library has been installed along with other libraries but are not designed to be linked against by users. For example `LCIOPlugins` is such a library.

The `find_package` CMake command can take additional arguments to find additional components:

```cmake
# Find Marlin core and additional components 
find_package( Marlin CONFIG COMPONENTS Book LCIOPlugins DD4hepPlugins REQUIRED )
```

This call will add an additional target to the `Marlin::` namespace, the target `Marlin::Book`. As a rule of thumb, if the component name contains `Plugins`, then we are dealing with `plugin` libraries. In the `find_package` CMake call for Marlin, no target will be created for `plugin` libraries. The role of this call with plugin components is to check that the installed version of Marlin is distributed with the components you need. For toolkit libraries, such as `MarlinBook`, an imported target will be created (in this case `Marlin::Book`). This target provides include directories, library, etc... (as for `Marlin::Core`) for linking you project against. If any of these components is missing CMake will raise an error.

## Examples

### General notes

In general a project using Marlin can be partitioned in 3 types of files:

- toolkit files: located in your `include` and `src` directories they implement all the functionalities you need in your project.
- plugin files: self-contained code not included in any other place of your project. These are plugins that will be compiled in a module library (see [module library](https://cmake.org/cmake/help/v3.14/command/add_library.html?highlight=add_library#id2)). For them, you don't need header files as they are self-contained.
- binaries: executables compiled and installed along with your project.

It is generally a question of taste of how to organize your files in a project. In the case of Marlin, the only recommendation is to make sure the toolkit and the plugins are well separated in the project. In particular, you should not compile a plugin in your toolkit library, as the plugin will be loaded by any project linking against your library. This is generally wanted and is preferable to compile a separate library (or many if you need) for your plugins and add `"Plugins"` in the library name.  

### A single processor library

We assume the following working tree:

```shell
$ ls -R
CMakeLists.txt
source/plugins/MyProcessor.cc
```

with `MyProcessor.cc` implementing a single processor plugin. 

To compile your project with CMake, use: 

```cmake
# Top-level CMakeLists.txt file
cmake_minimum_required( VERSION 3.14 FATAL_ERROR )
project( MyProject )

# Standard CMake file: sets prefix path for libraries, includes, etc ...
include( GNUInstallDirs )

# Find Marlin. Imports all needed targets
find_package( Marlin CONFIG REQUIRED )

# MODULE libraries are shared libraries, designed to be loaded 
# at runtime, not to be linked against
add_library( MyPlugins MODULE source/plugins/MyProcessor.cc )

# This will link your processor library against the MarlinCore library...
# but not only
# It will add the include directories of Marlin::Core to the target 
# include directories automatically (and definitions, etc ...) 
target_link_libraries( MyPlugins PUBLIC Marlin::Core )

# Install your library. CMAKE_INSTALL_LIBDIR is defined in GNUInstallDirs
install( TARGETS MyPlugins LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
```

### Project requiring other components

We assume the same working tree:

```shell
$ ls -R
CMakeLists.txt
source/plugins/MyProcessor.cc
```

with `MyProcessor.cc` implementing a single processor plugin using LCIO event data model.

To compile your project with CMake, use: 

```cmake
# Top-level CMakeLists.txt file
cmake_minimum_required( VERSION 3.14 FATAL_ERROR )
project( MyProject )

# Standard CMake file: sets prefix path for libraries, includes, etc ...
include( GNUInstallDirs )

# Find Marlin. Imports all needed targets
find_package( Marlin CONFIG COMPONENTS LCIOPlugins DD4hepPlugins REQUIRED )

# Find LCIO library. LCIO is not part of any Marlin imported targets
# and has to be found manually
find_package( LCIO REQUIRED )

# MODULE libraries are shared libraries, designed to be loaded 
# at runtime, not to be linked against
add_library( MyPlugins MODULE source/plugins/MyProcessor.cc )

# This will link your processor library against the MarlinCore library...
# but not only
# It will add the include directories of Marlin::Core to the target 
# include directories automatically (and definitions, etc ...) 
target_link_libraries( MyPlugins PUBLIC Marlin::Core )

# LCIO specific linking, includes ...
target_link_libraries( MyPlugins PUBLIC ${LCIO_LIBRARIES} )
# SYSTEM means "ignore warnings from LCIO includes" as they are not from 
# your project. No need to show them...
target_include_directories( MyPlugins SYSTEM PUBLIC ${LCIO_INCLUDE_DIRS} )

# Install your library. CMAKE_INSTALL_LIBDIR is defined in GNUInstallDirs
install( TARGETS MyPlugins LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
```

### Project with a toolkit library and a plugin library

We assume the following working tree:

```shell
$ ls -R
CMakeLists.txt
source/plugins/MyProcessor.cc
source/include/Tools.h
source/src/Tools.cc
```

with `MyProcessor.cc` implementing a single processor plugin and `Tools.h/.cc` implementing tools for your project.

To compile your project with CMake, use: 

```cmake
# Top-level CMakeLists.txt file
cmake_minimum_required( VERSION 3.14 FATAL_ERROR )
project( MyProject )

# Standard CMake file: sets prefix path for libraries, includes, etc ...
include( GNUInstallDirs )

# Find Marlin. Imports all needed targets
find_package( Marlin CONFIG REQUIRED )

# Compile your tools first in a library
add_library( MyTools SHARED source/Tools.cc )

# Include directories for compiling your library
# Also specify that you want to install your includes 
target_include_directories( MyTools PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source/include>
  $<INSTALL_INTERFACE:include>
)

# Your plugin library
add_library( MyPlugins MODULE source/plugins/MyProcessor.cc )

# This will link your processor library against the MarlinCore and library...
# but not only
# It will add the include directories of the targets
# automatically (and definitions, etc ...) 
target_link_libraries( MyPlugins PUBLIC Marlin::Core MyTools )

# Install your libraries. CMAKE_INSTALL_LIBDIR is defined in GNUInstallDirs
install( TARGETS MyPlugins MyTools LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
```

