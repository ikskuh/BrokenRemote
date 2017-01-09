local l = Game():GetLevel()
local idx = l:QueryRoomTypeIndex (RoomType.ROOM_TREASURE, true, RNG())
if idx ~= -1 then
	print("Start moving from "..tostring(l:GetCurrentRoomIndex()).." to "..tostring(idx))
	local round = 1
	while l:GetCurrentRoomIndex() ~= idx do
		print("Round="..tostring(round), "Room="..tostring(l:GetCurrentRoomIndex()))
		l:ChangeRoom(idx)
		round = round + 1
	end
	print("End moving. Arrived in "..tostring(l:GetCurrentRoomIndex()))
	Game():GetPlayer(0):AnimateHappy()
else
	Game():GetPlayer(0):AnimateSad()
end
