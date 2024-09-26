file(GLOB PRIVACT_CLIENT_UI_SRC CONFIGURE_DEPENDS "*.cpp" "*.hpp" "*.ui")
add_executable(privact-client-ui ${PRIVACT_CLIENT_UI_SRC})
target_link_libraries(privact-client-ui PRIVATE
        privact-client-core
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        ${GMP_LIBRARIES}
)
set_target_properties(privact-client-ui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "../..")
