# BrokenRemote
A remote debugging tool for The Binding of Isaac: Afterbirth+

![Early screenshot](http://i.imgur.com/7RFeRom.png)

![User Interface](https://puu.sh/tfig6/bb77592aae.png)

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
