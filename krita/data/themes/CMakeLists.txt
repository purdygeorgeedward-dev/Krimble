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
KritaNeutral.colors
KritaBlender.colors
KritaBright.colors
# KritaDark.colors   # Krimble 2026-11-12: renamed KritaBlue.colors (the scheme "Krimble Blue"); the old palette is the new KritaBlueGray.colors (the scheme "Krimble Blue-Gray")
KritaBlue.colors
KritaBlueGray.colors
KritaDarkOrange.colors
KritaDarker.colors
KritaPainted.colors
 ${schemefiles} DESTINATION ${KDE_INSTALL_DATADIR}/color-schemes
)

