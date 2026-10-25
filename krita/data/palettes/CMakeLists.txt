
########### install files ###############

install( FILES  
    concept-cookie.gpl
    pixelart-16.gpl
    pixelart-32.gpl
    default.gpl
    gradient.gpl
    marker.gpl
    ps.gpl
    swatch-cmyk.gpl
    swatch-colored-grey.gpl
    swatche.gpl
    swatch-rgb.gpl
    web.gpl
    animation-color-set.gpl
    grayscale.gpl
    black-and-white.gpl
    # Krimble 2026-10-24: retro palettes for Indexed Color (tools/palettes/make_retro_palettes.py)
    mac-8bit-256.gpl
    vga-256.gpl
    cga-16.gpl
    windows-16.gpl
    ega-64.gpl
    commodore-64-16.gpl
    game-boy-4.gpl
    amiga-workbench-4.gpl
    grayscale-16.gpl
DESTINATION ${KDE_INSTALL_DATADIR}/krita/palettes)
