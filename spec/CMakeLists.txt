set(CEPTR_SPEC_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/accumulator_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ceptr_specs.c
        ${CMAKE_CURRENT_SOURCE_DIR}/def_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/group_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/http_example.h
        ${CMAKE_CURRENT_SOURCE_DIR}/label_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/mtree_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/process_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/profile_example.h
        ${CMAKE_CURRENT_SOURCE_DIR}/protocol_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/receptor_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scape_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/semtable_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/semtrex_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/spec_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/stream_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/test_framework.h
        ${CMAKE_CURRENT_SOURCE_DIR}/tree_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/vmhost_spec.h
        ${CMAKE_CURRENT_SOURCE_DIR}/fmemopen.c
        ${CMAKE_CURRENT_SOURCE_DIR}/fmemopen.h
        ${CMAKE_CURRENT_SOURCE_DIR}/memstream.c
        ${CMAKE_CURRENT_SOURCE_DIR}/memstream.h
        ${CEPTR_SOURCE_FILES}
        )

include_directories(../src)

add_executable(ceptr_spec ${CEPTR_SPEC_SOURCE_FILES})