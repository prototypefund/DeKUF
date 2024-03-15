file(GLOB DEKUF_CLIENT_TESTS_SRC CONFIGURE_DEPENDS "*.cpp" "*.hpp")
add_executable(dekuf-client-tests ${DEKUF_CLIENT_TESTS_SRC})
target_link_libraries(dekuf-client-tests PRIVATE dekuf-client-lib Qt::Test)
add_test(NAME dekuf-client-tests COMMAND dekuf-client-tests)
