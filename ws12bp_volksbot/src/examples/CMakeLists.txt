cmake_minimum_required(VERSION 2.8.3)
project(tmc200ctrl_examples)

set(TMC200CTRL_PUBL_INC ${CMAKE_SOURCE_DIR}/../include)

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

## Specify additional locations of header files
include_directories(
  ${TMC200CTRL_PUBL_INC}/libtmc200ctrl_utils
  ${TMC200CTRL_PUBL_INC}/libtmc200ctrl_direct
  ${TMC200CTRL_PUBL_INC}/libtmc200ctrl_io
)

LINK_DIRECTORIES(
	${LINK_DIRECTORIES}
	
)

add_executable(tmc200ctrl_direct_test src/tmc200ctrl_direct_test.cpp)
add_executable(tmc200ctrl_remote_test src/tmc200ctrl_remote.cpp)

add_dependencies(tmc200ctrl_direct_test tmc200ctrl_io tmc200ctrl_direct tmc200ctrl_utils)
add_dependencies(tmc200ctrl_remote_test tmc200ctrl_utils)


target_link_libraries(tmc200ctrl_remote_test ${catkin_LIBRARIES} tmc200ctrl_utils )
target_link_libraries(tmc200ctrl_direct_test tmc200ctrl_direct tmc200ctrl_io  tmc200ctrl_utils)

install(TARGETS tmc200ctrl_direct_test tmc200ctrl_remote_test
	RUNTIME DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}/${PROJECT_NAME}
)
