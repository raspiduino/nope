"D:\Data\MRE SDK V3.0.00\tools\DllPackage.exe" "D:\Data\dev\mre\nope\nope.vcproj"
if %errorlevel% == 0 (
 echo postbuild OK.
  copy nope.vpp ..\..\..\MoDIS_VC9\WIN32FS\DRIVE_E\nope.vpp /y
exit 0
)else (
echo postbuild error
  echo error code: %errorlevel%
  exit 1
)

