--
--	Renders a text above Isaacs head that will show
--	the current luck our player has
--

-- This registers a BrokenRemote-specific callback
function mod:MC_POST_RENDER()
	local p = Game():GetPlayer(0)
	local text = "Luck = " .. p.Luck

	-- Determine the position of isaacs feet (p.Position),
	-- convert it to the render position (WorldToRenderPosition)
	-- and remove the rooms camera offset (GetRenderScrollOffset)
	local pos = Isaac.WorldToRenderPosition(p.Position, true) + Game():GetRoom():GetRenderScrollOffset()
	
	-- Finally, draw the text
	Isaac.RenderText(
		text,
		pos.X - 3 * #text, 
		pos.Y - 50,
		1,	-- R
		1,	-- G
		1,	-- B
		1.0)	-- A
end