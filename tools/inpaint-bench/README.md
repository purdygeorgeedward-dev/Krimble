# Content-aware fill: test bench (Krimble 2026-10-18)

A way to try `plugins/tools/tool_smart_patch/krimble_inpaint.cpp` (the engine of Edit > Fill > Content-Aware...) on its own, without Krita or Android, and to compare it with a plain blur fill (OpenCV Telea) and OpenCV's shift-map inpainting.

Build (any Linux with g++ and Python 3 with numpy, opencv-contrib-python, pillow, scikit-image):

    g++ -O2 -std=c++17 -pthread ../../plugins/tools/tool_smart_patch/krimble_inpaint.cpp test_main.cpp -I../../plugins/tools/tool_smart_patch -o inpaint_test
    python3 -c "from skimage import data; from PIL import Image; import numpy as np; [Image.fromarray(np.asarray(getattr(data,n)())[...,:3] if np.asarray(getattr(data,n)()).ndim==3 else np.stack([np.asarray(getattr(data,n)())]*3,-1)).save(f'photo_{n}.png') for n in ['astronaut','coffee','chelsea','brick','grass','gravel','camera','stereo_motorcycle']]"
    python3 bench.py          # makes bench1.png and bench2.png (original / blur / shift-map / Krimble)

`harness.py` has `run(name, rgb, mask, expand, sharpness)`, which writes PPM/PGM files, runs `inpaint_test` and returns the result and the blur fill. `test_main.cpp` reads `image.ppm mask.pgm out.ppm [expand] [sharpness]`.
