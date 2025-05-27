//********************************************************************************
// Copyright: 	B&R Industrial Automation GmbH 
// Author:    	lauryr 
// Created:   	May 23, 2025/2:20 PM 
// Description: Program impelemting robust print mark detection for Cross Cutter
//********************************************************************************/ 

#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

// Init subroutine that runs only once
void _INIT ProgramInit(void)
{
	// Initializing the function block
	PrintMarkDetector.MpLink = &gCrossCutRegMarkDetection;
	PrintMarkDetectorConfig.MpLink = &gCrossCutRegMarkDetection;
	PrintMarkDetectorConfig.Configuration = &PrintMarkConfigStruct;
}
// Cyclic subroutine
void _CYCLIC ProgramCyclic(void)
{
	// If stopped, disable function block and move to stop state
	if (PrintMarkControl.Cmd.Stop) {
		PrintMarkDetector.Enable = 0;
		PrintMarkDetector.Detect = 0;
		PrintMarkDetector.Search = 0;
		PrintMarkControl.Status.Searching = 0;
		PrintMarkState = pmSTOP;
	}
	// Print Mark state machine
	switch (PrintMarkState) {
		// INIT STATE - initialize the function block
		case pmINIT:
			// If started, enable and move to the Detect state
			if (PrintMarkControl.Cmd.Start) {
				PrintMarkDetector.Enable = 1;
				PrintMarkDetectorConfig.Enable = 1;
				if (PrintMarkDetector.Active) {
					PrintMarkState = pmDETECT;	
				} else if (PrintMarkDetector.Error) {
					PrintMarkControl.Status.Error = 1;
					PrintMarkState = pmERROR;
				}
			}
			break;
		// DETECT STATE - enables detection
		case pmDETECT:
			PrintMarkDetector.Detect = 1;
			// Once detection is enabled, move to the Search state
			if (PrintMarkDetector.DetectionActive) {
				PrintMarkState = pmSEARCH;
			} else if (PrintMarkDetector.Error) {
				PrintMarkState = pmERROR;
			}
			break;
		// SEARCH STATE - enables searching
		case pmSEARCH:
			PrintMarkDetector.Search = 1;
			// Set Searching to true while Search is active
			if (PrintMarkDetector.SearchActive) {
				PrintMarkControl.Status.Searching = 1;
			} else if (PrintMarkDetector.Error) {
				PrintMarkState = pmERROR;
			}
			break;
		// STOP STATE - if start command received, move to INIT state to start operation
		case pmSTOP:
			if (PrintMarkControl.Cmd.Start) {
				PrintMarkState = pmINIT;
			}
		// ERROR STATE - set inputs to false and error reset
		case pmERROR:
			PrintMarkControl.Status.Error = 1;
			PrintMarkDetector.Detect = 0;
			PrintMarkDetector.Search = 0;
			PrintMarkControl.Status.Searching = 0;
			PrintMarkDetector.ErrorReset = 1;
			if (!PrintMarkDetector.Error) {
				PrintMarkControl.Status.Error = 0;
				PrintMarkState = pmINIT;
			}
			break;
	}
	
	// Calling the function blocks cyclically
	PrintMarkDetectorConfig.Configuration->Detection.MarkWidthMonitoring.MaximumWidth = PrintMarkControl.Par.PrintMarkMaxWidth;
	PrintMarkDetectorConfig.Configuration->Detection.MarkWidthMonitoring.MinimumWidth = PrintMarkControl.Par.PrintMarkMinWidth;
	PrintMarkDetectorConfig.Configuration->Detection.ExpectationWindow.Width = PrintMarkControl.Par.WindowWidth;
	MpRegMarkDetectionConfig(&PrintMarkDetectorConfig);
	MpRegMarkDetection(&PrintMarkDetector);
	
	// Setting status variables
	PrintMarkControl.Status.Active = PrintMarkDetector.Active;
	PrintMarkControl.Status.ValidMarkDetected = PrintMarkDetector.DetectionTrigger;
	PrintMarkControl.Status.TotalMissedMarks = PrintMarkDetector.Info.Detection.TotalMissedMarks;
	PrintMarkControl.Status.TotalValidMarks = PrintMarkDetector.Info.Detection.TotalValidMarks;
}


void _EXIT ProgramExit(void)
{
	// Disabling the function blocks
	PrintMarkDetector.Enable = 0;
	MpRegMarkDetection(&PrintMarkDetector);
	PrintMarkDetectorConfig.Enable = 0;
	MpRegMarkDetectionConfig(&PrintMarkDetectorConfig);
}
