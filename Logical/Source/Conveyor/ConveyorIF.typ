(*global interface types*)

TYPE
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
	ConveyorParType : 	STRUCT  (*Parameter structure for Conveyor*)
		Velocity : REAL; (*Velocity parameter*)
		Acceleration : REAL; (*Acceleration parameter*)
		Deceleration : REAL; (*Deceleration parameter*)
		JogVelocity : REAL; (*Jog velocity parameter*)
		JogAcceleration : REAL; (*Jog acceleration parameter*)
		JogDeceleration : REAL; (*Jog deceleration parameter*)
	END_STRUCT;
	ConveyorStatusType : 	STRUCT  (*Status structure for Conveyor *)
		Position : LREAL; (*[mm] Position of the belt*)
		Velocity : REAL; (*[mm/s] Actual velocity *)
		ReadyForCommand : BOOL; (*Ready to receive command*)
		ReadyToStart : BOOL; (*Ready to power on*)
		Status : STRING[80] := ''; (*Status string*)
		MoveActive : BOOL; (*True if moving*)
		Error : BOOL; (*Error bit*)
		StatusID : DINT; (*Status ID from MpAxis functionblock*)
		ErrorText : STRING[255] := 'No errors, axis is disabled'; (*Error text depending on where an error occured*)
		State : STRING[50] := 'Disabled'; (*State of the axis*)
	END_STRUCT;
END_TYPE
