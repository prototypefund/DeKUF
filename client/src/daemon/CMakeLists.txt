file(GLOB DEKUF_CLIENT_DAEMON_LIB_SRC CONFIGURE_DEPENDS "*.cpp" "*.hpp")
list(FILTER DEKUF_CLIENT_DAEMON_LIB_SRC EXCLUDE REGEX "/main\\.cpp$")

add_library(dekuf-client-daemon-lib ${DEKUF_CLIENT_DAEMON_LIB_SRC})
target_link_libraries(dekuf-client-daemon-lib PUBLIC
    Qt6::Core
    Qt6::Network
    Qt6::DBus
    ${Gpgme_LIBRARY}
    dekuf-client-core
)

# Using .. to be able to include files like daemon/foo.cpp, but this also
# exposes the headers from sibling directories. There's probably a more elegant
# way to solve this.
target_include_directories(dekuf-client-daemon-lib PUBLIC ..)

add_executable(dekuf-client main.cpp)
target_link_libraries(dekuf-client PRIVATE dekuf-client-daemon-lib)

set_target_properties(dekuf-client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../..")
