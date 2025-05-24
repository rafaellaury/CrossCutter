(*global interface types*)

TYPE
	CutterStatusType : 	STRUCT  (*Status structure for Cutter*)
		Position : LREAL; (*Position of the belt*)
		ReadyForCommand : BOOL; (*Ready to receive command*)
		ReadyToStart : BOOL; (*Ready to power on*)
		Error : BOOL; (*Error bit*)
		MoveActive : BOOL; (*True if moving*)
		Status : STRING[80] := ''; (*Status string*)
	END_STRUCT;
	CutterSyncRecipeType : 	STRUCT  (*Recipe structure for synchronization phase*)
		DegreesBefore : LREAL := 10; (*Degrees before cut position (180deg) to start synchronization*)
		DegreesAfter : LREAL := 10; (*Degrees after cut position (180deg) to end synchronization*)
		ConveyorDistance : LREAL := 20; (*Coorindated distance of conveyor (in mm)*)
	END_STRUCT;
	CutterParType : 	STRUCT  (*Parameter structure for Cutter*)
		Velocity : REAL; (*Velocity parameter*)
		Acceleration : REAL; (*Acceleration parameter*)
		Deceleration : REAL; (*Deceleration parameter*)
		JogVelocity : REAL; (*Jog velocity parameter*)
		JogAcceleration : REAL; (*Jog acceleration parameter*)
		JogDeceleration : REAL; (*Jog deceleration parameter*)
		Position : LREAL; (*Position parameter*)
		HomingMode : McHomingModeEnum := mcHOMING_DEFAULT; (*Take the homing mode from the configuration*)
		SyncRecipe : CutterSyncRecipeType; (*Recipe PV for the synchronized motion*)
		RightJogBoundary : LREAL := 160; (*Jog boundary right side, 20 degrees less than 180*)
		LeftJogBoundary : LREAL := 200; (*Jog boundary left side, 20 degrees more than 180*)
		Direction : McDirectionEnum := mcDIR_NEGATIVE; (*Negative direction to match conveyor direction*)
	END_STRUCT;
	CutterCommandType : 	STRUCT  (*Command structure for Cutter*)
		Start : BOOL; (*Start command, called when machine boots up*)
		Reset : BOOL; (*Reset command to reset errors*)
		Stop : BOOL; (*Stop command*)
		JogForward : BOOL; (*Jog forward command*)
		JogBackward : BOOL; (*Jog backward command*)
		MoveVelocity : BOOL; (*Move velocity command*)
		MoveAbsolute : USINT;
	END_STRUCT;
	CutterControlType : 	STRUCT  (*Control structure for Cutter*)
		Cmd : CutterCommandType; (*Commands*)
		Par : CutterParType; (*Parameters*)
		Status : CutterStatusType; (*Status variables*)
	END_STRUCT;
END_TYPE
