local r = Game():GetRoom()
local slot = DoorSlot.UP0
local pos = r:GetDoorSlotPosition(slot)

local idx = r:GetGridIndex(pos)
local ent = r:GetGridEntity(idx)
print("index", idx, ent)

r:RemoveGridEntity(
	idx,
	0,
	false)
local success = r:SpawnGridEntity(
	idx,
	GridEntityType.GRID_DOOR,
	DoorVariant.DOOR_UNSPECIFIED,
	0,
	0)
print("spawn", success)
if success then
	ent = r:GetGridEntity(idx)
	ent = ent:ToDoor()
	ent:SetRoomTypes(RoomType.ROOM_DEFAULT, RoomType.ROOM_DEFAULT)
	ent.TargetRoomIndex = 58
	ent.Slot = slot;
	print(ent.TargetRoomIndex, ent.Direction, Direction.Down)
	Game():GetPlayer(0):AnimateHappy()
else
	Game():GetPlayer(0):AnimateSad()
end