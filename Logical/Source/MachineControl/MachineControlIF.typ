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
	MachineControlType : 	STRUCT  (*Machine control type*)
		Par : MachineParType; (*Parameters*)
	END_STRUCT;
END_TYPE
