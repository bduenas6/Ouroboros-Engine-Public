SpriteRenderer = {
	sprite = "???",
	r = 255,
	g = 255,
	b = 255,
	a = 255,
	sorting_order = 0,

	OnStart = function(self)
		self.pos = Vector2(0, 0)
		self.rot_degrees = 0
	end,

	OnUpdate = function(self)
		self.rb = self.actor:GetComponent("Rigidbody")

		if self.rb ~= nil then
			self.pos = self.rb:GetPosition()
			self.rot_degrees = self.rb:GetRotation()
		end

		if not Input.ControllerConnected(self.cID) then
			self.a = 100
		else
			self.a = 255
			Text.Draw((self.cID + 1), (self.pos.x - 0.125) * 100 + 960 / 2, (self.pos.y + 0.125) * 100 + 540 / 2, "NotoSans-Regular", 40, 0, 0, 0, self.a)
		end

		Image.DrawEx(self.sprite, self.pos.x, self.pos.y, self.rot_degrees, 1.0, 1.0, 0.5, 0.5, self.r, self.g, self.b, self.a, self.sorting_order)
	end
}

