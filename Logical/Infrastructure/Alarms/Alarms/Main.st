(*********************************************************************************
 * Copyright:   B&R Industrial Automation GmbH 
 * Author:      lauryr 
 * Created:     May 22, 2025/10:27 PM 
 * Description: Structured text program implementing simple alarm handling
 *********************************************************************************)

PROGRAM _INIT
	
	// Enabling MpAlarmXCore function block
	MpAlarmXCore_0.Enable := TRUE;
	MpAlarmXCore_0.MpLink := ADR(gCrossCutAlarmXCore);
	
	// Enabling MpAlarmXHistory function block
	MpAlarmXHistory_0.Enable := TRUE;
	MpAlarmXHistory_0.MpLink := ADR(gCrossCutAlarmXHistory);
	
	// Enabling MpAlarmXAcknowledgeAll function block
	MpAlarmXAcknowledgeAll_0.Enable := TRUE;
	MpAlarmXAcknowledgeAll_0.MpLink := ADR(gCrossCutAlarmXCore);

END_PROGRAM

PROGRAM _CYCLIC
		
	// Calling all function blocks cyclically
	MpAlarmXCore_0();
	MpAlarmXHistory_0();
	MpAlarmXAcknowledgeAll_0();
	
END_PROGRAM

PROGRAM _EXIT
	
	// Disabling MpAlarmXCore function block
	MpAlarmXCore_0.Enable := FALSE;
	MpAlarmXCore_0();
	
	// Disabling MpAlarmXHistory function block
	MpAlarmXHistory_0.Enable := FALSE;
	MpAlarmXHistory_0();
	
	// Disabling MpAlarmXAcknowledgeAll function block
	MpAlarmXAcknowledgeAll_0.Enable := FALSE;
	MpAlarmXAcknowledgeAll_0();
	 
END_PROGRAM

