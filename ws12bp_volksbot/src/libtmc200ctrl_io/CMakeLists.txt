cmake_minimum_required(VERSION 2.8.3)
project(libtmc200ctrl_io)

catkin_package()

## Specify additional locations of header files
include_directories(
  priv_inc
  include
)

if (CMAKE_COMPILER_IS_GNUCXX)
	set ( CMAKE_C_FLAGS "-Wall -Wextra -std=gnu99 -pedantic-errors")
	set ( CMAKE_C_FLAGS_DEBUG "-O0 -g3")
	set ( CMAKE_C_FLAGS_RELEASE "-O2 -g0")

	if(CMAKE_BUILD_TYPE MATCHES DEBUG)
		add_definitions(-DDEBUG)
	endif(CMAKE_BUILD_TYPE MATCHES DEBUG)

	if(CMAKE_BUILD_TYPE MATCHES RELEASE)
		set(CMAKE_EXE_LINKER_FLAGS "-s")
	endif(CMAKE_BUILD_TYPE MATCHES RELEASE)
endif (CMAKE_COMPILER_IS_GNUCXX)

add_library(
	tmc200ctrl_io
	SHARED
	src/queues.c
	src/sendtmc200cmdsthread.c
	src/tmc200feedbackthread.c
	src/libtmc200ctrl_io.c
	src/threads_common.c
)

target_link_libraries(tmc200ctrl_io pthread)


#############
## Install ##
#############

## Mark executables and/or libraries for installation
install(TARGETS tmc200ctrl_io
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
)

file(GLOB INCLUDES include/*.h)

## Mark cpp header files for installation
 install(FILES ${INCLUDES}
   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
 )

