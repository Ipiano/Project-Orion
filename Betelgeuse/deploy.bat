del /Q /S deploy\*
copy /B /Y release\release\Betelgeuse.exe deploy\Betelgeuse.exe
cd deploy
windeployqt Betelgeuse.exe
7za a Orion.zip *