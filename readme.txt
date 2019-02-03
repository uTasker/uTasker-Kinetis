

The uTasker project includes the "uTaskerV1.4" application (complete configurable application project with OS, TCP/IP stack, USB stack, utFAT file system, command line shell [on UART, USB-CDC and Telnet] and various peipheral demonstrations), the "uTasker Serial loader" (configurable standalone UART SREC, iHEX, USB-MSD, USB-CDC, Kboot, SD card, Ethernet boot loaders) and the Bare-Minimum uTasker boot loader.

It can be buid with Visual Studio (eg. VS 2015 Community) - which allows project and device simulation - or with various IDEs and cross-compilers:
- IAR, Keil uVision, Rowley Crossworks, CooCox, Green Hills, MCUXpresso, KDS, S32 Design Studio, Atollic and GCC make file

*********************************
STM32 Application support is now included (making Kinetis and STM32 projects compatible), as Visual Studio simulation, IAR, GCC and Atollic targets.
STM32 Serial Loader support is now included (making Kinetis and STM32 projects compatible), as Visual Studio simulation, IAR and GCC targets.
Users of the Arduino Blue Pill can see the USB-MSD loader video at: https://youtu.be/dq-m-Dokq7E
*********************************


Eclipse based IDEs:

It is possible to work with CodeWarrior10.x, Atollic, S32 Design Studio, MCUXpresso or Kinetis Design Studio.
These are however all based on Eclipse and need their project configuration files to be located at the top of the project directory structure, meaning that only one can be used at a time.

To work with the one of your choice, delete the original project files at the top of the project directory. Then copy the ones in this folder to that location, import into the Eclipse based IDE that you are to work with (matching this folder's contents).
Update the copy of the project files here if needed so that they can later be restored when working with the project again, after having changed to a different IDE in the meantime.


Documentation and videos:
http://www.utasker.com/docs/documentation.html
To get started downloading, importing, building see https://www.youtube.com/watch?v=K8ScSgpgQ6M&index=9&list=PLWKlVb_MqDQFZAulrUywU30v869JBYi9Q


The uTasker Open Source project is supplied for free use, including for commercial applications. It is a contribution from the main uTasker project which is generally 1 or 2 years more advanced than the open source version. The open source version is not supported (reports of problems will however be taken seriously and corrected). Users who like it and find that it allows them to save development time and costs are urged to consider the main project for their next commercial development, which is fully supported at a very moderate fee that helps to finance continued development and thus more interesting contributions in the future to the free maker community.

The majority of the project content has originated from industrial developments during a period from around 2002 to the present (with new HW support being added as new families of interest arrived) and so the majority of the work is mature and well tested for reliability.

The author of the code works as a professional embedded firmware engineer with >30 years experience on many projects and with customers throughout the world. In additional to development services and remote training he offers remote debugging help to efficiently solve problems in existing projects. More details and contact information can be found at http://www.utasker.com/services.html



