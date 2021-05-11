#=======================================================
# Video Compression and Stitch CMAKE BUILD 
#=======================================================
cmake_minimum_required(VERSION 3.9)
PROJECT(Video_Compression_and_Stitch LANGUAGES C CXX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


#=======================================================
# FIND DEPENDENCIES
#=======================================================
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Charts CONFIG REQUIRED)
find_package(Qt5Multimedia CONFIG REQUIRED)

#=======================================================
# ADD EXECUTABLE
#=======================================================
file(GLOB srcs ${PROJECT_SOURCE_DIR}/Source/*.cpp ${PROJECT_SOURCE_DIR}/Source/*.rc)
file(GLOB hdrs ${PROJECT_SOURCE_DIR}/Include/*.h)
file(GLOB icons ${PROJECT_SOURCE_DIR}/Resources/*.png)
file(GLOB devaids ${PROJECT_SOURCE_DIR}/Resources/*.txt)
file(GLOB rsrcs ${PROJECT_SOURCE_DIR}/Resources/*.qrc)

include_directories(${PROJECT_SOURCE_DIR}/Include ${PROJECT_SOURCE_DIR}/Resources)
add_executable(Video_Compression_and_Stitch ${srcs} ${hdrs} ${icons} ${devaids} ${rsrcs})
target_link_libraries(Video_Compression_and_Stitch Qt5::Widgets Qt5::Charts Qt5::Multimedia Wininet.dll)