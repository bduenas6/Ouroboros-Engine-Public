GameManager = {
	x_offset = -0.8,
	y_offset = -1,

	OnStart = function(self)
		Debug.PrintControllerMapping(0)
		Debug.PrintControllerMapping(1)
		Debug.PrintControllerMapping(2)
		Debug.PrintControllerMapping(3)
	end,

	OnUpdate = function(self)
		Text.Draw("Player 1:", (-3 + self.x_offset) * 100 + 960 / 2, (0 + self.y_offset) * 100 + 540 / 2, "NotoSans-Regular", 40, 0, 0, 0, 255)
		Text.Draw("Player 2:", (3 + self.x_offset) * 100 + 960 / 2, (0 + self.y_offset) * 100 + 540 / 2, "NotoSans-Regular", 40, 0, 0, 0, 255)
		Image.DrawEx("controller", -3, 0, 0, 1.0, 1.0, 0.5, 0.5, 0, 0, 0, 100, -1)
		Image.DrawEx("controller", 3, 0, 0, 1.0, 1.0, 0.5, 0.5, 0, 0, 0, 100, -1)
	end
}

