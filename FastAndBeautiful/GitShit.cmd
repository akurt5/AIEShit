@echo off
echo Close this if you dont want assisted Git help
echo Else press any key to continue
pause

set /p comment="Enter Comment for commit: "
git commit -a -m comment
git status

echo Is all good? If not please close this now!
pause
echo Are you SURE!!!
pause
git push
pause