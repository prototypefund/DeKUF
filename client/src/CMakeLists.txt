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
