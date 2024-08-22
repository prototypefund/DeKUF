file(GLOB DEKUF_CLIENT_CORE_SRC CONFIGURE_DEPENDS "*.cpp" "*.hpp")
add_library(dekuf-client-core ${DEKUF_CLIENT_CORE_SRC})

target_link_libraries(dekuf-client-core PUBLIC
        Qt6::Core
        Qt6::Sql
        ${GMP_LIBRARIES}
)

# Using .. to be able to include files like core/foo.cpp, but this also exposes
# the headers from sibling directories. There's probably a more elegant way to
# solve this.
target_include_directories(dekuf-client-core PUBLIC ..)
