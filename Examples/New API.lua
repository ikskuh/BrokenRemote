local itemId = Isaac.GetItemIdByName("Mod Active 2")

mod:SetItemActivation(itemId, function(player)
	-- This is called when the item is activated (use item)
	print("activate")
end)

mod:SetItemUpdate(itemId, function()
	-- This would be called every frame when the item is held.
end)

mod:SetItemVisualization(itemId, function()
	-- This method can call custom rendering routines
	Isaac.RenderText("I has item!", 100, 100, 1,1,1,1)
end)

mod:SetItemStat(itemId, CacheFlag.CACHE_DAMAGE, function(p, dmg)
	-- Interesting use item: It increases damage while held ;)
	return dmg + 2
end)
mod:SetItemPickup(itemId, function(player)
	-- We have picked up the item
	print("pickup!")
end)
mod:SetItemPutdown(itemId, function(player)
	-- We have layed down the item
	print("putdown!")
end)

-- Just to make sure everything worked!
print("done")