file(GLOB DEKUF_CLIENT_UI_SRC CONFIGURE_DEPENDS "*.cpp" "*.hpp" "*.ui")
add_executable(dekuf-client-ui ${DEKUF_CLIENT_UI_SRC})
target_link_libraries(dekuf-client-ui PRIVATE
        dekuf-client-core
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        ${GMP_LIBRARIES}
)
set_target_properties(dekuf-client-ui PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../..")
