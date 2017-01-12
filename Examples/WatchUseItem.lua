local lastItemCharge

function mod:MC_POST_UPDATE()
	local g = Game()
	local l = g:GetLevel()
	local r = g:GetRoom()
	local p = g:GetPlayer(0)	

	local currentCharge = p:GetActiveCharge()
	if lastItemCharge ~= currentCharge then
		print("Charge changed from", lastItemCharge, "to", currentCharge)
		lastItemCharge = currentCharge
	end

end

p:AnimateHappy()