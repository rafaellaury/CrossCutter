(*global interface types*)

TYPE
	ConveyorStatusType : 	STRUCT  (*Status structure for Conveyor *)
		Position : LREAL; (*Position of the belt*)
		ReadyForCommand : BOOL; (*Ready to receive command*)
		Error : BOOL; (*Error bit*)
		Status : STRING[80] := ''; (*Status string*)
	END_STRUCT;
	ConveyorParType : 	STRUCT  (*Parameter structure for Conveyor*)
		Velocity : REAL; (*Velocity parameter*)
		Acceleration : REAL; (*Acceleration parameter*)
		Deceleration : REAL; (*Deceleration parameter*)
		JogVelocity : REAL; (*Jog velocity parameter*)
		JogAcceleration : REAL; (*Jog acceleration parameter*)
		JogDeceleration : REAL; (*Jog deceleration parameter*)
	END_STRUCT;
	ConveyorCommandType : 	STRUCT  (*Command structure for Conveyor*)
		Start : BOOL; (*Start command, called when machine boots up*)
		Reset : BOOL; (*Reset command to reset errors*)
		Stop : BOOL; (*Stop command*)
		JogForward : BOOL; (*Jog forward command*)
		JogBackward : BOOL; (*Jog backward command*)
		MoveVelocity : BOOL; (*Move velocity command*)
	END_STRUCT;
	ConveyorControlType : 	STRUCT  (*Control structure for conveyor*)
		Cmd : ConveyorCommandType; (*Commands*)
		Par : ConveyorParType; (*Parameters*)
		Status : ConveyorStatusType; (*Status variables*)
	END_STRUCT;
END_TYPE
