/**
  **************************************************************************
  * File   : readme.txt
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : USB Aduio Composite Speaker and Microphone.
  **************************************************************************
  */

Description
****************
  The AT32 USB_Device Audio Composite Speaker and Micphone gives example how
  to use the AT32F4xx USB peripheral to communicate with PC host in the 
  isochronous transfer mode.
  This example implementation composite device include USB Speaker and USB
  Microphone.It can play voice and record voice.   
  
  The Demo software implementation is given in the "UM_USB_Audio_Composite_Speaker_Microphone_V1.0".

Directory contents
******************
  + \inc: contains the demo header files
  + \MDK-ARM: contains preconfigured projects for MDK-ARM toolchain
  + \src: contains the demo source files
  + \wm8988-driver: contains the drivers of wm8988

Hardware Board
******************
  details of this board is given in the "AT_START_F403" and "AT_START_F413".

  The Demo Base on the "Audio Arduino Demo Board",More details about this
  demo board implementation is given in the "UM_Audio_Arduino_Daughter_Board_V1.0".

How to use it
*****************
  \MDK-ARM
  - Open the Audio_Composite project
  - Rebuild all files
  - Load project iamge
  - Run program
  - Connect USB device to PC Host
  - Play voice on PC or record voice use windows recorder
  More details of how to use is given in "AT32 USB Audio Development_NoteV1.0.0.pdf"
