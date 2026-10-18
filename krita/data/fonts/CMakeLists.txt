# Krimble 2026-10-18: the fonts that come with the app.
# LICENSE: these fonts are NOT part of Krimble's GPL code. They are the work of George Edward Purdy (Purdy Design), who is also the author of
# Krimble, and are included here with the permission of their copyright holder, to be used inside Krimble. Their own license (embedded in each
# font file, and summarized in readme.md) applies to them, not the GPL.

install( FILES
    ROBO.ttf
    cartoonist_kooky.ttf
    cyberspace.ttf
    emoticons.ttf
    emoticons_outline.ttf
    readme.md
    DESTINATION ${KDE_INSTALL_DATADIR}/krita/fonts)

# Krimble 2026-10-18 (189th): open-license fonts (SIL OFL 1.1 / Apache 2.0), see open/readme.md. Installed in the same folder as the
# fonts above (krimbleLoadBundledFonts() reads only that folder, not subfolders); their licenses go to fonts/licenses.
file(GLOB KRIMBLE_OPEN_FONTS ${CMAKE_CURRENT_SOURCE_DIR}/open/*.ttf)
install( FILES ${KRIMBLE_OPEN_FONTS} DESTINATION ${KDE_INSTALL_DATADIR}/krita/fonts)
install( FILES open/readme.md DESTINATION ${KDE_INSTALL_DATADIR}/krita/fonts/licenses RENAME readme-open-fonts.md)
install( DIRECTORY open/licenses/ DESTINATION ${KDE_INSTALL_DATADIR}/krita/fonts/licenses)
