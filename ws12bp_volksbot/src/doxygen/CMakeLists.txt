cmake_minimum_required(VERSION 2.8.3)
project(doxygen)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
	MESSAGE(STATUS "Generating API documentation")
execute_process(COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/../tmc200ctrl.doxygen
				OUTPUT_QUIET
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../
				 )
else()
	ERROR("Doxygen wasn't found. Code documentation can't be build.")
endif(DOXYGEN_FOUND)

