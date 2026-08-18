function(target_copy_assets TARGET_NAME)
    set(ASSETS_SRC_DIR "${CMAKE_SOURCE_DIR}/assets")
    set(ASSETS_DST_DIR "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets")

    add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSETS_SRC_DIR}"
            "${ASSETS_DST_DIR}"
        COMMENT "Copying assets directory next to the ${TARGET_NAME} executable..."
        VERBATIM
    )
endfunction()