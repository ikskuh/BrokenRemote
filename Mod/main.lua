local mod = RegisterMod("isaac-remote", 2);
local socket = require "socket"
local json = require "json"

local client

local onConnect

local initialInit = false

local function sendMessage(msg)
	if client then
		if type(msg) ~= "table" then
			msg = { type="msg" , msg=tostring(msg) }
		end
		client:send(json.encode(msg).."\n")
	end
end

local function tryConnect(initial)
  if client then
    return
  end
  
  client = socket.tcp()
  success = client:connect("127.0.0.1", 12000)
  if success then
    client:settimeout(0.01)
    Isaac.DebugString("Done: " .. tostring(client))
		
		-- Route all printing to the network connection
		_G["print"] = function(...)
			local t = table.pack(...)
			if #t > 0 then
				local msg = tostring(t[1])
				for i=2,#t do
					msg = msg .. "\t" .. tostring(t[i])
				end
				sendMessage(msg)
			end
		end

    print("I am " .. Game():GetPlayer(0):GetName())
		
		-- if onConnect then onConnect("init") end
		initialInit = true
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

local function sendHealthStatus()
	local function cntBits(num)
		local r = 0
		for i=0,11 do
			if (num >> i) % 2 ~= 0 then
				r = r + 1
			end
		end
		return r
	end

	local p = Game():GetPlayer(0)
	local msg = {
		type = "health-status",
		red = p:GetHearts(),
		max = p:GetMaxHearts(),
		soul = p:GetSoulHearts(),
		black = 2*cntBits(p:GetBlackHearts()),
		blackmask = p:GetBlackHearts(),
		gold = p:GetGoldenHearts(),
	}
	sendMessage(msg)
end

local function sendChargeStatus()
	local p = Game():GetPlayer(0)
	local msg = {
		type = "active-charge",
		hasActiveItem = (p:GetActiveItem() ~= -1),
		currentCharge = p:GetActiveCharge(),
		needsCharge = p:NeedsCharge()
	}
	sendMessage(msg)
end

local function sendRoomList()
  local l = Game():GetLevel()
  local function hasRoom(t)
	local rng = RNG()
	local idx1 = Game():GetLevel():QueryRoomTypeIndex(t, false, rng)
	rng:Next()
	local idx2 = Game():GetLevel():QueryRoomTypeIndex(t, false, rng)
	rng:Next()
	local idx3 = Game():GetLevel():QueryRoomTypeIndex(t, false, rng)
	return (idx1 ~= -1) and (idx1 == idx2) and (idx1 == idx3)
  end
  local data = { 
    type = "roomlist", 
    rooms = {
      -- ui->actionTeleportToStartRoom,
      true,
      -- ui->actionTeleportToShop,
      hasRoom(RoomType.ROOM_SHOP),
      -- ui->actionTeleportToBossRoom,
      hasRoom(RoomType.ROOM_BOSS),
      -- ui->actionTeleportToTreasureRoom,
      hasRoom(RoomType.ROOM_TREASURE),
      -- ui->actionTeleportToSecretRoom,
      hasRoom(RoomType.ROOM_SECRET),
      -- ui->actionAngel_Room,
      hasRoom(RoomType.ROOM_ANGEL),
      -- ui->actionDevil_Room,
      hasRoom(RoomType.ROOM_DEVIL),
      -- ui->actionSuper_Secret_Room,
      hasRoom(RoomType.ROOM_SUPERSECRET),
      -- ui->actionArcade,
      hasRoom(RoomType.ROOM_ARCADE),
      -- ui->actionSacrifice_Room,
      hasRoom(RoomType.ROOM_SACRIFICE),
      -- ui->actionCurse_Room,
      hasRoom(RoomType.ROOM_CURSE),
      -- ui->actionChallenge_Room,
      hasRoom(RoomType.ROOM_CHALLENGE),
      -- ui->actionLibrary,
      hasRoom(RoomType.ROOM_LIBRARY),
      -- ui->actionDungeon,
      hasRoom(RoomType.ROOM_DUNGEON),
      -- ui->actionIsaacs_Room,
      hasRoom(RoomType.ROOM_ISAACS),
      -- ui->actionBarren,
      hasRoom(RoomType.ROOM_BARREN),
      -- ui->actionDice_Room,
      hasRoom(RoomType.ROOM_DICE),
      -- ui->actionError,
      hasRoom(RoomType.ROOM_ERROR),
      -- ui->actionBlack_Market,
      hasRoom(RoomType.ROOM_BLACK_MARKET),
      -- ui->actionBoss_Rush_Room
      hasRoom(RoomType.ROOM_BOSSRUSH),
      -- ui->actionMini_Boss
      hasRoom(RoomType.ROOM_MINIBOSS),
    }
  }
  
  sendMessage(data)
end

local lastItemCharge, lastActiveItem
local lastHealth = { }

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
              sendMessage { type ="err", msg = ("Failed to compile code: " .. tostring(err)) }
            end
          elseif data.type == "roomlist" then
            sendRoomList()
          else
            sendMessage { type ="err", msg = ("Unkown data type: " .. tostring(data.type)) }
          end
        else
          sendMessage { type ="err", msg = ("Failed to unpack data: " .. tostring(err)) }
        end
      end)
      if not ok then
        sendMessage { type ="err", msg = err or "Unknown error!" }
      end
	_G["mod"] = nil
    end
  else 
    if Isaac.GetFrameCount() % 60 == 0 then
      tryConnect(false)
    end
  end

	if Isaac.GetFrameCount() < 10 then
		return
	end

	-- Watch the current item charge
	local currentCharge = Game():GetPlayer(0):GetActiveCharge()
	local currentActiveItem = Game():GetPlayer(0):GetActiveItem()
	if lastItemCharge ~= currentCharge or lastActiveItem ~= currentActiveItem then
		sendChargeStatus()
		lastItemCharge = currentCharge
		lastActiveItem = currentActiveItem
	end

	local p = Game():GetPlayer(0)
	if p:GetMaxHearts() ~= lastHealth.max or p:GetHearts() ~= lastHealth.hearts or
	   p:GetSoulHearts() ~= lastHealth.soul or p:GetBlackHearts() ~= lastHealth.black or
	   p:GetGoldenHearts() ~= lastHealth.gold then

		sendHealthStatus()

		lastHealth.max = p:GetMaxHearts()
		lastHealth.hearts = p:GetHearts()
		lastHealth.soul = p:GetSoulHearts()
		lastHealth.black = p:GetBlackHearts()
		lastHealth.gold = p:GetGoldenHearts()
	end
	
	if initialInit or Game():GetLevel():GetAbsoluteStage() ~= lastStage then
		if onConnect then onConnect("update") end
		lastStage = Game():GetLevel():GetAbsoluteStage()
		initialInit = false
	end
end

onConnect = function(...)
	sendHealthStatus()
	sendChargeStatus()
	sendRoomList()
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
				sendMessage { type="err", msg="Failed to execute " .. v .. ": " .. tostring(result) }
			end
		end
	end)
end