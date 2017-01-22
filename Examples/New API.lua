mod:SetItemActivation(511, function(player)
	print("activate")
end)

mod:SetItemUpdate(511, function()
	
end)

mod:SetItemVisualization(511, function()
	Isaac.RenderText("I has item!", 10, 10, 1,1,1,1)
end)

mod:SetItemStat(511, CacheFlag.CACHE_DAMAGE, function(p,prev)
	return prev + 2
end)

mod:SetItemPickup(511, function(player)
	player:AddCacheFlag(CacheFlag.CACHE_DAMAGE)
end)

print("done")