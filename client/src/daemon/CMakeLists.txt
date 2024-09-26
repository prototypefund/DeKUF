file(GLOB PRIVACT_CLIENT_DAEMON_LIB_SRC CONFIGURE_DEPENDS "*.cpp" "*.hpp")
list(FILTER PRIVACT_CLIENT_DAEMON_LIB_SRC EXCLUDE REGEX "/main\\.cpp$")

add_library(privact-client-daemon-lib ${PRIVACT_CLIENT_DAEMON_LIB_SRC})
target_link_libraries(privact-client-daemon-lib PUBLIC
        Qt6::Core
        Qt6::Network
        Qt6::DBus
        ${Gpgme_LIBRARY}
        ${GMP_LIBRARIES}
        privact-client-core
)

# Using .. to be able to include files like daemon/foo.cpp, but this also
# exposes the headers from sibling directories. There's probably a more elegant
# way to solve this.
target_include_directories(privact-client-daemon-lib PUBLIC ..)

add_executable(privact-client main.cpp)
target_link_libraries(privact-client PRIVATE privact-client-daemon-lib)

set_target_properties(privact-client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "../..")
