CameraManager = {
	ease_factor = 0.1,
	tracking_player = false,

	OnUpdate = function(self)
		Camera.SetPosition(0, -0.5)
		Camera.SetZoom(0.5)
	end
}

