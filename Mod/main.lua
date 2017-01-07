local mod = RegisterMod("isaac-remote", 2);
local socket = require("socket")

local client

local function tryConnect(initial)
  if client then
    return
  end
  
  client = socket.tcp()
  success = client:connect("127.0.0.1", 12000)
  if success then
    client:settimeout(0.01)
    Isaac.DebugString("Done: " .. tostring(client))
    client:send("I am " .. Game():GetPlayer(0):GetName() .. "\n")
  else
    client = nil
  end
end

function mod:init(player)
  Isaac.DebugString("Initialize networking...")
  
  tryConnect(true)
end

function mod:render()
  local text = "Not connected"
  local cR = 1.0
  local cG = 0.0
  if client then
    text = "Connected"
    cR = 0.0
    cG = 1.0
  end
  local p = Game():GetPlayer(0)
	local pos = Isaac.WorldToRenderPosition(p.Position, true) + Game():GetRoom():GetRenderScrollOffset()
	Isaac.RenderText(
		text,
		pos.X - 3 * #text, 
		pos.Y + 3,
		cR,
		cG,
		0.0,
		1.0)

end

function mod:update()
  if client then
  
    local cmd, err = client:receive("*line")
    if err and err ~= "timeout" then
      Isaac.DebugString("CLI: "..err)
      -- force reconnect here
      client = nil
      return
    end
  
    if cmd then
    
      Isaac.DebugString("Received command: " .. tostring(cmd))
    
      local p = print
      print = function(x, ...)
        if x == nil then
          client:send("\n")
        else
          client:send(tostring(x))
          client:send("\t")
          print(...)
        end
      end
      _G["mod"] = mod
    
      local ok, err = pcall(function()
        local command, err = load(cmd)
        if command then
          command()
        else
          print("Failed to compile: " .. tostring(err))
        end
      end)
      if not ok then
        client:send(tostring(err or "Unknown error!"))
        client:send("\n")
      end
        print = p
        _G["mod"] = nil
    end
  else 
    if Isaac.GetFrameCount() % 60 == 0 then
      tryConnect(false)
    end
  end
end

mod:AddCallback(ModCallbacks.MC_POST_PLAYER_INIT, mod.init)
mod:AddCallback(ModCallbacks.MC_POST_UPDATE, mod.update)
mod:AddCallback(ModCallbacks.MC_POST_RENDER, mod.render)

-- Install mod hooks so mod has callback for everything :)

for i,v in pairs(ModCallbacks) do
	mod:AddCallback(v, function (...)
		if mod[i] then mod[i](...) end
	end)
end