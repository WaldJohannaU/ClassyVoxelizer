CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
PROJECT(ClassyVoxelizer)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

SET(HEADER_DIR ${PROJECT_SOURCE_DIR}/include)
SET(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

SET(SRC_FILES ${SOURCE_DIR}/ColoredVoxelGrid.cpp 			  
			  ${SOURCE_DIR}/ColoredVoxelizer.cpp 
			  ${SOURCE_DIR}/ClassyVoxelizer.cpp 
			  ${SOURCE_DIR}/main.cpp 
			  ${SOURCE_DIR}/MultiClassVoxelGrid.cpp 
			  ${SOURCE_DIR}/MultiClassVoxelizer.cpp  
			  ${SOURCE_DIR}/tinyply.cpp
			  ${SOURCE_DIR}/Voxelizer.cpp 
			  ${SOURCE_DIR}/VoxelGrid.cpp)

SET(HEADER_FILES ${HEADER_DIR}/ColoredVoxelGrid.h 
				 ${HEADER_DIR}/ColoredVoxelizer.h 
				 ${HEADER_DIR}/ClassyVoxelizer.h
				 ${HEADER_DIR}/MultiClassVoxelGrid.h 
				 ${HEADER_DIR}/MultiClassVoxelizer.h 
				 ${HEADER_DIR}/tinyply.h
			  	 ${HEADER_DIR}/Voxelizer.h 
				 ${HEADER_DIR}/VoxelGrid.h)

FIND_PACKAGE(Eigen3 REQUIRED)

INCLUDE_DIRECTORIES(${HEADER_DIR})
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})

ADD_EXECUTABLE(classy_voxelizer ${SRC_FILES} ${HEADER_FILES})
