@echo off
FOR /R %%G in (.) DO (
Pushd %%G
echo %%G
RD /S /Q ".\ipch"
RD /S /Q ".\debug"
RD /S /Q ".\release"
Popd )
del "*.sdf"
del "*.suo"
echo All done. should be clean for copy :D
pause