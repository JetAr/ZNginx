@echo off

IF "%1" == "" ( echo on &  echo "usage : update CommentMsg" & goto END )

setlocal ENABLEEXTENSIONS
set TDate=%DATE:~0,10%
set THour=%TIME:~0,2%
if %THour% LSS 10 set THour=0%TIME:~1,1%
set TMin=%time:~3,2%
set TSed=%time:~6,2%
set TMs=%TIME:~-2%

set FileTime=%1.%TDate%.%THour%_%TMin%_%TSed%.%TMs%

endlocal

echo %FileTime% >> History.txt
git >>history.txt
git add .
git commit -am "%FileTime%"
git push

:END
@echo on