file(GLOB DEKUF_CLIENT_SRC CONFIGURE_DEPENDS "*.cpp" "*.hpp")
add_executable(dekuf-client ${DEKUF_CLIENT_SRC})
target_link_libraries(dekuf-client PRIVATE
    Qt6::Core
    Qt6::Network
    dekuf-client-core
)
set_target_properties(dekuf-client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../..")
