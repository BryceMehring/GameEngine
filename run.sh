cwd=$(pwd)
if hash optirun 2>/dev/null; then
  LD_LIBRARY_PATH=$cwd optirun ./GameLauncher
else
  LD_LIBRARY_PATH=$cwd ./GameLauncher
fi
