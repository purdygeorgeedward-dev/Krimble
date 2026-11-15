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
# KrimbleNeutral.colors   # Krimble 2026-11-15: George: "We should remove the old themes." Not installed, so not in the theme menu (the file stays in the repository)
# KrimbleBlender.colors   # Krimble 2026-11-15: George: "We should remove the old themes." Not installed, so not in the theme menu (the file stays in the repository)
# KrimbleBright.colors   # Krimble 2026-11-15: George: "We should remove the old themes." Not installed, so not in the theme menu (the file stays in the repository)
# KrimbleDark.colors   # Krimble 2026-11-12: renamed KrimbleBlue.colors (the scheme "Krimble Blue"); the old palette is the new KrimbleBlueGray.colors (the scheme "Krimble Blue-Gray")
# KrimbleBlue.colors   # Krimble 2026-11-12 (task 4 of George's list): George: "Delete Krimble Blue. It looks bad." The theme is no longer installed, so it is not in the theme menu (the file stays in the repository)
KrimbleBlueGray.colors
KrimbleDarkGray.colors   # Krimble 2026-11-15: the theme "Krimble Dark Gray" (flat, no bevels)
KrimbleBevel.colors   # Krimble 2026-11-13 (task 63): the theme "Krimble Bevel" (the Nov 10 look), split off from Blue-Gray
# KrimbleDarkOrange.colors   # Krimble 2026-11-15: George: "We should remove the old themes." Not installed, so not in the theme menu (the file stays in the repository)
# KrimbleDarker.colors   # Krimble 2026-11-15: George: "We should remove the old themes." Not installed, so not in the theme menu (the file stays in the repository)
KrimblePainted.colors
 ${schemefiles} DESTINATION ${KDE_INSTALL_DATADIR}/color-schemes
)

