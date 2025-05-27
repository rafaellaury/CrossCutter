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
	// COMMS ALARMS - 1000 range
	// Powerlink interface card alarm
	IF CommsErrors.PLKInterfaceCardError THEN
		MpAlarmXSet(gCrossCutAlarmXCore, 'Powerlink Adapter Alarm');
	ELSE
		MpAlarmXReset(gCrossCutAlarmXCore, 'Powerlink Adapter Alarm');
	END_IF
	// Powerlink interface 1 alarm
	IF CommsErrors.PLKInterface1Error THEN
		MpAlarmXSet(gCrossCutAlarmXCore, 'Powerlink Node 1 Alarm');
	ELSE
		MpAlarmXReset(gCrossCutAlarmXCore, 'Powerlink Node 1 Alarm');
	END_IF
	// Powerlink interface 2 alarm
	IF CommsErrors.PLKInterface2Error THEN
		MpAlarmXSet(gCrossCutAlarmXCore, 'Powerlink Node 2 Alarm');
	ELSE
		MpAlarmXReset(gCrossCutAlarmXCore, 'Powerlink Node 2 Alarm');
	END_IF
	// AUTOMATIC MODE ALARMS - 2000 range
	// Cammed motion alarm
	IF MachineControl.Status.AutoAlarm1 THEN
		MpAlarmXSet(gCrossCutAlarmXCore, 'Motion in cam alarm');
	ELSE
		MpAlarmXReset(gCrossCutAlarmXCore, 'Motion in cam alarm');
	END_IF
	// Cam automat data alarm
	IF MachineControl.Status.AutoAlarm2 THEN
		MpAlarmXSet(gCrossCutAlarmXCore, 'Cam automat data alarm');
	ELSE
		MpAlarmXReset(gCrossCutAlarmXCore, 'Cam automat data alarm');
	END_IF
	// Auto mode conveyor alarm
	IF MachineControl.Status.AutoAlarm3 THEN
		MpAlarmXSet(gCrossCutAlarmXCore, 'Automatic mode conveyor alarm');
	ELSE
		MpAlarmXReset(gCrossCutAlarmXCore, 'Automatic mode conveyor alarm');
	END_IF
	// Initialization alarm
	IF MachineControl.Status.AutoAlarm4 THEN
		MpAlarmXSet(gCrossCutAlarmXCore, 'Automatic initialization alarm');
	ELSE
		MpAlarmXReset(gCrossCutAlarmXCore, 'Automatic initialization alarm');
	END_IF
	// Automatic to Manual alarm
	IF MachineControl.Status.AutoAlarm5 THEN
		MpAlarmXSet(gCrossCutAlarmXCore, 'Automatic to Manual Alarm');
	ELSE
		MpAlarmXReset(gCrossCutAlarmXCore, 'Automatic to Manual Alarm');
	END_IF
	// MANUAL AND OTHER ALARMS - 3000 range
	// Manual to Automatic alarm
	IF MachineControl.Status.ManAlarm1 THEN
		MpAlarmXSet(gCrossCutAlarmXCore, 'Manual to Automatic Alarm');
	ELSE
		MpAlarmXReset(gCrossCutAlarmXCore, 'Manual to Automatic Alarm');
	END_IF
	// Conveyor alarm
	IF ConveyorControl.Status.Error THEN
		MpAlarmXSet(gCrossCutAlarmXCore, 'Conveyor alarm');
	ELSE
		MpAlarmXReset(gCrossCutAlarmXCore, 'Conveyor alarm');
	END_IF
	// Cutter alarm
	IF CutterControl.Status.Error THEN
		MpAlarmXSet(gCrossCutAlarmXCore, 'Cutter alarm');
	ELSE
		MpAlarmXReset(gCrossCutAlarmXCore, 'Cutter alarm');
	END_IF
	// Print mark alarm
	IF PrintMarkControl.Status.Error THEN
		MpAlarmXSet(gCrossCutAlarmXCore, 'Print mark alarm');
	ELSE
		MpAlarmXReset(gCrossCutAlarmXCore, 'Print mark alarm');
	END_IF
	
	IF MachineControl.Status.OverallErrorPresent THEN
		AlarmString := 'Media/SymbolLib/Alarm/Alarm.svg';
	ELSE
		AlarmString := '';
	END_IF
		
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

