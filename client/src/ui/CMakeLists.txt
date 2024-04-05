file(GLOB DEKUF_CLIENT_UI_SRC CONFIGURE_DEPENDS "*.cpp" "*.hpp" "*.ui")
add_executable(dekuf-client-ui ${DEKUF_CLIENT_UI_SRC}
    # TODO: Move *Storage and SurveyResponse to core lib.
    "../daemon/sqlite_storage.cpp"
    "../daemon/survey_response.cpp"
)
target_link_libraries(dekuf-client-ui PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets
    Qt6::Sql # TODO: Link against core lib instead.
)
set_target_properties(dekuf-client-ui PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../..")
