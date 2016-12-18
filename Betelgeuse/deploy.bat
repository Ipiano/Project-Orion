del /Q /S deploy\*
mkdir deploy
copy /B /Y release\release\Betelgeuse.exe deploy\Betelgeuse.exe
cd deploy
windeployqt Betelgeuse.exe
7za a Betelgeuse.zip *
move /Y Betelgeuse.zip ..\Betelgeuse.zip