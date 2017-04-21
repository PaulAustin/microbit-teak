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

file ownership  
