-------------------------------------------------------------------------------
--
-- This script provides the basic implementation for a custom card.
-- The card will teleport the player to the black market.
--
-------------------------------------------------------------------------------

-- Replace the parameter with your own item name
local cardId = Isaac.GetCardIdByName("Mod Card 1")
if cardId == -1 then
	-- Don't do stuff when the item wasn't found
	Game():GetPlayer(0):AnimateSad()
	return;
end

-- Register our mod callback for the use with BrokenRemote
function mod:MC_USE_CARD(card)
	if card ~= cardId then
		return
	end
	
	local p = Game():GetPlayer(0)
	p:AnimateTeleport(true)
end