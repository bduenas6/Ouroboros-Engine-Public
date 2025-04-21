GamepadControl = {
	speed = 3,
	local_frame = 0,
	state = "neutral",
	direction = "right",
	blocking = false,
	attack = "???",
	health = 1000,

	OnStart = function(self)
		self.rb = self.actor:GetComponent("Rigidbody")
		self.rend = self.actor:GetComponent("SpriteRenderer")
	end,

	Attack = function(self)
		if self.attack == "Aattack" then
			--body
		elseif self.attack == "Battack" then
			--body
		elseif self.attack == "Xattack" then
			--body
		elseif self.attack == "Yattack" then
			--body
		elseif self.attack == "AattackC" then
			--body
		elseif self.attack == "BattackC" then
			--body
		elseif self.attack == "XattackC" then
			--body
		elseif self.attack == "YattackC" then
			--body
		elseif self.attack == "AattackA" then
			--body
		elseif self.attack == "BattackA" then
			--body
		elseif self.attack == "XattackA" then
			--body
		elseif self.attack == "YattackA" then
			--body
		else
			Debug.Log("wrong attack???")
		end
	end,

	OnUpdate = function(self)
		self.rb:SetRotation(0)
		self.blocking = false

		if self.state == "attack" then
			self.Attack(self)
			return
		elseif self.state == "hit" then
			if self.local_frame > 0 then
				self.local_frame = self.local_frame - 1
			else
				self.state = neutral
			end
		end

		if self.actor:GetName() == "p1" then
			local xpos = self.rb:GetPosition().x
			local xposOther = Actor.Find("p2"):GetComponent("Rigidbody"):GetPosition().x
			if(xpos > xposOther) then
				self.direction = "left"
			else
				self.direction = "right"
			end
		else
			local xpos = self.rb:GetPosition().x
			local xposOther = Actor.Find("p1"):GetComponent("Rigidbody"):GetPosition().x
			if(xpos > xposOther) then
				self.direction = "left"
			else
				self.direction = "right"
			end
		end

		if self.state == "neutral" then
			self.rb:SetVelocity(Vector2(0, 0))
			if Input.GetControllerButton("DPadRight", self.cID) then
				self.rb:SetVelocity(Vector2(self.speed, 0))
				if self.direction == "left" then
					self.blocking = true
				end
			elseif Input.GetControllerButton("DPadLeft", self.cID) then
				self.rb:SetVelocity(Vector2(-self.speed, 0))
				if self.direction == "right" then
					self.blocking = true
				end
			end

			if Input.GetControllerButton("DPadUp", self.cID) then
				self.state = "air"
				local jump = Vector2(0, -self.speed) * 150
				self.rb:AddForce(jump)
			elseif Input.GetControllerButton("DPadDown", self.cID) then
				self.state = "crouch"
			end

			if Input.GetControllerButton("A", self.cID) then
				-- Y attack
				self.state = "attack"
				self.attack = "Aattack"
			elseif Input.GetControllerButton("B", self.cID) then
				-- Y attack
				self.state = "attack"
				self.attack = "Battack"
			elseif Input.GetControllerButton("X", self.cID) then
				-- Y attack
				self.state = "attack"
				self.attack = "Xattack"
			elseif Input.GetControllerButton("Y", self.cID) then
				-- Y attack
				self.state = "attack"
				self.attack = "Yattack"
			end

		elseif self.state == "crouch" then
			self.rb:SetVelocity(Vector2(0, 0))

			if Input.GetControllerButton("DPadRight", self.cID) then
				if self.direction == "left" then
					self.blocking = true
				end
			elseif Input.GetControllerButton("DPadLeft", self.cID) then
				if self.direction == "right" then
					self.blocking = true
				end
			end

			if Input.GetControllerButton("DPadUp", self.cID) then
				self.state = "air"
				local jump = Vector2(0, -self.speed * 1.3)  * 150
				self.rb:AddForce(jump)
			elseif Input.GetControllerButton("DPadDown", self.cID) then
				self.state = "crouch"
			else
				self.state = "neutral"
			end

			if Input.GetControllerButton("A", self.cID) then
				-- Y attack
				self.state = "attack"
				self.attack = "AattackC"
			elseif Input.GetControllerButton("B", self.cID) then
				-- Y attack
				self.state = "attack"
				self.attack = "BattackC"
			elseif Input.GetControllerButton("X", self.cID) then
				-- Y attack
				self.state = "attack"
				self.attack = "XattackC"
			elseif Input.GetControllerButton("Y", self.cID) then
				-- Y attack
				self.state = "attack"
				self.attack = "YattackC"
			end	

		elseif self.state == "air" then
			local below = Physics.Raycast(self.rb:GetPosition(), Vector2(0, 1), 1.1)
			if below == nil then
				self.state = "air"
				if Input.GetControllerButton("DPadRight", self.cID) then
					if self.direction == "left" then
						self.blocking = true
					end
				elseif Input.GetControllerButton("DPadLeft", self.cID) then
					if self.direction == "right" then
						self.blocking = true
					end
				end

				if Input.GetControllerButton("A", self.cID) then
					-- Y attack
					self.state = "attack"
					self.attack = "AattackA"
				elseif Input.GetControllerButton("B", self.cID) then
					-- Y attack
					self.state = "attack"
					self.attack = "BattackA"
				elseif Input.GetControllerButton("X", self.cID) then
					-- Y attack
					self.state = "attack"
					self.attack = "XattackA"
				elseif Input.GetControllerButton("Y", self.cID) then
					-- Y attack
					self.state = "attack"
					self.attack = "YattackA"
				end
			else
				self.state = "neutral"
			end

		end

		if self.blocking == true then
			Debug.Log("block")
		end
	end,

	OnTriggerEnter = function(self, collision)
		if self.blocking == true then
			if collision.other:GetComponent("GamepadControl").attack == "AattackA" then
				self.state = "hit"
				self.local_frame = 5
				self.health = self.health - 60
				self.rb:AddForce(collision.normal)
			elseif collision.other:GetComponent("GamepadControl").attack == "BattackA" then
				self.state = "hit"
				self.local_frame = 7
				self.health = self.health - 90
				self.rb:AddForce(collision.normal)
			elseif collision.other:GetComponent("GamepadControl").attack == "XattackA" then
				self.state = "hit"
				self.local_frame = 9
				self.health = self.health - 120
				self.rb:AddForce(collision.normal * 2)
			elseif collision.other:GetComponent("GamepadControl").attack == "YattackA" then
				self.state = "hit"
				self.local_frame = 11
				self.health = self.health - 80
				self.rb:AddForce(collision.normal + Vector2(0, -1))
			else
				self.rb:AddForce(collision.normal)
				return
			end
		end
		self.state = "hit"
		if collision.other:GetComponent("GamepadControl").attack == "Aattack" then
			self.local_frame = 5
			self.health = self.health - 60
			self.rb:AddForce(collision.normal)
		elseif collision.other:GetComponent("GamepadControl").attack == "Battack" then
			self.local_frame = 7
			self.health = self.health - 90
			self.rb:AddForce(collision.normal)
		elseif collision.other:GetComponent("GamepadControl").attack == "Xattack" then
			self.local_frame = 9
			self.health = self.health - 120
			self.rb:AddForce(collision.normal * 2)
		elseif collision.other:GetComponent("GamepadControl").attack == "Yattack" then
			self.local_frame = 11
			self.health = self.health - 80
			self.rb:AddForce(collision.normal + Vector2(0, -1))
		elseif collision.other:GetComponent("GamepadControl").attack == "AattackC" then
			self.local_frame = 5
			self.health = self.health - 40
			self.rb:AddForce(collision.normal)
		elseif collision.other:GetComponent("GamepadControl").attack == "BattackC" then
			self.local_frame = 7
			self.health = self.health - 70
			self.rb:AddForce(collision.normal)
		elseif collision.other:GetComponent("GamepadControl").attack == "XattackC" then
			self.local_frame = 9
			self.health = self.health - 100
			self.rb:AddForce(collision.normal * 2)
		elseif collision.other:GetComponent("GamepadControl").attack == "YattackC" then
			self.local_frame = 11
			self.health = self.health - 60
			self.rb:AddForce(collision.normal + Vector2(0, -1))
		elseif collision.other:GetComponent("GamepadControl").attack == "AattackA" then
			self.local_frame = 5
			self.health = self.health - 60
			self.rb:AddForce(collision.normal)
		elseif collision.other:GetComponent("GamepadControl").attack == "BattackA" then
			self.local_frame = 7
			self.health = self.health - 90
			self.rb:AddForce(collision.normal)
		elseif collision.other:GetComponent("GamepadControl").attack == "XattackA" then
			self.local_frame = 9
			self.health = self.health - 120
			self.rb:AddForce(collision.normal * 2)
		elseif collision.other:GetComponent("GamepadControl").attack == "YattackA" then
			self.local_frame = 11
			self.health = self.health - 80
			self.rb:AddForce(collision.normal + Vector2(0, -1))
		end
	end
}

