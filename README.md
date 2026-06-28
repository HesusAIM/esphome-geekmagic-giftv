# esphome-geekmagic-giftv
This is a fully vibe coded project aiming to get ESPHome working on your GeekMagic GIFTV HelloCubic-Lite

Before you do anything keep in mind due to ram limitations the screen refreshes very slowly so it is not recommended for anything that updated constantly

To get this thing to work create a new folder named "components" in ESPHome home directory (where all your .yaml files are)
Then in newely creted components folder create new folder called "smalltv"
Open "smalltv" folder and upload those files: "__init.py", "display.py", "smalltv.h"
Then all you need to do is flash "hellocubiclite.yaml" to our device via ESPHome dashboard.
