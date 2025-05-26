(*local enumerated types*)

TYPE
	OperatingStateEnum : 
		( (*Operating state enum*)
		ccMANUAL := 0, (*Manual mode*)
		ccAUTOMATIC := 1 (*Automatic mode*)
		);
	AutomaticStateEnum : 
		( (*State enum for automatic mode*)
		amINIT := 1, (*Initializing state*)
		amGET_CAMAUT_DATA := 2, (*Get cam automat data*)
		amWAITING := 3, (*Waiting for command state*)
		amENABLE_CAMAUT := 4, (*Enable cam automat*)
		amSTART_CAMAUT := 5, (*Start cam automat*)
		amSTART_CONVEYOR := 6, (*Start conveyor (master axis)*)
		amSTART_MOVEMENT := 7, (*Start movement*)
		amMOVING := 8, (*Cam in motion*)
		amPREPARE_UPDATE := 9, (*Prepare cam parameter update*)
		amUPDATE_MOVEMENT := 10, (*Cam in state*)
		amSTOP := 11, (*Automat has been stopped*)
		amERROR := 12 (*Error on axis or in automat*)
		);
END_TYPE
