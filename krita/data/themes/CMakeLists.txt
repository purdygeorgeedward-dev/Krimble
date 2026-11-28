########### install files ###############

if (FOUNDATION_BUILD)
    install(FILES
         ${schemefiles} DESTINATION ${KDE_INSTALL_DATADIR}/color-schemes
)
endif()

install(FILES
# Krimble 2026-11-15: George: "No theme should have the word Krita in it." The theme files are named Krimble*.colors; the old themes are deleted.
KrimbleBlueGray.colors
KrimbleWarmGray.colors   # Krimble 2026-11-15: the theme "Krimble Warm Gray" (flat, no bevels; named Dark Gray until Nov 28 2026)
KrimblePainted.colors
 ${schemefiles} DESTINATION ${KDE_INSTALL_DATADIR}/color-schemes
)

