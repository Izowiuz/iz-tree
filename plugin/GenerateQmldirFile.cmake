include (CMakeParseArguments)

function(generate_qmldir_file MODULE_NAME PLUGIN_NAME ADDITIONAL_JS_FILES)
	configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/qmldir.in
        ${CMAKE_CURRENT_BINARY_DIR}/qmldir
        @ONLY)
endfunction()