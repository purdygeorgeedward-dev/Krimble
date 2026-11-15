set (krita_BUNDLE_FILES
     README
     Krita_4_Default_Resources.bundle
#     Krita_Artists_SeExpr_examples.bundle   # Krimble 2026-11-15 (609th): not installed (startup time)
#     RGBA_brushes.bundle   # Krimble 2026-11-15 (609th): not installed (startup time)
)

if (NOT ANDROID)
    set (krita_BUNDLE_FILES
         ${krita_BUNDLE_FILES}
         Krita_3_Default_Resources.bundle
         )
endif()

install( FILES
    ${krita_BUNDLE_FILES}
    DESTINATION  ${KDE_INSTALL_DATADIR}/krita/bundles)
