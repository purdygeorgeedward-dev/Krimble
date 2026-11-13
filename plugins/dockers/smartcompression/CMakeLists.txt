# Krimble 2026-11-13 (task 52): George: "When the user edits the selection for SmartCompression they should start with the smart selection." and "This is how XAT handled it." (the Work with Regions mode, the AutoCompress wand, Remove All).
set(kritasmartcompressiondocker_SOURCES
    SmartCompression.h
    SmartCompression.cpp
    SmartCompressionDock.h
    SmartCompressionDock.cpp
)

kis_add_library(kritasmartcompressiondocker MODULE ${kritasmartcompressiondocker_SOURCES})
target_include_directories(kritasmartcompressiondocker PRIVATE ${CMAKE_SOURCE_DIR}/plugins/impex/jpeg)
target_link_libraries(kritasmartcompressiondocker kritaimage kritaui)
install(TARGETS kritasmartcompressiondocker DESTINATION ${KRITA_PLUGIN_INSTALL_DIR})
