if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")

file(GLOB DEKUF_CLIENT_LIB_SRC CONFIGURE_DEPENDS "*.cpp" "*.hpp")
list(REMOVE_ITEM DEKUF_CLIENT_LIB_SRC "main.cpp")

add_library(dekuf-client-lib ${DEKUF_CLIENT_LIB_SRC})
target_link_libraries(dekuf-client-lib PUBLIC
    Qt6::Core
    Qt6::Network
    Qt6::Sql
)

add_executable(dekuf-client main.cpp)
target_link_libraries(dekuf-client PRIVATE dekuf-client-lib)
set_target_properties(dekuf-client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "..")

add_subdirectory(ui)
