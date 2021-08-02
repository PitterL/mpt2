#	MPTT lowpower v25 platform

## OBJECT PROTOCOL
### Introduction
The Object Protocol provides a single common interface across the Microchip maXTouch controllers. This allows the
different features in each controller to be configured in a consistent manner. This makes the future expansion of features
and simple product upgrades possible, whilst allowing backwards compatibility for the host driver and application code.
The protocol is designed to control the processing chain in a modular manner. This is achieved by breaking the features
of the device into objects that can be controlled individually. Each object represents a certain feature or function of the
device, such as a touchscreen. Where appropriate, objects can be disabled or enabled as needed.
The Object Protocol is designed in such a way that a driver can be “future-proofed”

For Generated protocol, you could refer below link.
https://www.microchip.com/content/dam/mchp/documents/OTH/ApplicationNotes/ApplicationNotes/MXTAN0213_Interfacing_with_maXTouch_Touchscreen_Controllers_00002428A.pdf

MPTT discributed protocol:
https://github.com/PitterL/mpt2/blob/EVK_3217_Xpro/doc/MCU%20Based%20Maxtouch%20Framework%20and%20Protocol%20-%20V1.01.pdf

For detailed protocol, please request the protocol file `MCU Based Maxtouch Framework and Protocol - MPTT Vxxx` for us.

The hardware based on ATtiny3217 Xplained Pro board:

https://www.microchip.com/developmenttools/ProductDetails/ATTINY3217-XPRO

The v25 platformn embedded T126 object to control lowpower function, please check protocol for more details:

### T6 message:
- byte[0] ID: Report ID
- byte[1] STATUS: 
	- bit[3] CFGERR: the config crc checked mismatch
	- bit[4] CAL: in calibration or calibration failed
	- bit[5] SIGERR: the Fuse checked mismatch with packed firwmare
	- bit[6] OFL: message overflow
	- bit[7] RESET: chip reseting

### T7 configure:
- byte[2] ACTV2IDLETO: Active to idle timeout

### T126 configure:
- byte[0] CTRL: 
	- bit[0] enable 
	- bit[1] T126 Wake up message report enabled when low power status message changed.
	- bit[2] Enable T15 message for Non-Low power button in diel mode.
	- bit[3] Enables T15 message reporting for Low power button in idle mode.
	- bit[4] Enables T15 message reporting for Low Power Button in active mode.
- byte[1] Node: wakeup node(For `Event system sleep`, this is the node channel; for `software sleep`, this is node channels mask, which supports multi channel wakeup)
- byte[3] THRESHOLD: wakeup threshold of the key( in `Event system mode` only, this byte is not use in `software sleep` mode).
- byte[5] DRIFT: Drift time interval(200ms)

### T126 message:
- byte[0] ID: Report ID
- byte[1] STATUS: Wakeup message type
- byte[2-3] DELTA: Wakedup delta value 

### T126 Diagnostic debug:
- [T6 debug command: 0x3B]
- Diagnostic Debug T37 Data – Low Power Mode
	- byte [0–1]: Low power idle delta
	- byte [2–3]: Low power idle reference level
	- byte [4–5]: Raw low power idle signal
	- byte [6]: Status:
		- 0 = device in idle mode
		- 1 = device in active mode
	- byte [7–8]: Raw (unfiltered) low power signal


## Power comsuption measured:
<img src="https://github.com/PitterL/mpt2/blob/EVK_3217_Xpro/doc/Power%20consumption%20measured.png" />



## Other tools:

Maxtouch studio:

https://www.microchip.com/en-us/products/touch-and-gesture/solutions-for-touchpads-and-touchscreens/maxtouch-touchscreen-controllers

Chip xml load file for maxtouch studio:
https://github.com/PitterL/mpt2/blob/EVK_3217_Xpro/doc/mptt_t25_3.xml

## Porting guide:
https://github.com/PitterL/mpt2/blob/EVK_3217_Xpro/doc/MPTT%20architecture%20update(v25)%2020210716.pdf
