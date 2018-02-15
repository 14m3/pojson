#INCLUDE_DIRECTORIES (${PROJECT_BINARY_DIR}/include)
INCLUDE_DIRECTORIES (../src)
ADD_EXECUTABLE(polojson_test test.cpp)
TARGET_LINK_LIBRARIES(polojson_test libpolojson)
