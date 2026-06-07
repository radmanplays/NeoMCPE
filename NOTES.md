# NeoMCPE 0.7.0-alpha-1.0.1b

## Changes

* Fully port mcpe 0.7.0
* Backported every item from mcpe 0.7.0
* Backported every GUI from mcpe 0.7.0
* Fixed bows not releasing while using touchscreen
* Fixed MCPE-131 – Dyes sometimes show up as the wrong color when holding in hand.
* Fixed MCPE-381 – Bows aren't destroyed after taking maximum damage.
* Fixed MCPE-1212 – Torch placement is incorrect.
* Lava now lights flammable blocks around it on fire.
* Bonemeal can now grow sugarcane(QoL feature)
* Empty buckets in creative mode can now pick up liquids(the player will not get a full bucket)
* Added an option while creating levels for the custom terrain generation from upstream repo

## Known bugs
* Something seems wrong with the chat packet as it dosnt recognise the one from real 0.7.0
* The scrolling in TouchSelectWorldMenu is not accurate to real 0.7.0 as i couldnt figure out how to port it(its really buggy rn)
* The TextBox(not component and Chat menus have not been ported yet
* The options category texts are slightly indented in real 0.7.0 while they're not in this port

## Roadmap

![Roadmap](.github/todo.png)
