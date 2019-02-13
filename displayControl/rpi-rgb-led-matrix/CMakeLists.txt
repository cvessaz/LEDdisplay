###################
# Add definitions #
###################
add_definitions(-DADAFRUIT_RGBMATRIX_HAT)

###############
# Set sources #
###############
set(SOURCES	lib/bdf-font.cc
		lib/framebuffer.cc
		lib/gpio.cc
		lib/graphics.cc
		lib/led-matrix.cc
		lib/thread.cc)

###############
# Set headers #
###############
set(HEADERS	include/canvas.h
		lib/framebuffer-internal.h
		include/gpio.h
		include/graphics.h
		include/led-matrix.h
		include/thread.h
		include/threaded-canvas-manipulator.h
		lib/utf8-internal.h)

##################################
# Add rpi-rgb-led-matrix library #
##################################
message("Adding rpi-rgb-led-matrix library...")
include_directories(${PROJECT_SOURCE_DIR}/rpi-rgb-led-matrix/include)
if(RPI)
	add_library(rgb_matrix ${SOURCES} ${HEADERS})
	set(LINKING_FLAGS	rgb_matrix
				-lrt -lm -lpthread
				${LINKING_FLAGS} PARENT_SCOPE)
	set(INCLUDE_PATHS	${PROJECT_SOURCE_DIR}/rpi-rgb-led-matrix/include
				${INCLUDE_PATHS} PARENT_SCOPE)
else(RPI)
	add_library(rpi_rgb_matrix ${SOURCES} ${HEADERS})
endif(RPI)
