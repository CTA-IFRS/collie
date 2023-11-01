
@ for /r src %%f in (*.cpp) do @ nmake /f Makefile.win "build\%%~nf.obj"
@ for /r src %%f in (*.cxx) do @ nmake /f Makefile.win "build\%%~nf.obj"
@ nmake /f Makefile.win exe

