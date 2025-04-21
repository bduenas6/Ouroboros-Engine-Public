CameraManager = {
	ease_factor = 0.1,
	tracking_player = false,

	OnUpdate = function(self)
		local player_actor = Actor.Find("p1")
		local player_actor2 = Actor.Find("p2")
		if player_actor == nil and player_actor2 == nil then
			self.tracking_player = false
			return
		
		elseif self.tracking_player == false then
			self.tracking_player = true
			local rb = player_actor:GetComponent("Rigidbody")
			local rb2 = player_actor2:GetComponent("Rigidbody")
			local player_pos = rb:GetPosition()
			local player2_pos = rb2:GetPosition()
			Camera.SetPosition((player_pos.x + player2_pos.x) / 2, (player_pos.y + player2_pos.y) / 2)
			return
		end

		local player_rb = player_actor:GetComponent("Rigidbody")
		local player_rb2 = player_actor2:GetComponent("Rigidbody")
		local desired_position = (player_rb:GetPosition() + player_rb2:GetPosition()) * 0.5 + Vector2(0,  - 1.0)
		local current_position = Vector2(Camera.GetPositionX(), Camera.GetPositionY())

		local new_position = current_position + (desired_position - current_position) * self.ease_factor
		Camera.SetPosition(new_position.x, new_position.y)

		local dist = Vector2.Distance(player_rb:GetPosition(), player_rb2:GetPosition())
		if (7.0 / dist) > 1.0 then
			Camera.SetZoom(1.0)
		else
			Camera.SetZoom(7.0 / dist)
		end
	end
}

