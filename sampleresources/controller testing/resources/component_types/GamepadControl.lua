GamepadControl = {
	speed = 10,

	OnStart = function(self)
		self.rb = self.actor:GetComponent("Rigidbody")
	end,

	OnUpdate = function(self)
		-- Horizontal
		local horizontal_input = 0
		if Input.GetControllerButton("DPadRight", self.cID) then
			horizontal_input = self.speed
		end

		if Input.GetControllerButton("DPadLeft", self.cID) then
			horizontal_input = -self.speed
		end

		-- Vertical
		local vertical_input = 0
		if Input.GetControllerButton("DPadUp", self.cID) then
			vertical_input = -self.speed
		end

		if Input.GetControllerButton("DPadDown", self.cID) then
			vertical_input = self.speed
		end

		self.rb:AddForce(Vector2(horizontal_input, vertical_input))

		-- Rotation
		if Input.GetControllerAxis("TriggerLeft", self.cID) > 20000 then
			self.rb:SetAngularVelocity(self.rb:GetAngularVelocity() - 50)
		end
		if Input.GetControllerAxis("TriggerRight", self.cID) > 20000 then
			self.rb:SetAngularVelocity(self.rb:GetAngularVelocity() + 50)
		end

		self.rb:SetAngularVelocity((3000 * Input.GetControllerAxis("TriggerRight", self.cID) / 32767))
	end
}

