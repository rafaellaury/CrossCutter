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
		amWAIT_CUTTING := 9, (*Wait for cutting to complete (cutter pos > 180)*)
		amPREPARE_UPDATE := 10, (*Prepare cam parameter update*)
		amCAM_IN := 11, (*Cam in state*)
		amERROR := 12, (*Error on axis or in automat*)
		amSTOP := 13 (*Automat has been stopped*)
		);
END_TYPE
