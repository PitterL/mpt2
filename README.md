# mpt2
MCU based touch framework compatible with MaxTouch protocol, current achieved with ATTINY1617/3217 series, but could be porting on other platform

## version

v22: Object table function (basic version)
v23: Object table with lower power control supported


# #OBJECT PROTOCOL
# ##Introduction
The Object Protocol provides a single common interface across the Microchip maXTouch controllers. This allows the
different features in each controller to be configured in a consistent manner. This makes the future expansion of features
and simple product upgrades possible, whilst allowing backwards compatibility for the host driver and application code.
The protocol is designed to control the processing chain in a modular manner. This is achieved by breaking the features
of the device into objects that can be controlled individually. Each object represents a certain feature or function of the
device, such as a touchscreen. Where appropriate, objects can be disabled or enabled as needed.
The Object Protocol is designed in such a way that a driver can be “future-proofed”

https://www.microchip.com/content/dam/mchp/documents/OTH/ApplicationNotes/ApplicationNotes/MXTAN0213_Interfacing_with_maXTouch_Touchscreen_Controllers_00002428A.pdf
