###############
# Set sources #
###############
set(SOURCES	main.cpp
		data.cpp
		localControl.cpp
		remoteControl.cpp)

###############
# Set headers #
###############
set(HEADERS	global.h
		data.h
		localControl.h
		remoteControl.h)

#################################
# Add displayControl executable #
#################################
message("Adding executable for displayControl...")
include_directories(${INCLUDE_PATHS})
add_executable(displayControl ${SOURCES} ${HEADERS})
target_link_libraries(displayControl ${LINKING_FLAGS})
message("   INCLUDE_PATHS: ${INCLUDE_PATHS}")
message("   LINKING_FLAGS: ${LINKING_FLAGS}")

#######################
# Post-build commands #
#######################
add_custom_command(TARGET displayControl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:displayControl>/bdfFonts
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_custom_command(TARGET displayControl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory bdfFonts $<TARGET_FILE_DIR:displayControl>/bdfFonts
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
if(APPLE)
        add_custom_command(TARGET displayControl POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:displayControl>/shaders
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
        add_custom_command(TARGET displayControl POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory ogl-rgb-led-matrix/shaders $<TARGET_FILE_DIR:displayControl>/shaders
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif(APPLE)
