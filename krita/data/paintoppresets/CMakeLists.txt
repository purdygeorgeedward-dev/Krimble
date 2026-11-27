# Krimble 2026-11-15: George: "we don't need more than about 5 brushes. Delete the others." The watercolor presets are deleted; the five that stay are listed below.
install( FILES
a\)_Eraser_Circle.kpp
b\)_Basic-5_Size_default.kpp
b\)_Basic-5_Size_Opacity.kpp
b\)_Airbrush_Soft.kpp
c\)_Pencil-2.kpp
d\)_Ink-2_Fineliner.kpp
# Krimble 2026-11-22: George: "Build some perfect brushes to simulate these professional tools. Add them to the brush set." (HB lead on Bristol, technical pens, brush pens, and a non-photo blue pencil; made by tools/brushgen/gen.py)
c\)_Pencil_HB.kpp
c\)_Pencil_Blue_Non-Photo.kpp
d\)_Ink_Technical_Pen.kpp
d\)_Ink_Brush_Pen.kpp
# Krimble 2026-11-22: George: "A good art marker for colorists is a good idea too." / "Design markers were special. They were a high bleed marker that made perfect uniform flat tones."
e\)_Marker_Art.kpp
DESTINATION  ${KDE_INSTALL_DATADIR}/krita/paintoppresets)

install( FILES
    "Favorites.tag"
    "Pixel_Art.tag"
    Digital.tag        
    Erasers.tag
    FX.tag   
    Ink.tag  
    Paint.tag        
    Sketch.tag
    Textures.tag
DESTINATION  ${KDE_INSTALL_DATADIR}/krita/paintoppresets)
