cwd=$(pwd)
optirun=$(eval which optirun)
if [ -n "$optirun" ]
then
  LD_LIBRARY_PATH=$cwd optirun ./GameLauncher
else
  LD_LIBRARY_PATH=$cwd ./GameLauncher
fi
