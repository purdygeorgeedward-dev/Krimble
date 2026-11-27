########### install files ###############
add_subdirectory(elles-icc-profiles)
# add_subdirectory(ycbcr-icc-profiles)   # Krimble 2026-11-21 (819th): not installed (startup time; "Cut 4 and 5"); files stay in the repository
install(FILES
    README
    scRGB.icm
    cmyk.icm
    krita25_lcms-builtin-sRGB_g100-truegamma.icc
    DESTINATION ${KDE_INSTALL_DATADIR}/color/icc/krita)
