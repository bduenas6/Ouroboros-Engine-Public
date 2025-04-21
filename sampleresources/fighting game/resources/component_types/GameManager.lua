GameManager = {

	-- TILE CODES --
	-- 0 : nothing
	-- 1 : Static box
	-- 2 : player

	fighting_stage = {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, -- 10x10
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	},

	OnStart = function(self)
		-- Spawn stage
		for y=1,15 do 
			for x = 1,15 do
				local tile_code = self.fighting_stage[y][x]
				local tile_pos = Vector2(x, y)

				if tile_code == 2 then
					local new_player = Actor.Instantiate("Player")
					local new_player_rb = new_player:GetComponent("Rigidbody")
					new_player_rb.x = tile_pos.x
					new_player_rb.y = tile_pos.y
				
				elseif tile_code == 1 then
					local new_box = Actor.Instantiate("KinematicBox")
					local new_box_rb = new_box:GetComponent("Rigidbody")
					new_box_rb.x = tile_pos.x
					new_box_rb.y = tile_pos.y

				elseif tile_code == 3 then
					local new_box = Actor.Instantiate("BouncyBox")
					local new_box_rb = new_box:GetComponent("Rigidbody")
					new_box_rb.x = tile_pos.x
					new_box_rb.y = tile_pos.y
				
				elseif tile_code == 4 then
					local new_box = Actor.Instantiate("VictoryBox")
					local new_box_rb = new_box:GetComponent("Rigidbody")
					new_box_rb.x = tile_pos.x
					new_box_rb.y = tile_pos.y
				end
			end
		end

		--Input.PrintControllerMapping(0)
		--Input.PrintControllerMapping(1)
	end

--[[	OnUpdate = function(self)
		local seconds_elapsed = Actor.Find("HUD"):GetComponent("Hud").seconds_elapsed
		if seconds_elapsed == 15 then
			local player_actor = Actor.Find("player")
			if player_actor == nil then
				Debug.Log("nothing here")
			else
				player_actor:RemoveComponent(player_actor:GetComponent("KeyboardControls"))
			end
		end
		if seconds_elapsed >= 15 then
			Text.Draw("Game Over!", 300, 200, "NotoSans-Regular", 80, 0, 0, 0, 255)
		end
		if seconds_elapsed == 20 then
			Application.Quit()
		end
	end]]
}

