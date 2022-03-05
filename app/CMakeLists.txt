AUX_SOURCE_DIRECTORY(. APP_LIST)

FILE(GLOB LIB_LIST 
        ${PROJECT_SOURCE_DIR}/lib/*.dll 
        ${PROJECT_SOURCE_DIR}/lib/*.lib 
        ${PROJECT_SOURCE_DIR}/lib/*.a
)
MESSAGE("${LIB_LIST}\n")

FOREACH(app ${APP_LIST})
    GET_FILENAME_COMPONENT(app_name ${app} NAME_WE)
    MESSAGE("\nFound application ${app}")
    ADD_EXECUTABLE(${app_name} ${app})
    # TARGET_LINK_LIBRARIES(${app_name} myGL glfw3)
    IF(LIB_LIST)
        FOREACH(lib ${LIB_LIST})
            GET_FILENAME_COMPONENT(lib_name ${lib} NAME_WE)
            MESSAGE("Found library ${lib_name}")
            STRING(FIND ${lib_name} "lib" lib_idx)
            IF(lib_idx)
                TARGET_LINK_LIBRARIES(${app_name} ${lib_name})
            ELSE()
                STRING(REPLACE "lib" "" lib_name_nib ${lib_name})
                TARGET_LINK_LIBRARIES(${app_name} ${lib_name_nib})
            ENDIF()
        ENDFOREACH()
    ELSE()
        MESSAGE("No library was found...")
    ENDIF()
ENDFOREACH()