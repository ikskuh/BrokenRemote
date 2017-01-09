-- Query our item id as we can't hardcode it.
local itemId = Isaac.GetItemIdByName("Mod Passive 1")

function mod:MC_EVALUATE_CACHE(player, cache)
	if player:HasCollectible(itemId) == false then
		-- Do not apply any item effect when the player
		-- does not have the item
		return
	end
	
	-- This checks if we should modify damage
	if cache == CacheFlag.CACHE_DAMAGE then
		-- Oh yeah, massive damage up!
		player.Damage = player.Damage + 150
	end

	-- Add more if-blocks here to modify more stats
end

-- This enforces the reevaluation of our item,
-- Add all cache flags that you want to have modified 
Game():GetPlayer(0):AddCacheFlags(CacheFlag.CACHE_DAMAGE)
Game():GetPlayer(0):EvaluateItems()

-- Announce, that everything worked :)
Game():GetPlayer(0):AnimateHappy()
