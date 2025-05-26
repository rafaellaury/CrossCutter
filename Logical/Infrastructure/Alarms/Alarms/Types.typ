(*local structure types*)

TYPE
	CommsErrorsType : 	STRUCT  (*Structure containing comms errors*)
		PLKInterfaceCardError : BOOL; (*Error with Powerlink interface card*)
		PLKInterface1Error : BOOL; (*Error with PLK interface 1*)
		PLKInterface2Error : BOOL; (*Error with PLK interface 2*)
	END_STRUCT;
END_TYPE
