rem This is a batch file for Windows machines with ImageMagick installed and on the PATH.
rem It converts all JPG/JPEG, TIF, HEIC, PNG, and WEBP files in the current folder to PPM,
rem without deleting anything.

for %%i in (*.jpg) do convert "%%~ni.jpg" "%%~ni.ppm"
for %%i in (*.jpeg) do convert "%%~ni.jpeg" "%%~ni.ppm"
for %%i in (*.tif) do convert "%%~ni.tif" "%%~ni.ppm"
for %%i in (*.heic) do convert "%%~ni.heic" "%%~ni.ppm"
for %%i in (*.png) do convert "%%~ni.png" "%%~ni.ppm"
for %%i in (*.webp) do convert "%%~ni.webp" "%%~ni.ppm"