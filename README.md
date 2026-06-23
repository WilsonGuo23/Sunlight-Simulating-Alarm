# Sunlight-Simulating-Alarm

A microcontroller-based alarm clock that simulates a natural sunrise by gradually increasing light intensity as the configured wake-up time approaches. The system uses a TRIAC-based AC dimmer to control a dimmable daylight bulb, helping create a more gradual and comfortable waking experience.

## Features
* Configurable alarm time
* Sunrise simulation with gradual brightness increase
* Real-time clock synchronization
* TM1637 7-segment display for time and alarm settings
* Push-button user interface
* TRIAC-based AC light dimming control
* Custom 3D-printed enclosure

# Schematic
<img width="2075" height="1318" alt="Sunlight Alarm Schematic" src="https://github.com/user-attachments/assets/a134af7a-e7d0-4519-a21f-105d2a449b64" />

## Hardware
| Quantity | Component                      |
| -------- | ------------------------------ |
| 1        | Raspberry Pi Pico              |
| 1        | TRIAC Dimmer Module            |
| 1        | TM1637 Display Module          |
| 1        | Dimmable Daylight Bulb         |
| 1        | Power Strip with USB Port      |
| 1        | 1-In / 2-Out Terminal Block    |
| 5        | 24 mm Push Buttons             |
| 5        | 110 mm Quick-Disconnect Crimps |
| 2 ft     | 20 AWG Copper Wire             |
| 0.5 ft   | 16 AWG Copper Wire             |

## Fasteners
| Quantity | Component          |
| -------- | ------------------ |
| 4        | M3 × 25 mm Screws  |
| 10       | M3 × 20 mm Screws  |
| 4        | M3 × 15 mm Screws  |
| 4        | M3 × 10 mm Screws  |
| 4        | M2.5 × 5 mm Screws |
| 4        | M2 × 10 mm Screws  |
| 10       | M3 Nuts            |
| 4        | M2.5 Nuts          |
| 4        | M2 Nuts            |
| 4        | M3 Connector Bolts |

## 3D Printed Parts
| Quantity | Part               |
| -------- | ------------------ |
| 4        | Pico Standoffs     |
| 4        | TRIAC Standoffs    |
| 1        | Bottom Panel       |
| 1        | Light Bulb Bracket |
| 1        | Mounting Bracket   |
| 1        | Lower Middle Wall  |
| 1        | Upper Middle Wall  |
| 1        | Top Panel          |


# Assembly
Engineering Drawings can be found in the "Mechanical Schematics" folder, you can also use the assembly in the SLDPRT Files as a reference intended assembly
Assembly Instructions

Engineering drawings can be found in the Mechanical Schematics directory. The SolidWorks assembly files (SLDPRT and assembly files) may also be used as references during assembly.

## 1. Install Electronics
Mount the Raspberry Pi Pico and TRIAC dimmer to the mounting bracket using:
M2 × 10 mm screws and M2 nuts (Pico)
M3 × 10 mm screws and M3 nuts (TRIAC dimmer)
Appropriate standoffs
Ensure the AC terminal connections on the TRIAC dimmer face outward from the mounting bracket for easy wiring access.
Secure the mounting bracket to the bottom panel using M3 × 15 mm screws and M3 nuts.
## 2. Install Power Strip
Insert the power strip into the rectangular opening in the bottom panel.
Orient the power strip so:
The cable exit aligns with the enclosure notch.
The USB ports face upward.
## 3. Complete Wiring
Wire all components according to the electrical schematic.
Leave approximately 90 mm of wire for connections to:
Push buttons
TM1637 display

This provides sufficient length to reach the upper section of the enclosure.

## 4. Assemble Lower Section
Insert an M3 connector bolt into each corner recess of the bottom panel.
Secure each connector bolt using an M3 × 25 mm screw.
Attach the lower middle wall to the bottom panel using M3 × 20 mm screws.
Route all wires through the wire passage before tightening the fasteners.
## 5. Assemble Upper Section
Install the push buttons into the upper middle wall.
Install the light bulb socket into the top panel.
Mount the TM1637 display using M2.5 screws and M2.5 nuts.
Secure the light bulb socket using:
Light bulb bracket
M3 × 20 mm screws
M3 nuts
Attach the top panel to the upper middle wall using M3 × 20 mm screws and M3 nuts.
## 6. Final Wiring and Assembly
Connect the remaining wiring to all buttons and peripherals.
Verify all connections against the electrical schematic.
Perform a functional test before closing the enclosure.
Secure the lower and upper middle walls together using two zip ties.
## 7. Final Installation
Install the light bulb.
Connect power to the unit.
Verify proper operation of the display, buttons, and sunrise simulation.

Your Sunlight Simulating Alarm is now ready for use.

# Safety Notice
⚠️ Warning: This project contains mains-voltage AC wiring. Improper assembly can result in electric shock, fire, or equipment damage. Always disconnect power before servicing the device and verify all wiring connections before operation. If you are unfamiliar with AC electrical systems, seek assistance from a qualified individual.




