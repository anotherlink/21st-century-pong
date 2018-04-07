pong game clone for linux / nailara agent child
===============================================

Pong game clone for Linux written in C++ with SDL 2.0
    by [Chafic Najjar](https://github.com/chaficnajjar).

[ cloned and modified to work as nailara layout tile agent child
  by [photon@nailara.net](https://github.com/anotherlink) (2018-03-07) ]

![Screenshot](http://mirror.nailara.net/screenshots/pong-game.png)

## Dependencies

You will need:

+ [SDL 2.0](https://www.libsdl.org/)
+ [SDL Mixer 2.0](http://www.libsdl.org/projects/SDL_mixer/)
+ [SDL TTF 2.0](https://www.libsdl.org/projects/SDL_ttf/)

### Ubuntu / Debian

Installing dependencies on Ubuntu or Debian-style distributions:

`sudo apt-get install libsdl2-dev libsdl2-mixer-dev libsdl2-ttf-dev`

## Getting the Source and Building the Game Binary

```
git clone https://github.com/anotherlink/pong-game.git
cd pong-game
make
```

## Running the Game

To run the game you need to supply its window dimensions as follows:

`./pong <X> <Y> <W> <H>`

To run the game with joystick support:

`./pong <X> <Y> <W> <H> joystick`

to start playing press the left mouse or joystick button (labeled 'OK' in game).

It will go into fullscreen mode as soon as you start the game and print game
events to STDOUT. To exit the game press ESC. to return from fullscreen back to
'tile' (window) mode press the button again while the game is running.
It will then exit fullscreen and reset the game score once the round is over.


## License and Credits

Pong code is MIT licensed

```
The MIT License (MIT)

Copyright (c) 2016 Chafic

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

Created by [Chafic Najjar](https://github.com/chaficnajjar).
Modified for the [nailara project](https://github.com/anotherlink/nailara)
                    by [photon](https://github.com/anotherlink).

NES Chimera Font by [Brian Kramer](https://www.pkeod.com/).
