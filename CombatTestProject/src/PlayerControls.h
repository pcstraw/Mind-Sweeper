#pragma once
#include "NativeHeader.h"
#include "Input.h"
#include "Transform.h"
#include "Timestep.h"

nativenamespace{

	class PlayerControls
	{
	public:

		void KeyboardControls(Transform& transform,float forwardSpeed)
		{
			vec3 pos = transform.getPosition();

			if (Input::keyboard.IsDown(Key::W))
				pos += transform.getForward()*forwardSpeed*Timestep::delta;
			if (Input::keyboard.IsDown(Key::S))
				pos += transform.getForward()*-forwardSpeed*Timestep::delta;
			if (Input::keyboard.IsDown(Key::A))
				pos += transform.getRight()*forwardSpeed*Timestep::delta;
			if (Input::keyboard.IsDown(Key::D))
				pos += transform.getRight()*-forwardSpeed*Timestep::delta;
			transform.setPosition(pos);
		}
	};

}
