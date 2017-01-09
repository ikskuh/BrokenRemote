local g = Game()
local l = g:GetLevel()
local r = g:GetRoom()
local p = g:GetPlayer(0)

local json = require "json"
for i,v in pairs(json) do print(i,v) end
t = {
	type = "msg",
	msg = "Hello\nWorld!"
}

print(json.encode(t))

p:AnimateHappy()