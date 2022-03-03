AUX_SOURCE_DIRECTORY(. APP_LIST)

FILE(GLOB LIB_LIST 
        ${PROJECT_SOURCE_DIR}/lib/*.dll 
        ${PROJECT_SOURCE_DIR}/lib/*.lib 
        ${PROJECT_SOURCE_DIR}/lib/*.a
)
MESSAGE("${LIB_LIST}")

FOREACH(app ${APP_LSIT})
    GET_FILENAME_COMPONENT(app_name ${app} NAME_WE)
    MESSAGE("Found application ${app}")
    ADD_EXECUTABLE(${app_name} ${app})

    IF(LIB_LIST)
        FOREACH(lib ${LIB_LIST})
            GET_FILENAME_COMPONENT(lib_name ${lib} NAME_WE)
            MESSAGE("Linking library ${lib_name}")
            TARGET_LINK_LIBRARIES(${app_name} ${lib_name})
        ENDFOREACH()
    ELSE()
        MESSAGE("No library was found...")
    ENDIF()
ENDFOREACH()