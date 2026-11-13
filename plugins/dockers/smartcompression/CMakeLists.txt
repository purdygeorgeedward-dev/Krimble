# Krimble 2026-11-13 (task 52, 55): the SmartCompression panel
set(kritasmartcompressiondocker_SOURCES
    SmartCompression.h
    SmartCompression.cpp
    SmartCompressionDock.h
    SmartCompressionDock.cpp
    SmartCompressionPanel.h
)

kis_add_library(kritasmartcompressiondocker MODULE ${kritasmartcompressiondocker_SOURCES})
target_include_directories(kritasmartcompressiondocker PRIVATE ${CMAKE_SOURCE_DIR}/plugins/impex/jpeg)
target_include_directories(kritasmartcompressiondocker SYSTEM PRIVATE ${JPEG_INCLUDE_DIR})
target_link_libraries(kritasmartcompressiondocker kritaimage kritaui ${JPEG_LIBRARIES})
install(TARGETS kritasmartcompressiondocker DESTINATION ${KRITA_PLUGIN_INSTALL_DIR})
