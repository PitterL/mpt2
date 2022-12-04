======================
CRCSCAN driver
======================

A Cyclic Redundancy Check (CRC) takes a data stream of bytes as input and generates a checksum.
The CRC peripheral can be used to detect errors in program memory.
Typically, an n-bit CRC, applied to a data block of arbitrary length, will detect any single error burst not
longer than n bits (i.e. any single alteration that spans no more than n bits of the data), and will detect a
fraction 1-2-n of all longer error bursts.

Features
--------
* Initialization

Applications
------------
* Data verification method 
* Detect errors occurring during transmission, storage, or retrieval 

Dependencies
------------
* CLKCTRL for clock
* CPUINT for Interrupt
* UPDI for debug

Note
----
* ISR will be generated only when Global Interrupt checkbox and driver_isr Harness checkbox are enabled

Concurrency
-----------
N/A

Limitations
-----------
N/A

Knows issues and workarounds
----------------------------
N/A

