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
		
		-- Route all printing to the network connection
		_G["print"] = function(...)
			local t = table.pack(...)
			if #t > 0 then
				client:send(tostring(t[1]))
				for i=2,#t do
					client:send("\t")
					client:send(tostring(t[i]))
				end
				client:send("\n")
			end
		end
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
      _G["mod"] = mod
      local ok, err = pcall(function()
        local command, err = load("return " .. cmd)
        if command then
          local data = command()
          if data.type == "run" then
            local script, err = load(data.code)
            if script then
              script()
            else
              print("Failed to compile code: " .. tostring(err))
            end
          else
            print("Unkown data type: " .. tostring(data.type))
          end
        else
          print("Failed to unpack data: " .. tostring(err))
        end
      end)
      if not ok then
        client:send(tostring(err or "Unknown error!"))
        client:send("\n")
      end
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
		if mod[i] then
			local args = table.pack(...)
			local ok, result = pcall(function()
				mod[i](table.unpack(args)) 
			end)
			if not ok and client then
				client:send("Failed to execute " .. v .. ": " .. tostring(result) .. "\n")
			end
		end
	end)
end