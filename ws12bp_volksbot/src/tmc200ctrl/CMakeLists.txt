cmake_minimum_required(VERSION 2.8.3)
project(tmc200ctrl)

set(TMC200CTRL_PUBL_INC ${CMAKE_SOURCE_DIR}/../include)

OPTION(PA_DEMONSTRATION "Option description" OFF) # Enabled by default

IF(PA_DEMONSTRATION)
    ADD_DEFINITIONS(-DPA_DEMONSTRATION)
ENDIF(PA_DEMONSTRATION)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp message_generation)

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)

## Uncomment this if the package has a setup.py. This macro ensures
## modules and scripts declared therein get installed
# catkin_python_setup()

#######################################
## Declare ROS messages and services ##
#######################################

file(GLOB MESSAGES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/msg msg/*.msg)

## Generate messages in the 'msg' folder
add_message_files (
    FILES ${MESSAGES}
)

file(GLOB SERVICES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/srv srv/*.srv)

add_service_files (
	FILES ${SERVICES}
)

## Generate added messages and services with any dependencies listed here
generate_messages(
	DEPENDENCIES std_msgs	
 )



###################################################
## Declare things to be passed to other projects ##
###################################################

## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need

catkin_package(
   CATKIN_DEPENDS message_runtime
)

###########
## Build ##
###########

## Specify additional locations of header files
include_directories(
	include
	${TMC200CTRL_PUBL_INC}/libtmc200ctrl_io
)

LINK_DIRECTORIES(
	${LINK_DIRECTORIES}
)

add_executable (tmc200controller
			 src/tmc200controller.cpp
			)

add_dependencies(tmc200controller tmc200ctrl_io)

## Specify libraries to link a library or executable target against
target_link_libraries(tmc200controller ${catkin_LIBRARIES} tmc200ctrl_io)


#############
## Install ##
#############

## Mark executables and/or libraries for installation
install(TARGETS tmc200controller
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	RUNTIME DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}/${PROJECT_NAME}
 )

file(GLOB INCLUDES include/*.h)

## Mark cpp header files for installation
 install(FILES ${INCLUDES}
   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
 )

