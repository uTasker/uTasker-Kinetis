The uTasker project includes the "uTaskerV1.4" application (complete configurable application project with OS, TCP/IP stack, USB stack, utFAT file system, command line shell [on UART, USB-CDC and Telnet] and various peipheral demonstrations), the "uTasker Serial loader" (configurable standalone UART SREC, iHEX, USB-MSD, USB-CDC, Kboot, SD card, Ethernet boot loaders) and the Bare-Minimum uTasker boot loader.

It can be buid with Visual Studio (eg. VS 2015 Community) - which allows project and device simulation - or with various IDEs and cross-compilers:
- IAR, Keil uVision, Rowley Crossworks, CooCox, Green Hills, MCUXpresso, KDS, S32 Design Studio, Atollic and GCC make file



Eclipse based IDEs:

It is possible to work with CodeWarrior10.x, Atollic, S32 Design Studio, MCUXpresso or Kinetis Design Studio.
These are however all based on Eclipse and need their project configuration files to be located at the top of the project directory structure, meaning that only one can be used at a time.

To work with the one of your choice, delete the original project files at the top of the project directory. Then copy the ones in this folder to that location, import into the Eclipse based IDE that you are to work with (matching this folder's contents).
Update the copy of the project files here if needed so that they can later be restored when working with the project again, after having changed to a different IDE in the meantime.


Documentation and videos:
http://www.utasker.com/docs/documentation.html
To get started downloading, importing, building see https://www.youtube.com/watch?v=K8ScSgpgQ6M&index=9&list=PLWKlVb_MqDQFZAulrUywU30v869JBYi9Q


The uTasker Open Source project is supplied for free use, including for commercial applications. It is a contribution from the main uTasker project which is generally 1 or 2 years more advanced than the open sourec version. The open source version is not supported (reports of problem will however be taken seriously and corrected). Users who like it and find that it allows them to save development time and costs are urged to consider the main project for their next commercial development, which is fully supported at a very moderate fee that helps to finance continued development and thus more interesting contributions in the future to the free maker community.


