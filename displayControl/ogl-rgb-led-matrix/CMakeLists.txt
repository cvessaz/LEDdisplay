##############
# Set opengl #
##############
message("Adding opengl library...")
find_package(opengl)
if(OPENGL_FOUND)
        set(LINKING_FLAGS       ${OPENGL_gl_LIBRARY}
                                ${LINKING_FLAGS})
        set(INCLUDE_PATHS       ${OPENGL_INCLUDE_DIR}
                                ${INCLUDE_PATHS})
else(OPENGL_FOUND)
        message(ERROR "Unable to find opengl library...")
endif(OPENGL_FOUND)

############
# Set glew #
############
message("Adding glew library...")
find_package(glew)
if(GLEW_FOUND)
	set(LINKING_FLAGS       ${GLEW_LIBRARIES}
				/usr/local/lib/libglfw.dylib
                        	${LINKING_FLAGS})
	set(INCLUDE_PATHS       ${GLEW_INCLUDE_DIRS}
                        	${INCLUDE_PATHS})
else(GLEW_FOUND)
	message(ERROR "Unable to find glew library...")
endif(GLEW_FOUND)

###############
# Set sources #
###############
set(SOURCES	rgb_matrix.cpp
		shader.cpp
		image.cpp
		bdfPrinter.cpp)

###############
# Set headers #
###############
set(HEADERS	rgb_matrix.h
		shader.h
		image.h
		bdfPrinter.h)

##################################
# Add ogl-rgb-led-matrix library #
##################################
message("Adding ogl-rgb-led-matrix library...")
include_directories(${INCLUDE_PATHS})
add_library(rgb_matrix ${SOURCES} ${HEADERS})
set(LINKING_FLAGS	rgb_matrix
			${LINKING_FLAGS} PARENT_SCOPE)
set(INCLUDE_PATHS	${PROJECT_SOURCE_DIR}/ogl-rgb-led-matrix
			${INCLUDE_PATHS} PARENT_SCOPE)
