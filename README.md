# PGM median filter

Applies a median filter with a 5x1 window to PGM images

Usage:
```
./median_filter - run the example mode for a 256x256 image
./median_filter example <size_x> <size_y> - generates a random <size_x> by <size_y> gray image (saved as pgm_noisy.pgm) and filters it (saved as pgm_filtered.pgm)
./median_filter file <input_path> <output_path> - filter a pgm image
./median_filter file_noise <input_path> <output_path> - filter a pgm image with additional noise (noisy image will be saved as pgm_filtered.pgm)
```
