// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Constants.h"

/**
 * 
 */
class LE_GAME_API DirectionHelper
{
public:
	DirectionHelper();
	~DirectionHelper();

	static constexpr float Pi = 3.141592653589;

	static int DirectionKeyFromVector(FVector Vector)
	{
		float angle = Vector.HeadingAngle();


		//angle = angle < 0 ? 2 * Pi + angle : angle;

		if (angle < 0) {
			angle = angle + 2 * Pi;
		}

		angle = angle - Pi * 3 / 8;

		if (angle < 0) {
			angle = angle + 2 * Pi;
		}

		int result = static_cast<int>(4 * angle / Pi);


		/*if (GEngine) {
			FString xy = TEXT("direction angle: ") + FString::SanitizeFloat(angle) + TEXT("  result: ") + FString::FromInt(result);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, xy);
		}
		*/
		return result;
	}

	static int GetMovementFacingOffset(FVector movement_dir = FVector(0.f), FVector facing_dir = FVector(0.f)) {
		int movement_key = DirectionKeyFromVector(movement_dir);
		int facing_key = DirectionKeyFromVector(facing_dir);

		if (movement_key == facing_key) return LG_FORWARD;

		int offset = std::abs(movement_key - facing_key);
		
		if (offset > 4) offset = 8 - offset;

		if (offset < 3) return LG_SIDE;
		return LG_BACK;
	}

	/**
	* returns int corresponding to current movement state
	* 0 idle
	* 1 walking
	* 2 running
	* 3 sprinting
	* 4 walking sideways
	* 5 running sideways
	* 6 walking backwards
	* 7 running backwards
	*/

	static int GetMovementState(float velocity, float max_velocity, int offset) {
		if (velocity == 0.f) return 0;
		if (velocity > max_velocity) return 3;

		if (offset == LG_FORWARD) {
			if (velocity < 0.5f * max_velocity) return LG_WALK;
			return LG_RUN;
		}

		if (offset == LG_SIDE) {
			if (velocity < 0.5f * max_velocity) return LG_WALK_SIDE;
			return LG_RUN_SIDE;
		}

		if (offset == LG_BACK) {
			if (velocity < 0.5f * max_velocity) return LG_WALK_BACK;
			return LG_RUN_BACK;
		}

		return 0;
	}

	static int GetMovementState(float velocity, float max_velocity, FVector movement_dir = FVector(0.f), FVector facing_dir = FVector(0.f)) {
		if (velocity == 0.f) return 0;
		if (velocity > max_velocity) return 3;

		int offset = GetMovementFacingOffset(movement_dir, facing_dir);

		return GetMovementState(velocity, max_velocity, offset);

	}
};
