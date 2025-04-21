GamepadControl = {

	left_filled = false,
	right_filled = false,
	position = "center",

	OnStart = function(self)
		self.rb = self.actor:GetComponent("Rigidbody")
		Event.Subscribe("left_filled", self, self.OnLeftFilled)
		Event.Subscribe("left_emptied", self, self.OnLeftEmptied)
		Event.Subscribe("right_filled", self, self.OnRightFilled)
		Event.Subscribe("right_emptied", self, self.OnRightEmptied)
	end,

	OnUpdate = function(self)
		if Input.GetControllerButtonDown("DPadRight", self.cID) then
			if self.position == "center" and self.right_filled == false then
				Event.Publish("right_filled")
				self.position = "right"
			elseif self.position == "right" then
				self.position = "right"
			elseif self.position == "left" then
				Event.Publish("left_emptied")
				self.position = "center"
			end
		end

		if Input.GetControllerButtonDown("DPadLeft", self.cID) then
			if self.position == "center" and self.left_filled == false then
				Event.Publish("left_filled")
				self.position = "left"
			elseif self.position == "left" then
				self.position = "left"
			elseif self.position == "right" then
				Event.Publish("right_emptied")
				self.position = "center"
			end
		end

		if Input.GetControllerButtonDown("DPadDown", self.cID) then
			if self.position == "center" and self.right_filled == false then
				self.position = "center"
			elseif self.position == "right" then
				Event.Publish("right_emptied")
				self.position = "center"
			elseif self.position == "left" then
				Event.Publish("left_emptied")
				self.position = "center"
			end
		end

		local desired_position = Vector2(0,0)
		local current_position = self.rb:GetPosition()

		if self.position == "left" then
			desired_position = (Vector2(-3, 0))
		elseif self.position == "center" then
			desired_position = (Vector2(0, 1.3 * self.cID - 1.95))
		elseif self.position == "right" then
			desired_position = (Vector2(3, 0))
		end

		local new_position = current_position + (desired_position - current_position) * 0.2
		self.rb:SetPosition(new_position)

	end,

	OnLeftFilled = function(self)
		self.left_filled = true
	end,

	OnLeftEmptied = function(self)
		self.left_filled = false
	end,

	OnRightFilled = function(self)
		self.right_filled = true
	end,

	OnRightEmptied = function(self)
		self.right_filled = false
	end
}

