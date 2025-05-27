(*local enumerated types*)

TYPE
	PrintMarkStateEnum : 
		( (*Enumerated type for print mark state machine*)
		pmINIT := 0, (*Initial state*)
		pmDETECT := 1, (*Turn on detection*)
		pmSEARCH := 2, (*Enable search*)
		pmENABLE := 3, (*Set enable again*)
		pmSTOP := 4, (*Stop state*)
		pmERROR := 5 (*Error state*)
		);
END_TYPE
