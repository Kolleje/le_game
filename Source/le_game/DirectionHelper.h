// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
//#include "MathUtil.h"

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
};
