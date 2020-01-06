#pragma once
#include "NativeHeader.h"
#include "Skeleton.h"
#include "Timestep.h"
#include "Modal.h"

nativenamespace{

	class AnimationPlayback
	{

	public:

		bool pause = false;
		bool reverse = false;
		bool loop = true;
		double time = 0.0;
		double animationSpeed = 10.0;
		double start = 0.0;
		double duration = 100.0;
		double totalDuration = 100.0;
		AnimationData* animation = NULL;

		AnimationPlayback() {}

		void SetData(AnimationData* data)
		{
			animation = data;
			start = data->trimStart;
			duration = data->trimEnd;
			totalDuration = data->duration;
		}

		bool Animate(Model& model)
		{
			if (animation == NULL)
				return false;
			UpdateAnimationTime();
			animation->UpdateKeys(time);
			for (Mesh& m : model.meshes)
				UpdateSkeleton(m.skeleton);
			return true;
		}

		void AnimateInitialFrame()
		{
			animation->UpdateKeys(0.0);
		}

		void UpdateSkeleton(Skeleton& skeleton)
		{
			skeleton.UpdateBones(*animation);
		}

		void GetFrame(double timeStamp, Skeleton& skeleton, std::vector<mat4>& boneList)
		{
			animation->UpdateKeys(timeStamp);
			Skeleton::AnimateBones(*animation, skeleton, boneList);
		}

		static void GetFrame(double timeStamp, AnimationData& dopeSheet, Skeleton& skeleton, std::vector<mat4>& boneList)
		{
			dopeSheet.UpdateKeys(timeStamp);
			Skeleton::AnimateBones(dopeSheet,skeleton, boneList);
		}

		double UpdateAnimationTime()
		{
			if (pause)
				return time;

			if (!reverse)
			{
				time += animationSpeed * Timestep::delta;
				if (time >= duration)
				{
					if (!loop)
						time = duration;
					else
						time = start;
				}
			}
			else {
				time -= animationSpeed*Timestep::delta;
				if (time < 0.0)
				{
					if (!loop)
						time = start;
					else
						time = duration;
				}
			}
			return time;
		}
	};
}