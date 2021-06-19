// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
#include "PaperCharacter.h"
#include "Dude.generated.h"

UCLASS(config = Game)
class ADude : public APaperCharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	ADude();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

protected:

	/** Called for forwards/backward input */
	void MoveX(float Value);

	/** Called for side to side input */
	void MoveY(float Value);

	/** Called for side to side input */
	void FaceX(float Value);

	/** Called for side to side input */
	void FaceY(float Value);
	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);
	void LookAtDirection(FVector2D Vector);
	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	void OnAim();
	void OnAimEnd();

	void OnSprint();
	void OnSprintEnd();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void Tick(float DeltaTime) override;
	
	/**
	 * returns int corresponding to current movement direction
	 * 0 down
	 * 1 down left
	 * 2 left
	 * 3 up left
	 * 4 up
	 * 5 up right
	 * 6 right
	 * 7 down right
	*/
	UFUNCTION(BlueprintCallable, Category = "Control")
	int GetMovementDirectionKey();

	/**
	 * returns int corresponding to current facing direction
	 * 0 down
	 * 1 down left
	 * 2 left
	 * 3 up left
	 * 4 up
	 * 5 up right
	 * 6 right
	 * 7 down right
	*/
	UFUNCTION(BlueprintCallable, Category = "Control")
	int GetFacingDirectionKey();


	/**
	* returns int corresponding to current movement state
	* 0 idle
	* 1 walking
	* 2 running
	* 3 sprinting
	* 4 walking sideways
	* 5 running sieways
	* 6 walking backwards
	* 7 running backwards
	*/ 
	UFUNCTION(BlueprintCallable, Category = "Control")
	int GetMovementState() { return movement_state; }

private:
	int movement_state = 0;
	int direction_offset = 0;
	int tick_count = 0;
	float time_passed = 0;
	bool _aiming = false;
	bool _sprinting = false;
	float right_axis = 0;
	float top_axis = 0;
	float base_speed = 0;
	FVector movement_dir = FVector(0.f);
	FVector look_dir = FVector(0.f);

	// Show 360 facing arrow
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* MovementDirectionArrow;

	// Show 360 facing arrow
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* FacingArrow;
};

