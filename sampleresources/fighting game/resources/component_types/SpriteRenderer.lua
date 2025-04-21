SpriteRenderer = {
	sprite = "???",
	r = 255,
	g = 255,
	b = 255,
	a = 255,
	sorting_order = 0,
	color_change = false,

	OnStart = function(self)
		self.pos = Vector2(0, 0)
		self.rot_degrees = 0
		self.old_sprite = self.sprite
	end,

	OnUpdate = function(self)
		self.rb = self.actor:GetComponent("Rigidbody")

		if self.rb ~= nil then
			self.pos = self.rb:GetPosition()
			self.rot_degrees = self.rb:GetRotation()
		end

		if self.color_change and Input.GetControllerButton("A", self.cID) then
			self.r = 0
		else
			self.r = 255
		end

		if self.color_change and Input.GetControllerButton("B", self.cID) then
			self.g = 0
		else
			self.g = 255
		end

		if self.color_change and Input.GetControllerButton("X", self.cID) then
			self.sprite = "box1"
		else
			self.sprite = self.old_sprite
		end

		if self.color_change and Input.GetControllerButton("Y", self.cID) then
			self.a = 100
		else
			self.a = 255
		end

		local height = 1.0
		if self.actor:GetComponent("Rigidbody") ~= nil then
			height = self.actor:GetComponent("Rigidbody").height
		end
		Image.DrawEx(self.sprite, self.pos.x, self.pos.y, self.rot_degrees, 1.0, height, 0.5, 0.5, self.r, self.g, self.b, self.a, self.sorting_order)
	end
}

