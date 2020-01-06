#pragma once
#include "NativeHeader.h"
#include "Modal.h"

nativenamespace{

	class AnimationTrigger
	{
	public:

		vec3 oldpos;

		void WalkAnimation(Transform& transform,Model& model,float threshold = 0.1f)
		{
			vec3 currentpos = transform.getPosition();
			vec3 delta = currentpos - oldpos;
			
			if (glm::length(delta) > threshold)
			{
				//model.SelectAnimation(1);
			}else {
				//model.SelectAnimation(0);
			}
			//model.SelectAnimation(1);
			//model.Animate();
			oldpos = currentpos;
		}
	};
}
