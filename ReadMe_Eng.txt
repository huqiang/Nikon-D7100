Nikon Type0010 Module SDK Revision.1.0 summary


Usage
 Control the camera.


Supported camera
 D7100


Environment of operation
 [Windows]
    Windows XP (SP3)
    (*  Professional, Home Edition)
    Windows Vista (SP2) -- 32bit / 64bit(Compatibility mode)
    (*  Ultimate, Enterprise, Business, Home Premium, Home Basic)
    Windows 7 (SP1) -- 32bit / 64bit(Compatibility mode)
    (*  Ultimate, Enterprise, Professional, Home Premium, Home Basic)
    Windows 8 -- 32bit / 64bit(Compatibility mode)

 [Macintosh]
    MacOS X 10.6.8
    OS X 10.7.5
    OS X 10.8.2
    * Not supported using on PowerPC/Rosetta.


Contents
 [Windows]
    Documents
      MAID3(E).pdf : Basic interface specification
      MAID3Type0010(E).pdf : Extended interface specification used 
                                                              by Type0010 Module
      Usage of Type0010 Module(E).pdf : Notes for using Type0010 Module
      Type0010 Sample Guide(E).pdf : The usage of a sample program

    Binary Files
      Type0010.md3 : Type0010 Module for Win
      NkdPTP.dll : Driver for PTP mode used by Win

    Header Files
      Maid3.h : Basic header file of MAID interface
      Maid3d1.h : Extended header file for Type0010 Module
      NkTypes.h : Definitions of the types used in this program.
      NkEndian.h : Definitions of the types used in this program.
      Nkstdint.h : Definitions of the types used in this program.

    Sample Program
      Type0010CtrlSample(Win) : Project for Microsoft Visual Studio 2010


 [Macintosh]
    Documents
      MAID3(E).pdf : Basic interface specification
      MAID3Type0010(E).pdf : Extended interface specification used by 
                                                                Type0010 Module
      Usage of Type0010 Module(E).pdf : Notes for using Type0010 Module
      Type0010 Sample Guide(E).pdf : The usage of a sample program

    Binary Files
      Type0010 Module.bundle : Type0010 Module for Mac
      libNkPTPDriver.dylib : PTP driver for Mac
 
    Header Files
      Maid3.h : Basic header file of MAID interface
      Maid3d1.h : Extended header file for Type0010 Module
      NkTypes.h : Definitions of the types used in this program.
      NkEndian.h : Definitions of the types used in this program.
      Nkstdint.h : Definitions of the types used in this program.

    Sample Program
      Type0010CtrlSample(Mac) : Sample program project for Xcode 3.2.4.


Limitations
 This module cannot control two or more cameras.
