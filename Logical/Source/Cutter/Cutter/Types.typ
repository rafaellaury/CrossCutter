(*local enumerators*)

TYPE
	MotionControlEnum : 
		( (*Enum type for state machine variable*)
		mcINIT := 0, (*Intial state*)
		mcPOWER := 1, (*Powering on*)
		mcHOME := 2, (*Homing the axis*)
		mcACTIVE := 3, (*Waiting for a move command*)
		mcIN_CAM := 4, (*In cam*)
		mcSTOP := 5, (*Stopped*)
		mcERROR := 6 (*Error handling*)
		);
END_TYPE
