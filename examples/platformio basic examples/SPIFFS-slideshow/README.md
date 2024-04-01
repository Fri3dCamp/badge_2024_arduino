This sample code uses TFT_eSPI and SPIFFS to read bitmaps from the flash filesystem and display them in sequence.
Each bitmap must be uncompressed, same dimensions as the screen (296*240 in my case) and use 16-bit colors (5R, 6G, 5B)

The example now uses pushImageDMA for DMA transfers (turns out you need to encapsulate DMA transfer with beginWrite() and endWrite(),
but there is no speedup because I'm not doing anything in parallel. Max speed seems to be 10 FPS.

Currently, the slideshow is restarted by closing root file object and opening it again. This seems to slow down the algorithm a lot.
To get the max speed of 10 FPS, I needed to keep all bmp filenames in an array and iterate the array rather than iterate the root file object.

# Instructions:
1. open in VSCode + PlatformIO
2. upload code
3. If you want, change the bitmap files in the "data folder"
4. goto PlatformIO window (the icon that looks like an alien)
5. click "upload filesystem"