
include_directories(
    ${CMAKE_SOURCE_DIR}/plan
    ${CMAKE_SOURCE_DIR}/plan/libs/kernel
    ${KOMAIN_INCLUDES}
)

set(kplatoimport_PART_SRCS 
   kplatoimport.cpp
)


add_library(plankplatoimport MODULE ${kplatoimport_PART_SRCS})
calligra_filter_desktop_to_json(plankplatoimport plan_kplato_import.desktop)

target_link_libraries(plankplatoimport planprivate kplatokernel komain)

install(TARGETS plankplatoimport  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)
