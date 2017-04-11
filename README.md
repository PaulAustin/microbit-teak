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
cp ./build/bbc-microbit-classic-gcc/source/microbit-samples-combined.hex /Volumes/"MICROBIT"
```
