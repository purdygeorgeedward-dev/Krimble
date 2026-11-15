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
# KritaNeutral.colors   # Krimble 2026-11-15: George: "We should remove the Krita themes. The less Krita the better." Not installed, so not in the theme menu (the file stays in the repository)
# KritaBlender.colors   # Krimble 2026-11-15: George: "We should remove the Krita themes. The less Krita the better." Not installed, so not in the theme menu (the file stays in the repository)
# KritaBright.colors   # Krimble 2026-11-15: George: "We should remove the Krita themes. The less Krita the better." Not installed, so not in the theme menu (the file stays in the repository)
# KritaDark.colors   # Krimble 2026-11-12: renamed KritaBlue.colors (the scheme "Krimble Blue"); the old palette is the new KritaBlueGray.colors (the scheme "Krimble Blue-Gray")
# KritaBlue.colors   # Krimble 2026-11-12 (task 4 of George's list): George: "Delete Krimble Blue. It looks bad." The theme is no longer installed, so it is not in the theme menu (the file stays in the repository)
KritaBlueGray.colors
KritaDarkGray.colors   # Krimble 2026-11-15: the theme "Krimble Dark Gray" (flat, no bevels)
KritaBevel.colors   # Krimble 2026-11-13 (task 63): the theme "Krimble Bevel" (the Nov 10 look), split off from Blue-Gray
# KritaDarkOrange.colors   # Krimble 2026-11-15: George: "We should remove the Krita themes. The less Krita the better." Not installed, so not in the theme menu (the file stays in the repository)
# KritaDarker.colors   # Krimble 2026-11-15: George: "We should remove the Krita themes. The less Krita the better." Not installed, so not in the theme menu (the file stays in the repository)
KritaPainted.colors
 ${schemefiles} DESTINATION ${KDE_INSTALL_DATADIR}/color-schemes
)

