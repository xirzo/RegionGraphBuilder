find_package(OGDF REQUIRED)

add_executable(ogdf_test ogdf_test.cpp)

target_link_libraries(ogdf_test 
    PRIVATE 
    OGDF
    COIN
)
