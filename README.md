# 3D-Graphics
Repository for my graphics works

find most recent video at
https://www.youtube.com/watch?v=3Fl5gqyP-tc

## Indoors
Over dim ambient light and also dim reddish directional light coming from the window, two lightbulbs provide point light. One of them sits in a lamp and gives bluish point light. The other; giving yellowish point light on the rotating pyramid, rotates with it and lights the marble statue at different angles as time passes. Both lights can be turned off and on at will and separately: Their bulbs look dark and they stop emitting light when they are turned off. Point light is set up on the fragment shader to provide more realistic lighting. Objects of different specularity and textures are scattered around the room in a logical order.

All in all, the scene resembles a kitchen at a Christmas morning; with a mystical present on the table waiting for whoever that might find it.

## Outdoors
The Scene is of a wooden cottage with bluish lights coming out of it near a street lamp with yellowish light in hilly terrain with a river flowing in a ravine nearby and a road near the cottage. Lights can be turned on and off and the weather can be changed between a dark and starry night and a foggy and cloudy day.

The height map of the scene is made in a way that the edges can enclose on each other seamlessly, so that when the same scene is concatenated from end to end on any of its four sides, the crossing between the scenes is not apparent. Then, the scenes are concatenated on a 7 by 7 grid centred on the camera. As the camera moves, scenes falling outside that grid de-spawn and new ones spawn to preserve the grid structure and thus the suspension of disbelief.

On that note, when below a Y value that’s lower than the highest hill, the terrain keeps a constant Y value between itself and the camera on the camera’s X and Z coordinates, effectively making it look like the camera/player is wading the terrain. However, when the Y distance between the camera and the terrain exceed that of the highest hill, the camera enters what one might call the flight mode: Here, the terrain doesn’t try to keep the distance between the camera and itself same, so it feels like the camera/player is free flying.

To create a more convincing look, there are 9 street lights, each once being inside the bulb of a street lamp, scattered around the scene cells that surround the cells where the camera is currently at.

This is how the scenes are connected around the camera. The camera is at cell dubbed “C” at any particular moment. The grids whose street lights are lit at any moment are painted yellow.

As the lighting inside the house is not so apparent, they aren’t repeated. But as with the whole grid structure, whenever the camera moves to a new cell, the point lights inside the house relocate to the house in the new cell.

Using a switch (key z), the user can turn off all the point lights in the scene and turn them back on again at will. To prevent total darkness, there is always some ambient light and dim directional light.

All point lights are attenuated, where the more powerful street lights provide a large bright spot around them, the lights coming from the house barely light up the ground around them.

To make the grid look more natural when flying high and to provide more realism while walking around, the scene is equipped with fog. In day mode, the fog is the colour of the clouds and makes the distant hills blend into the background. In night mode, the fog is pitch black and makes the dark of the night look more realistic on the terrain, especially in the distance.

To create the night and day modes, the scene is initialised with two separate skyboxes, and either one of them is rendered using a switch (key c). When the user uses the switch and changes between the modes, the fog changes alongside with the sky to too look coherent.

The river in the ravine is a separate height map that is mostly the same with the terrain, but with a higher section where the riverbed would be. This height map is positioned a few units below the terrain so that it doesn’t look like it is intersecting with the terrain at any point besides where the actual river itself is. Then, the map is assigned a water texture, which is animated in a direction and speed so that it looks like running water. 

The rocky outcropping overlooking the ravine, the cottage, the road and the lamp are all hand-placed and scaled appropriately to construct the scene.

All in all, considering how narrow the road is and how rustic the cottage looks, this scene seems to be that of an old riverside house by a scenic route that mysteriously seems to fold onto itself. Pity to all the travellers that might arrive at this house! Now they are trapped for an eternity to stay here!
