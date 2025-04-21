ParticleController = {
	ps = nil,
	t = 0.0,

	OnUpdate = function(self)
		if Application.GetFrame() == 10 then
			local particle_actor = Actor.Instantiate("particle_actor")
			self.ps = particle_actor:GetComponent("ParticleSystem")
		end

		if Application.GetFrame() >= 10 then
			self.t = self.t + 0.05

			self.ps.x = math.cos(self.t)
			self.ps.y = math.sin(self.t)
		end
	end
}

