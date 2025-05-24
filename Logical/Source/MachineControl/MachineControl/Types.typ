(*local enumerated types*)

TYPE
	OperatingStateEnum : 
		( (*Operating state enum*)
		ccMANUAL := 0, (*Manual mode*)
		ccAUTOMATIC := 1 (*Automatic mode*)
		);
	AutomaticStateEnum : 
		( (*State enum for automatic mode*)
		amINIT, (*Initializing state*)
		amGET_CAMAUT_DATA, (*Get cam automat data*)
		amWAITING, (*Waiting for command state*)
		amENABLE_CAMAUT, (*Enable cam automat*)
		amSTART_CAMAUT, (*Start cam automat*)
		amSTART_CONVEYOR, (*Start conveyor (master axis)*)
		amSTART_MOVEMENT, (*Start movement*)
		amWAIT_CUTTING, (*Wait for cutting to complete (cutter pos > 180)*)
		amPREPARE_UPDATE, (*Prepare cam parameter update*)
		amCAM_IN, (*Cam in state*)
		amERROR, (*Error on axis or in automat*)
		amSTOP (*Automat has been stopped*)
		);
END_TYPE
