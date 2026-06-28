# esphome-geekmagic-giftv
This is a fully vibe coded project aiming to get ESPHome working on your GeekMagic GIFTV HelloCubic-Lite

Before you do anything keep in mind due to ram limitations the screen refreshes very slowly so it is not recommended for anything that updates constantly.

Using File editor add-on create a new folder named "components" in ESPHome home directory (where all your .yaml files are)
Then in newely creted components folder create a new folder called "smalltv"
Open "smalltv" folder and upload those files: "__init__.py", "display.py", "smalltv.h"
Then all you need to do is flash "hellocubiclite.yaml" to your device via ESPHome dashboard.

To flash you'll need a UART to USB adapter

Wiring:
From top to bottom. On the left is HelloCubic, on the right UART adapter
Square Pad   GND    -   GND

Second Pad   TXD0   -   RXD

Third Pad    RXD0   -   TXD

Fourth Pad   3V3    -   3V3

Fifth Pad    GPIO0  -   Short to ground while plugging UART adapter to PC

Sixth Pad    RST    -   NC

Next open ESPHome dashboard, click "+ Create device" then "Advanced set up options" and choose "Import from file".
Select hellocubiclite.yaml. Click "Install", then "Plug into this computer" and finally choose your device from the pop-up window
It should flash it and you should be able to see something on the screen.

Of course it's up to you what you want to display. Enjoy.
