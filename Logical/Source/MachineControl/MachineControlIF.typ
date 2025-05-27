(*global interface structure types*)

TYPE
	SyncRecipeType : 	STRUCT  (*Recipe structure for synchronization phase*)
		DegreesBefore : LREAL := 10; (*[°] Degrees before cut position (180deg) to start synchronization*)
		DegreesAfter : LREAL := 10; (*[°] Degrees after cut position (180deg) to end synchronization*)
		ConveyorDistance : LREAL := 20; (*[mm] Coorindated distance of conveyor (in mm)*)
	END_STRUCT;
	MachineParType : 	STRUCT  (*Parameter type for machine control*)
		SyncRecipe : SyncRecipeType; (*Recipe for synchronized zone*)
	END_STRUCT;
	MachineStatusType : 	STRUCT  (*Status type*)
		OverallErrorPresent : BOOL; (*Error is present somewhere in the system*)
		AutoAlarm1 : BOOL; (*Alarm tracker 1*)
		AutoAlarm2 : BOOL; (*Alarm tracker 2*)
		AutoAlarm3 : BOOL; (*Alarm tracker 3*)
		AutoAlarm4 : BOOL; (*Alarm tracker 4*)
		AutoAlarm5 : BOOL; (*Alarm tracker 5*)
		ManAlarm1 : BOOL; (*Manual alarm tracker 1*)
	END_STRUCT;
	MachineControlType : 	STRUCT  (*Machine control type*)
		Par : MachineParType; (*Parameters*)
		Status : MachineStatusType; (*Statuses*)
	END_STRUCT;
END_TYPE
