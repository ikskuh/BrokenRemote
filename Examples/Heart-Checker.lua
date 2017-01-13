local g = Game()
local l = g:GetLevel()
local r = g:GetRoom()
local p = g:GetPlayer(0)

-- TODO: Put your code here!

local function toBin(num)
	local r = ""
	for i=0,11 do
		if (num >> i) % 2 ~= 0 then
			r = "1"..r
		else
			r = "0"..r
		end
	end
	return r
end
local function cntBits(num)
	local r = 0
	for i=0,11 do
		if (num >> i) % 2 ~= 0 then
			r = r + 1
		end
	end
	return r
end

print(p:GetHearts(), p:GetSoulHearts(), p:GetBlackHearts(), p:GetMaxHearts(), toBin(p:GetBlackHearts()))

print(p:GetSoulHearts(), 2*cntBits(p:GetBlackHearts()))

p:AnimateHappy()