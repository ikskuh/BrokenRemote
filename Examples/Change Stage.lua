local g = Game()
local l = g:GetLevel()
local r = g:GetRoom()
local p = g:GetPlayer(0)

-- l:InitializeDevilAngelRoom(true, false)
-- l:ShowMap()
-- l:SetStage(LevelStage.STAGE1_2, StageType.STAGETYPE_ORIGINAL)
-- g:StartStageTransition(true, 1) 

l:InitializeDevilAngelRoom(true, false)
l:AddAngelRoomChance(1.0)

print("devil deal", l:CanSpawnDevilRoom(), r:TrySpawnDevilRoomDoor(true))

p:AnimateHappy()

-- This is fucked up, but awesome!
-- l:SetStage(LevelStage.STAGE6, StageType.STAGETYPE_AFTERBIRTH)
