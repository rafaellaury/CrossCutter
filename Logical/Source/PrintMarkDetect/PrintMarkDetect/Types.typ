(*local enumerated types*)

TYPE
	PrintMarkStateEnum : 
		( (*Enumerated type for print mark state machine*)
		pmINIT := 0, (*Initial state*)
		pmDETECT, (*Turn on detection*)
		pmSEARCH, (*Enable search*)
		pmENABLE, (*Set enable again*)
		pmERROR
		);
END_TYPE
