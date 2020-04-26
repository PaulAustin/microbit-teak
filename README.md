# microbit-teak

A teak runtime for the trashbots robotics platform.

```
yt target bbc-microbit-classic-gcc
yt clean
yt build
```

This project is typically built using the MBED offline tool chain based on yotta.
More instructions are available here:

http://lancaster-university.github.io/microbit-docs/offline-toolchains/#yotta

For OSX, new builds can be installed onto the micro:bit by copying the HEX file to the
micro:bit's pseudo USB drive. If only one device is plugged in the following command
should work well.

```
cp ./build/bbc-microbit-classic-gcc/source/microbit-teak-combined.hex /Volumes/"MICROBIT"
```

To update the yotta modules such as microbit-dal, bluetooth, nrf51. Use the Yotta update
command.
```
yt update
```
Note, I seen scenarios where file system permission have gotten confused. Most likely by doing
an update with sudo. If you get access errors, you may need to fix ownership settings with chown.
check `~/.yotta`, `project/.yotta_modules`, and `project/.yotta_targets`
for correct file ownership  

installing yotta on catalina ( and perhasp later)
Here are some suggestions. Python installaions can be a bit complex. 
(0) I installed the latest python from python org (this gave me pip and pip3)

```
pip3 install virtualenv
```

(1) set up a virtualenv for you user account connected to python 2.7
(2) with that environment active install yotta into the virtual env
(3) install brew for the mac and some tools necessary for embedded AR  cross development.

```
virtualenv -p /usr/bin/python2.7 venv27 
source ./venv27/bin/activate   
pip install yotta

brew tap ARMmbed/homebrew-formulae 
brew install python cmake ninja arm-none-eabi-gcc    
brew insall srecord
```

SDG - Paul 