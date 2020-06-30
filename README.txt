Hello friend!

I included the finished program and also the source if you'd like to modify the code or just compile it yourself.

PC_TO_EEPROM.exe sends a byte to a serial port and waits to receive back a byte, checks that the sent byte == received byte, and sends the next one.
PC_TO_EEPROM.ino waits to receive a byte from serial, writes it to the EEPROM, reads it and sends it back to the serial for the computer to check.

Use PC_TO_EEPROM as follows:
first put the .ino on your arduino, it's based on ben's EEPROM programmer modified to take serial input.

then put the file (I recommend a .bin but it'll read any file and turn it into hexadecimal bytes) 
that you want to be written to the EEPROM in the same folder as the .exe

then:
Option one: 
just double click the exe, tell it the filename, and number of COM port where your Arduino is (check this from device manager if needed)


Option two:
open a command prompt (console,  terminal, cmd.exe whatever you want to call it)
cd into the folder and type the following:
PC_TO_EEPROM file_name port
where file_name and port are replaced by the file that you want to be written and the number of your COM port



IMPORTANT:
The error: something went wrong! basically means that the byte the program sent is not equal the byte the arduino sent back,
could mean that there was an error in transtmitting or the arduino didn't write to the EEPROM correctly.
I didn't get reliable writing to the EEPROM until I added two big capacitors to the power rails on the arduino's breadboard
I was using only the USB's power so if you are on an external power source it might not be a problem, haven't tested myself 

both the C program and arduino program are hard coded to 19200 baud rate, you can change these if you want of course

Serial error codes:
02: no serial device found, check that your arduino is connected and you have the right port number
05: port busy, close the arduino IDE's serial monitor 

