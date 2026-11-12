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
# KritaBlue.colors   # Krimble 2026-11-12 (task 4 of George's list): George: "Delete Krimble Blue. It looks bad." The theme is no longer installed, so it is not in the theme menu (the file stays in the repository)
KritaBlueGray.colors
KritaDarkOrange.colors
KritaDarker.colors
KritaPainted.colors
 ${schemefiles} DESTINATION ${KDE_INSTALL_DATADIR}/color-schemes
)

