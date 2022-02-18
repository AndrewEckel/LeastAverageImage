rem This is a batch file for Windows machines with ImageMagick installed.
rem It converts every PPM file in the current folder to the TIF format and then DELETES the original PPM files.
for %%i in (*.ppm) do convert "%%~ni.ppm" "%%~ni.tif" && del "%%~ni.ppm"