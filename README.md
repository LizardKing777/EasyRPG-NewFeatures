Test project is in /EasyRPG3D

Changes:

1. Support for larger charsets.
2. Jumping now supported as movement - EXCEPT if you're turning left, for some reason.

Active bugs/issues to fix:

1. Still no rendering of upper tile layer.
2. Need to figure out how to render Charsets tallers than walls? Or make rendering higher so there's room to draw them?
3. Can't jump over impassable tiles - need some way of checking the passability and terrain type (walls are Terrain #2 in the database)
4. Events are drawn behind walls, so putting events (like a sign) on a wall is not visible in the game.
5. Need to figure out how to draw the Event Charsets as they move from point to point (I'm thinking this might be in the x_offset area of sprite_character.cpp)
6. Vehicles work but don't render in 3D. Also embarking/disembarking ships is kinda janky.
7. Looping maps - can't pass over the threshold to get to opposite side of map, nor does it render.


Features/things yet to be figured out:

1. Elevation - since this uses terrain as a flag to tell the plugin what to do with a specific tile, it should be possible to make "acending/descending" terrain that draw it at an angle so we could have hills and valleys.
2. High res support - Using FakeRes and 640x480 resolution, the entire thing gets extremely laggy. Not sure if this is fixable or not.
3. 
