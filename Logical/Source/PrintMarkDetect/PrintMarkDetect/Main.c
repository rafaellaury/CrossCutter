/*********************************************************************************
 * Copyright: 	B&R Industrial Automation GmbH 
 * Author:    	lauryr 
 * Created:   	May 23, 2025/2:20 PM 
 * Description: Program impelemting robust print mark detection for Cross Cutter
 *********************************************************************************/ 

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

void _CYCLIC ProgramCyclic(void)
{
	
	switch (PrintMarkState) {
	
		case pmINIT:
			
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
			
		case pmDETECT:
			PrintMarkDetector.Detect = 1;
			if (PrintMarkDetector.DetectionActive) {
				PrintMarkState = pmSEARCH;
			} else if (PrintMarkDetector.Error) {
				PrintMarkState = pmERROR;
			}
			break;
			
		case pmSEARCH:
			PrintMarkDetector.Search = 1;
			if (PrintMarkDetector.SearchActive) {
				PrintMarkControl.Status.Searching = 1;
			} else if (PrintMarkDetector.Error) {
				PrintMarkState = pmERROR;
			}
			break;
		
		
	}
	
	// Calling the function blocks cyclically
	MpRegMarkDetection(&PrintMarkDetector);
	MpRegMarkDetectionConfig(&PrintMarkDetectorConfig);
	
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
