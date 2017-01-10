-------------------------------------------------------------------------------
--
-- This script provides the basic implementation for a custom pill.
-- The pill will spawn a bunch of spiders.
--
-------------------------------------------------------------------------------

-- Replace the parameter with your own item name
local pillEffectId = Isaac.GetPillEffectByName("Mod Pill 1")
if pillEffectId == -1 then
	-- Don't do stuff when the pill wasn't found
	Game():GetPlayer(0):AnimateSad()
	return
end

-- Register our mod callback for the use with BrokenRemote
function mod:MC_USE_PILL(pillEffect)
	if pillEffect ~= pillEffectId then
		-- Make sure we have the correct pill effect
		return
	end
	local rng = RNG()	
	local p = Game():GetPlayer(0)

	-- Now let's spawn some spiders
	for i=1,rng:RandomInt(10) do
		p:AddBlueSpider(p.Position)
	end
end

-- Now spawn the pill to test it:
local pill = Isaac.AddPillEffectToPool(pillEffectId)
Game():GetPlayer(0):AddPill(pill)

Game():GetPlayer(0):AnimateHappy()