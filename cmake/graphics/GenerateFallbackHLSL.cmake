# Helper to generate fallback HLSL header files containing the raw source code
if(NOT SHADER_PATH OR NOT HEADER_OUTPUT OR NOT VAR_NAME)
    message(FATAL_ERROR "Missing required variables: SHADER_PATH, HEADER_OUTPUT, VAR_NAME")
endif()

file(READ "${SHADER_PATH}" SHADER_CONTENT)
file(WRITE "${HEADER_OUTPUT}" "const char ${VAR_NAME}[] = R\"raw(\n${SHADER_CONTENT}\n)raw\";\nconst bool ${VAR_NAME}_is_source = true;\n")
