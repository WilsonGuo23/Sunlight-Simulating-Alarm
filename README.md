# Sunlight-Simulating-Alarm
Alarm that simulates sunlight by starting at a lower light level and increasing output in relation to proximity to target waking time

# Schematic
<img width="2075" height="1318" alt="Sunlight Alarm Schematic" src="https://github.com/user-attachments/assets/a134af7a-e7d0-4519-a21f-105d2a449b64" />

# Bill of Materials (subject to change)
- 4 × M3 × 20 mm screws
- 4 × M2 × 20 mm screws
- 4 × M3 bolts
- 4 × M2 bolts
- 4 × M3 connector bolts
- 8 × M3 × 50 mm screws
- 5 × 24 mm pushbuttons
- 5 × 110 mm quick-disconnect crimps
- 2 ft × 20 gauge copper wire
- 1 × Raspberry Pi Pico
- 1 × Sunlight-simulating lightbulb

## STL Files
- 4 × Pico standoffs
- 4 × Triac standoffs
- 1 × Bottom panel
- 1 x Lightbulb Bracket
- 1 × Mounting bracket
- 1 × Lower middle connecting wall
- 1 × Upper middle connecting wall
- 1 × Top panel

# Assembly
Engineering Drawings can be found in the "Mechanical Schematics" folder, you can also use the assembly in the SLDPRT Files as a reference intended assembly
1. Attach the Raspberry Pi Pico and TRIAC dimmer to the mounting bracket using the M2 screws, M2 nuts, M3 screws, M3 nuts, and standoffs. Ensure the AC-voltage terminals on the TRIAC dimmer face outward from the bracket.
2. Secure the mounting bracket to the bottom panel using M3 screws and M3 nuts.
3. Insert the power strip into the rectangular opening in the bottom panel. Position it so the notch is on the same side as the power cord and the USB ports face upward.
4. Wire the components according to the electrical schematic. Any wires that do not connect directly between the Pico and TRIAC dimmer, or to the power strip, should be approximately 90 mm long to reach the top section of the enclosure.
5. Insert an M3 connector bolt into each corner hexagonal recess and secure each one with an M3 screw.
6. Attach the lower middle wall to the bottom panel using M3 screws. Make sure all wires are routed through the wire passage before tightening.
7. Install the push buttons and light bulb socket into their respective mounting holes in the upper middle wall and top panel.
8. Attach the TM1637 display module to the upper middle wall using M2.5 screws and M2.5 nuts.
9. Secure the light bulb socket using the light bulb bracket, two M3 screws, and two M3 nuts.
10. Attach the top panel to the upper middle wall using M3 screws and M3 nuts.
11. Connect the remaining wiring to the appropriate push buttons and components. Verify all connections against the electrical schematic and test the assembly.
12. Secure the lower and upper middle walls with 2 zipties or ropes
13. Install the light bulb, power on the unit, and enjoy your new alarm clock!







