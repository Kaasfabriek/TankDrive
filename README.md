# Kaasfabriek pro mini TANK DRIVE

##Libraries required
- Servo.h

##Tank drive to controll left & right power drive
```
  implementation for fablab Kaasfabriek Junior power drive projects in 2017
       initially to power Marco van Schagen's dual or quad Minn Kota boat drive
               also for a quick manual controll of gripper on the Klokhuis robot
```

- Software plakker: Maarten van Schagen
- Algemene verhalen kletser: Marco --> marco@kaasfabriek.nl
- Regie en inspiratie: Kaasfabriek --> info at kaasfabriek punt nl

##Important
the pin layout below does not include the wiring tips...
1. bring all connectors to the back of the arduino pro mini board
2. use 4-pin angular male header for programmer connector
3. use female 5 pin header to connect joystick without additional wiring
4. use two 1-pin angular header to help solder 3 wires on each of RAW and GND
5. use two sets of orange/red/brown flat cable to connect directly to servo connector
6. use male JST connector to connect directly to 7.2 volt LIPO
8. when done soldering and testing, tape all wires togather and tie-wrap onto board to prevent breaking the wires


##Pin mapping
    Suggested pin mapping:
    -----------------------------------------------------------------------------
                            ║                 ║ USB to serial programmer
                            ║3.3 TX RX GND 5V ║ while programming, power comes
                            ╚══╬══╬══╬══╬══╬══╝ directly from ESC or from 
                               x  │  │  │  x    external supply for Servo
                               ┌──│──┘  │
                               │  │  x  │      ARDUINO pro mini
                          ╔═╬══╬══╬══╬══╬══╬═╗ 8Mhz 16kb
                          ║ G TX RX VCC GND B║ 5 volt
                          ║                  ║     
                          ╬ TXD          RAW ╬─┬─ Power +
    brown/servo conn 1 ─┐ ╬ RXD          GND ╬ ├─ red/servo conn 1
    brown/servo conn 2 ─┤ ╬ RST          RST ╬ └─ red/servo conn 2
              Power - ──┴─╬ GND          VCC ╬    ╔═══════╗
                          ╬ 2             A3 ╬────╬ GND   ║
                          ╬ 3             A2 ╬────╬ 5V    ║
    orange/servo conn 1 ──╬ 4             A1 ╬────╬ RX    ║
                          ╬ 5             A0 ╬────╬ RY    ║
    orange/servo conn 2 ──╬ 6       (led) 13*╬────╬ BTN   ║
                          ╬ 7        MISO 12*╬    ╚═══════╝
                          ╬ 8        MOSI 11*╬    joystick
                          ╬ 9             10 ╬
                          ║                  ║
                          ╚═══╬══╬══╬══╬══╬══╝
                                                 
                                              

    arduino serco1 servo2
    | |     | |    | |                                     ╔═════════╗
    └─|─────┴─|────┴─|───red────            ────red─lipo───╬+  1 CELL║
      └───────┴──────┴───black──            ───black─lipo──╬-  LIPO  ║
                              JST male   JST female        ╚═════════╝
                              connector  connector
         
