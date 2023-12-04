# A script to read the contents of an Apple ProFile hard drive

This is a collection of scripts to read any data off of an old Apple ProFile hard drive.

WARNING! I have run this script exactly twice. While it produced the same results on both occasions, I make no guarantees it will work for your system. My timing is probably way off of what's expected. This might damage your drive, I don't know. I don't think it damaged mine, but I wouldn't know if it did.

## Setup

You will need:

* A functional (for 30 minutes) ProFile hard drive, for an Apple III or Lisa
* An arduino, of (probably) any kind, or a clone, of any kind
* A DB-25 breakout connector
* Some wires to connect everything together

## Connect it all

Connect the arduino to the DB-25 breakout connector:
* Pin 2 on the Arduino reads the 0th bit of data, and connects to Pin 5 on the DB-25
* Pin 3 on the Arduino reads the 1th bit of data, and connects to Pin 6 on the DB-25
* Pin 4 on the Arduino reads the 2th bit of data, and connects to Pin 8 on the DB-25
* Pin 5 on the Arduino reads the 3th bit of data, and connects to Pin 22 on the DB-25
* Pin 6 on the Arduino reads the 4th bit of data, and connects to Pin 23 on the DB-25
* Pin 7 on the Arduino reads the 5th bit of data, and connects to Pin 11 on the DB-25
* Pin 8 on the Arduino reads the 6th bit of data, and connects to Pin 12 on the DB-25
* Pin 9 on the Arduino reads the 7th bit of data, and connects to Pin 13 on the DB-25
* Pin 10 on the Arduino carries the CMD signal, and connects to Pin 17 on the DB-25
* Pin 11 on the Arduino carries the PSTRB signal, and connects to Pin 15 on the DB-25
* Pin 12 on the Arduino carries the DRW signal, and connects to Pin 3 on the DB-25
* Pin 13 on the Arduino carries the CRES signal, and connects to Pin 21 on the DB-25
* Pin 14 on the Arduino carries the BSY signal, and connects to Pin 16 on the DB-25
* Pin 15 on the Arduino carries the PARITY signal, and connects to Pin 18 on the DB-25

## Run the extraction script

From within the Arduino IDE, compile and run the read.c. Open the Serial Terminal; it should be waiting for a keyboard input. After sending any key, it will begin reading the ProFile block by block. It should take ~30 minutes to complete.

Save the output to a file called profile.bin, and then run parse.py. This will make a file disk.img with the bytes on the disk. It will also verify there are no errors that occurred during reading. If there were any errors you should re-run the arduino script.

## License

GPL v3