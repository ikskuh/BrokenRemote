-------------------------------------------------------------------------------
--
-- This script provides the basic implementation for a trinket.
-- The trinket will double our speed if collected.
--
-------------------------------------------------------------------------------

-- Small helper that returns only true when a or b is true, but not both
local function xor(a, b)
	return (a and not b) or (not a and b)
end

-- Query our item id as we can't hardcode it.
local trinketId = Isaac.GetTrinketIdByName("Mod Trinket 1")
if trinketId == -1 then
	-- We are sad, our trinket wasn't found
	Game():GetPlayer(0):AnimateSad()
	return;
end

function mod:MC_EVALUATE_CACHE(player, cache)
	if player:HasTrinket(trinketId) == false then
		-- Do not apply any trinket effect when the player
		-- does not have the trinket
		return
	end
	
	-- This checks if we should modify speed
	if cache == CacheFlag.CACHE_SPEED then
		-- Our trinket will double the speed
		-- Aware!
		player.MoveSpeed = player.MoveSpeed * 2
	end

	-- Add more if-blocks here to modify more stats
end

-- Sadly, we have to do some additional logic here
-- as the MC_EVALUATE_CACHE callback won't be called
-- when a trinket is collected

-- Store if we had the trinket in the last tick
local hadTrinket = false

function mod:MC_POST_UPDATE()
	
	local p = Game():GetPlayer(0)
	
	-- Check if we have the trinket now
	local hasTrinket = p:HasTrinket(trinketId)

	-- xor will now return only true if a 
	-- change of possesion happens
	local change = xor(hadTrinket, hasTrinket)
	if change then
		-- Either we lost our trinket 
		-- or we collected it, so:
		p:AddCacheFlags(CacheFlag.CACHE_SPEED)
		p:EvaluateItems()
	end
	
	-- Store the current status for the next tick
	hadTrinket = hasTrinket
end

-- Announce, that everything worked :)
Game():GetPlayer(0):AnimateHappy()
