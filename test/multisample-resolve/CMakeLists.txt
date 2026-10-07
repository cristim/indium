add_executable(indium-test-multisample-resolve multisample-resolve.cpp)
target_link_libraries(indium-test-multisample-resolve PRIVATE indium indium_private)
set_target_properties(indium-test-multisample-resolve PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
