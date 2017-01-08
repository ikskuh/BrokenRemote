# BrokenRemote
BrokenRemote is a remote Lua code injector. This means, you can type some code in the tool, hit a
key and the code will be executed in the game **instantaneously**.

This allows a dev to prototype and create mods much faster than reloading a mod by using
the `luamod` command repeatedly. The tool also provides a vast set of predefined actions
that modify the current game state like spawning enemies, pedestal items, pickups, ...

It also provides 12 experimental items (4 active, 4 passive, 4 trinkets) that can be used
to prototype custom item ideas without the need of putting the items into the game first.

## Screenshots

### User Interface
![User Interface](https://puu.sh/tfig6/bb77592aae.png)

### Ingame Options
![Ingame Look](https://puu.sh/tfFRQ/77269d8c9d.png)

## Install instructions

1. Copy the folder "Mod" into your isaac mod folder
2. Rename the folder to "broken-remote"

## Usage instructions

1. Set the Steam console options to `--luadebug`
2. Start the tool
3. Start the game, enable the mod, start a run
4. Under isaacs feet, a "Connected" text should appear (see picture below)
5. In the mod tool, the console should show a connected message as well as the name of the character.

![Isaac is Connected](https://puu.sh/teMkw/2a0a0bd421.png)

## Getting started

Here are some examples what you can do with the tool... Just enter the provided script, then press **Ctrl+R**.

### Having some thumbs up

```lua
Game():GetPlayer(0):AnimateHappy()
```

### Spawning a pedestal item
```lua
local p = Game():GetPlayer(0)
local r = Game():GetRoom()

pos = r:FindFreePickupSpawnPosition(p.Position, 1.0, false)

Isaac.Spawn(
	EntityType.ENTITY_PICKUP,
	PickupVariant.PICKUP_COLLECTIBLE,
	CollectibleType.COLLECTIBLE_DEAD_EYE,
	pos,
	Vector(0,0),
	nil)
``` 

### Render text above isaacs head

```lua
function mod:MC_POST_RENDER()
	local p = Game():GetPlayer(0)
	local text = "Luck = " .. p.Luck
	local pos = Isaac.WorldToRenderPosition(p.Position, true) + Game():GetRoom():GetRenderScrollOffset()
	Isaac.RenderText(
		text,
		pos.X - 3 * #text, 
		pos.Y - 50,
		1,	-- R
		1,	-- G
		1,	-- B
		1.0)	-- A

end
```

![Text rendering](https://puu.sh/teN4c/1849c2765b.png)

## Experimental Items
The mod features some items that can be used to implement your dream mod:

### Active Item

- `Mod Active 1`
- `Mod Active 2`
- `Mod Active 3`
- `Mod Active 4`

### Passive Item

- `Mod Passive 1`
- `Mod Passive 2`
- `Mod Passive 3`
- `Mod Passive 4`

### Trinkets

- `Mod Trinket 1`
- `Mod Trinket 2`
- `Mod Trinket 3`
- `Mod Trinket 4`

# Troubleshooting

## The game is always paused, when I'm in the tool. Is there a way to prevent this?

The game is paused whenever it has lost its focus. You can prevent this by editing
the file named `options.ini` located in `%USERPROFILE%\Documents\My Games\Binding of Isaac Afterbirth+`. `%USERPROFILE%` is an environmental variable that should point to your users profile directory.

You have to change the line containing:

```
PauseOnFocusLost=1
``` 

to this one:

```
PauseOnFocusLost=0
``` 

## The "Connected" text is actually "Disconnected" and my game lags
This happens when **BrokenRemote** is not running. The game will then search for the
tool once every second and this is what causes the lagging.

Just start **BrokenRemote** or disable the mod if you don't want to develop this mod
right now.

## The "Connected" text does not appear
This can happen because of several reasons:

1. Make sure the game was started with the console option `--luadebug`
2. Make sure the mod has been loaded by the game in the "Mods" menu.
3. Make sure **BrokenRemote** is running

If none of the above helps, please file a GitHub issue and append your game log file.
This can be found here:

```
  %USERPROFILE%\Documents\My Games\Binding of Isaac Afterbirth+
```

## The "Open ***-Door" doesn't do anything
Yes, this is a known problem. I don't know what those functions do in particular, as the button
only calls `Game():GetRoom():TrySpawnDevilRoomDoor(...)` or similar.

If somebody knows more, please send me a message!

## Using *Mod Active 1* doesn't do anything
Those items don't do anything until you infuse them with some code.
Create a new script based on the *Active Item* template, hit **Ctrl+R**,
then use the item again. It should now spawn a *Breakfast* pedestal near
the player.

## The mod appears broken in the mod list
Yes, this happens from time to time. Open the mod folder, delete the `.it` files,
then restart the game.

It happens because the mod will executy any code that is send to it. This code may
be broken or use outdated APIs and if so, the game will flag the mod as broken or
outdated. Just ignore it and delete those files.