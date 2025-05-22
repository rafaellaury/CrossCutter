(*local enumerated types*)

TYPE
	OperatingStateEnum : 
		( (*Operating state enum*)
		ccMANUAL := 0, (*Manual mode*)
		ccAUTOMATIC := 1 (*Automatic mode*)
		);
	AutomaticStateEnum : 
		( (*State enum for automatic mode*)
		amINIT,
		amWAITING,
		amCAM_IN,
		amERROR
		);
END_TYPE
