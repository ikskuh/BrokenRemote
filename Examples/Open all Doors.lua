local g = Game()
local l = g:GetLevel()
local r = g:GetRoom()
local p = g:GetPlayer(0)

-- TODO: Put your code here!
local slots = { 
	DoorSlot.UP0,
	DoorSlot.DOWN0,
	DoorSlot.LEFT0,
	DoorSlot.RIGHT0,
	DoorSlot.UP1,
	DoorSlot.DOWN1,
	DoorSlot.LEFT1,
	DoorSlot.RIGHT1,
}

for i,slot in ipairs(slots) do
	local door = r:GetDoor(slot)
	if door then
		door:SetLocked(false)
		door:Open()
	end
end

print(door)

p:AnimateHappy()