########### install files ###############

if (FOUNDATION_BUILD)
    install(FILES
        BreezeDark.colors
        BreezeHighContrast.colors
        BreezeLight.colors
         ${schemefiles} DESTINATION ${KDE_INSTALL_DATADIR}/color-schemes
)
endif()

install(FILES
# Krimble 2026-11-15: George: "No theme should have the word Krita in it." The theme files are named Krimble*.colors; the old themes are deleted.
KrimbleBlueGray.colors
KrimbleDarkGray.colors   # Krimble 2026-11-15: the theme "Krimble Dark Gray" (flat, no bevels)
KrimblePainted.colors
KrimbleLight.colors      # Krimble 2026-11-22: the light theme
 ${schemefiles} DESTINATION ${KDE_INSTALL_DATADIR}/color-schemes
)

