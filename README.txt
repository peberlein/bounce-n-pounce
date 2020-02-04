Bounce'n'Pounce (game in development)

Here is a TI99/4A port of the arcade game "Bump 'n' Jump" from Data East in 1982.
It is written in assembly language, developed using Ralph's XDT99 tools.
It features full-screen vertical scrolling at pixel resolution in bitmap graphics mode.
Use joystick 1 or the E S D X keys to accelerate/steer/decelerate, and Fire, Space bar
or Enter to jump.  Press the P key to pause and unpause the game.

Colliding cars will temporarily bounce in opposite directions, and bounced 
opponent cars may smash against the barriers.  Various cars have different point score values.
Your car speed must be at least 100 to be able to jump.
Jump over gaps in the road, or to avoid being bounced into a hazard.  
Upon landing, cars underneath will be instantly smashed.
Completing a road will score bonus points for the number of cars smashed,
before moving on to the next season.  An extra life is awarded every 30K points.
Landing on an island will award 1000 points.


This game was started when I had an idea for a method of scrolling the whole
screen, and I made a prototype for the 4K contest in early 2018.  The prototype
proved the scrolling effect worked, and then I spent a lot of time compressing
it down to fit in 4K.  The prototype used only 13 out of the 16 track segments, 
used only one season's color table, and had track data only for the first two levels, 
(before going off into the weeds and filling video memory with garbage.)
The full game has 8 levels of 64-segment road, with more complicated segment 
transitions that didn't work within the prototype transition scheme.
Instead of loading a new transition set every 32 rows (1 segment), I needed to
allow reloading every 8 rows (1/4 segment).  This is fine because the bitmap 
graphics mode is 3 chunks of 8 rows, so each chunk can be updated individually.
Some of the original segment elements had to be adjusted to made the seams easier
to fit into the transition table.  The transition tables are limited to 32 entries,
since each transition consumes 8 of the 256 characters.
I spent some of my last winter holiday vacation working on the transition table 
generation, and finally got it working.

I grew up playing the Intellivision port as a kid, and have incorporated some
of the same design elements into this version.  The "Get Ready Player" and 
"Congratulations" screens have white text over a black background.  
Another similarity is that the opponent vehicle sprites are limited to a single color.


