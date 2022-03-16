rem This is a batch file for Windows machines with ImageMagick installed and on the PATH.
rem It converts each PPM file in the current folder to the TIF format
rem and then DELETES the original PPM file, unless its filename ends with "avg.ppm"

setlocal EnableDelayedExpansion

for %%i in (*.ppm) do (
	convert "%%~ni.ppm" "%%~ni.tif"
	set filename=%%~ni
	set last_three_letters=!filename:~-3!
	if not "!last_three_letters!" == "avg" del "%%~ni.ppm"
)