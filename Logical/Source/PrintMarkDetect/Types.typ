(*global interface types*)

TYPE
	PrintMarkCommandType : 	STRUCT  (*Commands for print mark type*)
		Start : BOOL; (*Start detecting print marks*)
		Stop : BOOL; (*Stop detecting print marks*)
	END_STRUCT;
	PrintMarkParType : 	STRUCT  (*Print mark parameter type*)
		Temp : BOOL; (*Temp*)
	END_STRUCT;
	PrintMarkStatusType : 	STRUCT  (*Statuses for print mark*)
		ValidMarkDetected : BOOL; (*Print mark detected*)
		TotalMissedMarks : UDINT; (*Total missed marks*)
		TotalValidMarks : UDINT; (*Total valid marks*)
		AverageDistance : LREAL; (*Average distance between consecutive print marks*)
		LastPrintMarkWidth : LREAL; (*Width of last print mark*)
		LastCutDistance : LREAL; (*Distance between the current and last cut*)
		PositionError : LREAL; (*Position error*)
		ActualPosition : LREAL; (*Actual position*)
		Error : BOOL; (*True if error present*)
		Searching : BOOL; (*Searching is active*)
		Active : BOOL; (*True if active*)
	END_STRUCT;
	PrintMarkControlType : 	STRUCT  (*Control type for print mark*)
		Cmd : PrintMarkCommandType; (*Commands *)
		Par : PrintMarkParType; (*Parameters*)
		Status : PrintMarkStatusType; (*Statuses*)
	END_STRUCT;
END_TYPE
