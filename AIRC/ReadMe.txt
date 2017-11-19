Arduino Infrared Remote Control(AIRC).
Written by Charles Law
Version 2.0
Feb 2016

AIRC is based on:
"A Multi-Protocol Infrared Remote Library for the Arduino"
Written by Ken Shirriff
Version 2.0.1 June, 2015
http://www.righto.com/2009/08/multi-protocol-infrared-remote-library.html
https://github.com/z3t0/Arduino-IRremote

AIRC accepts the following commands:
  Status
  RxEnable
  RxDisable
  TxHide
  TxShow
  PLHide
  PLShow
  PLGap
  PLUnknown
  ReadPulses
  Pulses
  NEC
  SharpRaw
  Sharp
  Panasonic
  Sony
  
Commands and corresponding parameters are separated by space or tab,
and they are not case sensitive.

Bin, Dec, Hex based number can be used for all numeric parameters.
Default is Dec, while Bin or Hex based number can be used with a leading "b"
or "h" respectively, eg. b1001, h7f.

Hyphen and comma can be added anywhere within a numeric parameter for
clarity. They are ignored when read by AIRC.
eg. b1000-11-11 = b10001111, 6,5,535 = 65535, hab-cd,ef = habcdef

"m" or "s" can also be added for clarity, especially when used for denoting
mark or space pulse length. They are ignored when read by AIRC.

Details for each command and corresponding parameters are described as
follows:

Status
Print Rx, Tx and PL status.

RxEnable
Default. Enable receiver.

RxDisable
Disable receiver.

TxHide
Default. Hide transmit message.

TxShow
Show transmit message.

PLUnknown
Default. Shows received pulse length only if un-decoded.

PLGap
Shows previous gap length only.

PLHide
Always hide received pulse length.

PLShow
Always show received pulse length.

ReadPulses Gap
Read incoming IR stream pulses length.
  Gap....min gap length, in us, between each IR streams.
         max = 1000000us(1s).
         default = _GAP if omitted, that is defined in IRremoteInt.h
         max. no. of pulses = RAWBUF -1(or -2 if RAWBUF is even)
         while RAWBUF is defined in IRremoteInt.h, same for Pulses.

Pulses kHz InitialEnd Repeat m1 s2 m3 s4 m5 s6....sX
Transmit and decode in pulse length format.
  kHz             int, Carrier frequency in kHz.
                  According to IRsend::enableIROut from irSend.cpp, it is
                  designed for 36~40kHz.
                  Decodes only if kHz=0.
  InitialEnd      int, End index of initial stream, including the gap space.
                  default = total no. of raw values if 0 is entered.
  Repeat          int, No. of repeat.
                  Repetitive IR stream will be send indefinitely if
                  Repeat >= 1000.
                  Repeat can be terminated when new TxCode command line is
                  sent to AIRC, same for other transmit commands.
  m1 s2...sX      Pulse length of each mark/space, in us, starting with the
                  the first mark. An extra space length of 1s will be added
                  if the final pulse length is a mark.
                  max. pulse length is 1000000us(1s).
                  max. no. of pulses = RAWBUF -1(or -2 if RAWBUF is even)
                  while RAWBUF is defined in IRremoteInt.h

NEC Data Nbits Repeat
Transmit NEC Protocol
  Data      unsigned long
  Nbits     int, optional, max = 32, default = 32 if omitted
  Repeat    int, optional, default = 0 if omitted or Nbits is
            omitted
  
SharpRaw Data Nbits Repeat
Transmit Sharp protocol in raw format
  Data      unsigned long
  Nbits     int, optional, default = 15 if omitted
  Repeat    int, optional, default = 0 if omitted or Nbits is omitted

Sharp Address Command Repeat
Transmit Sharp protocol in address and command format
  Address    unsigned int
  Command    int
  Repeat     int, optional, default = 0 if omitted

Panasonic Address Data Repeat
Transmit Panasonic protocol
  Address    unsigned int
  Data       unsigned long
  Repeat     int, optional, default = 0 if omitted

Sony Data Nbits Repeat
Transmit Sony protocol
  Data      unsigned long
  Nbits     int
  Repeat    int, optional, default = 0 if omitted
