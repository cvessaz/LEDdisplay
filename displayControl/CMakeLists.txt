###############
# Set project #
###############
cmake_minimum_required(VERSION 3.0)
message("Running cmake for LEDdisplay project...")
project(displayControl)

################
# Set platform #
################
if(APPLE)
	message("Build for apple...")
	add_definitions(-DAPPLE)
else()
	message("Build for RaspberryPi...")
	add_definitions(-DRPI)
endif()

######################
# Set configurations #
######################
message("Setting CXX_FLAGS and configurations...")
set(CMAKE_CONFIGURATION_TYPES DEBUG;RELEASE)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
message("   CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG -g -O0)
message("   CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE -O3 -ffast-math)
set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG)
message("   CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message("   Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

######################
# rpi-rgb-led-matrix #
######################
add_subdirectory(rpi-rgb-led-matrix)

######################
# ogl-rgb-led-matrix #
######################
if(APPLE)
add_subdirectory(ogl-rgb-led-matrix)
endif()

#####################
# Add src directory #
#####################
add_subdirectory(src)
