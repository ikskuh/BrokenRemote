--
-- This code spawns an item near the player.
-- The item can be spawned by name or by id
--

-- Set this to false to get the item by
-- CollectibleType instead of the item name
local getItemIdByName = true

-- Get the current room and the main player
local p = Game():GetPlayer(0)
local r = Game():GetRoom()

-- Find a spot where we can spawn the item
-- the last parameter determines if we want the
-- spawn position to avoid entities
local pos = r:FindFreePickupSpawnPosition(p.Position, 1.0, true)

local itemId
if getItemIdByName then
	-- Use this function to find an item by name
	itemId = Isaac.GetItemIdByName("Dead Eye")
else
	-- Use this method to assign a direct item id
	itemId = CollectibleType.COLLECTIBLE_DEAD_EYE
end

Isaac.Spawn(
	-- This defines the entity type is a pickup
	EntityType.ENTITY_PICKUP,

	-- This defines, that the pickup is a collectible (podestal item)
	PickupVariant.PICKUP_COLLECTIBLE,

	-- This is our item id
	itemId,

	-- Position of the item
	pos,

	-- Set this to a direction to give the spawn a "kick"
	Vector(0,0),

	-- We don't need an owner
	nil)