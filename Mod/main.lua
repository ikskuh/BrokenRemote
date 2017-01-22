local mod = RegisterMod("isaac-remote", 2);
local socket = require "socket"
local json = require "json"

local client
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
	  sendHealthStatus()
		sendChargeStatus()
		sendRoomList()
		lastStage = Game():GetLevel():GetAbsoluteStage()
		initialInit = false
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
			  sendMessage { type="err", msg="Failed to execute " .. v .. ": " .. tostring(result) }
			end
		end
	end)
end

local mod_data =
{
  activations = { },
	updates = { },
	visualizations = { },
	stats = { },
	pickups = { }
}

local function checkType(name, val, t)
  assert(type(val) == t, name .. " should be of type " .. t)
end

local function getPickupState(itemId)
  mod_data.pickups[itemId] = mod_data.pickups[itemId] or { has=false,stat=false }
  return mod_data.pickups[itemId]
end

function mod:SetItemActivation(itemId, callback) 
  checkType("itemId", itemId, "number")
  checkType("callback", callback, "function")
  mod_data.activations[itemId] = callback
end
function mod:SetItemUpdate(itemId, callback)
  checkType("itemId", itemId, "number")
  checkType("callback", callback, "function")
  mod_data.updates[itemId] = callback
end
function mod:SetItemVisualization(itemId, callback)
  checkType("itemId", itemId, "number")
  checkType("callback", callback, "function")
  mod_data.visualizations[itemId] = callback
end
function mod:SetItemStat(itemId, cache, callback)
  checkType("itemId", itemId, "number")
  checkType("cache", cache, "number")
  checkType("callback", callback, "function")
  mod_data.stats[itemId] = mod_data.stats[itemId] or { }
	mod_data.stats[itemId][cache] = callback
  getPickupState(itemId).stat = true
end
function mod:SetItemPickup(itemId, callback)
  checkType("itemId", itemId, "number")
  checkType("callback", callback, "function")
  getPickupState(itemId).pickup = callback
end
function mod:SetItemPutdown(itemId, callback)
  checkType("itemId", itemId, "number")
  checkType("callback", callback, "function")
  getPickupState(itemId).putdown = callback
end

local function mod_render(mod)
  local p = Game():GetPlayer(0)
	for item, vis in pairs(mod_data.visualizations) do
	  if p:HasCollectible(item) then
		  local ok, result = pcall(vis)
			if not ok and client then
			  sendMessage { type="err", msg="Failed to execute item visualization ("..tostring(item).."): " .. tostring(result) }
			end
		end
	end
end

local mod_activate = function(mod, item, player)
  if mod_data.activations[item] then
	  local ok, result = pcall(mod_data.activations[item], player)
		if not ok and client then
		  sendMessage { type="err", msg="Failed to execute item activation ("..tostring(item).."): " .. tostring(result) }
		end
	end
end

local function mod_cache(mod, player, cache)
  local propnames = {
	  [CacheFlag.CACHE_DAMAGE] = { "Damage" },
		[CacheFlag.CACHE_FIREDELAY] = { "MaxFireDelay" },
		[CacheFlag.CACHE_SHOTSPEED] = { "ShotSpeed" },
		[CacheFlag.CACHE_RANGE] = { "TearHeight", "TearFallingSpeed" },
		[CacheFlag.CACHE_SPEED] = { "MoveSpeed" } ,
		[CacheFlag.CACHE_TEARFLAG] = { "TearFlags" },
		[CacheFlag.CACHE_TEARCOLOR] = { "TearColor" },
		[CacheFlag.CACHE_FLYING] = { "CanFly" },
		[CacheFlag.CACHE_LUCK] = { "Luck" },
	}
	local statlist = propnames[cache]
	for item, stats in pairs(mod_data.stats) do
	  if player:HasCollectible(item) and stats[cache] then
      local input = { }
      for i,v in ipairs(statlist) do
        input[i] = player[v]
        print(i, v, input[i])
      end
    
		  local rdata = table.pack(pcall(stats[cache], player, table.unpack(input)))
      for i,v in pairs(rdata) do
        print("result", i, v)
      end
			if not rdata[1] and client then
			  sendMessage { type="err", msg="Failed to execute item stat ("..tostring(item)..","..tostring(cache).."): " .. tostring(rdata[2]) }
			end
      if rdata[1] then
        print("mod stat?")
        for i,v in ipairs(statlist) do
          print("i=",i, v)
          print(player[v])
          print(rdata[i + 1])
          player[v] = rdata[i + 1]
        end
        print("mod stat!")
			end
		end
	end
end

local function mod_update(mod, ...)
  local p = Game():GetPlayer(0)
  
  for item,status in pairs(mod_data.pickups) do
    local curr = p:HasCollectible(item)
    if curr ~= status.has then
      status.has = curr
      
      if status.stat then
        -- Enforce all item changes
        for i,v in pairs(mod_data.stats[item]) do
          p:AddCacheFlags(i)
        end
        p:EvaluateItems()
      end
      
      -- Call the pick-up callback
      if status.has and status.pickup then
        status.pickup(p)
      end
      
      -- Call the put-down callback
      if not status.has and status.putdown then
        status.putdown(p)
      end
      
    end
  end

  for item, vis in pairs(mod_data.updates) do
	  if p:HasCollectible(item) then
		  local ok, result = pcall(vis)
			if not ok and client then
			  sendMessage { type="err", msg="Failed to execute item update ("..tostring(item).."): " .. tostring(result) }
			end
		end
	end
end

mod:AddCallback(ModCallbacks.MC_POST_UPDATE, mod_update)
mod:AddCallback(ModCallbacks.MC_POST_RENDER, mod_render)
mod:AddCallback(ModCallbacks.MC_USE_ITEM, mod_activate)
mod:AddCallback(ModCallbacks.MC_EVALUATE_CACHE, mod_cache)

--[[
mod:SetItemStat(10, CacheFlags.CACHE_DAMAGE, function (player, prev)
  return prev + 10 -- 10 Damage Up!
end)
]]
