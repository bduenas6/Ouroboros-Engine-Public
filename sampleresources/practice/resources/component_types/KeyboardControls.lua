KeyboardControls = {
	speed = 5,
	jump_power = 350,
	coyote_time = 0,

	OnStart = function(self)
		self.rb = self.actor:GetComponent("Rigidbody")
	end,

	OnUpdate = function(self)
		-- Horizontal
		local horizontal_input = 0
		if Input.GetControllerButton("DPadRight", 1) then
			horizontal_input = self.speed
		end

		if Input.GetControllerButton("DPadLeft", 1) then
			horizontal_input = -self.speed
		end

		-- Vertical
		local vertical_input = 0

		-- Check on ground
		local on_ground = false
		ground_object = Physics.Raycast(self.rb:GetPosition(), Vector2(0, 1), 0.5)
		if ground_object == nil then
			self.coyote_time = self.coyote_time - 1
		elseif not ground_object.is_trigger then
			self.coyote_time = 15
		else
			self.coyote_time = self.coyote_time - 1
		end
		if Input.GetControllerButtonDown("DPadUp", 1) or Input.GetControllerButtonDown("A", 1) then
			if self.coyote_time > 0 then
				vertical_input = -self.jump_power
				self.coyote_time = 0
			end
		end

		self.rb:AddForce(Vector2(horizontal_input, vertical_input))
	end
}

