
#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

void _INIT ProgramInit(void)
{
	// Initializing Cam Sequencer function block
	MpAxisCamSequencer_CrossCutter.Enable = 1;
	//MpAxisCamSequencer_CrossCutter.MpLink = &gAxis_Cutter;
	//MpAxisCamSequencer_CrossCutter.MpLinkMaster = &gAxis_Conveyor;
	

	
	
	MpAxisCamSeqParameters.Deceleration = CAM_AUTOMAT_DECELERATION;
	MpAxisCamSeqParameters.CamSequence.Data.DataAddress = &CamAutomatParameters;
	MpAxisCamSeqParameters.CamSequence.Get.Command = mcGET_PAR_FROM_OBJECT;
	//MpAxisCamSeqParameters.CamSequence.Get.GetOnEnable
	
}

void _CYCLIC ProgramCyclic(void)
{
	// OVERALL OPERATION STATE MACHINE
	switch (OperatingState) {
		// MANUAL MODE
		case ccMANUAL:
			// Turn on conveyor if it is not already turned on
			if (!ConveyorControl.Cmd.Start && !ConveyorControl.Status.ReadyForCommand && !ConveyorControl.Status.Error) {
				ConveyorControl.Cmd.Start = 1;
			} 
			// Turn on cutter if it is not already turned on
			if (!CutterControl.Cmd.Start && !CutterControl.Status.ReadyForCommand && !CutterControl.Status.Error) {
				CutterControl.Cmd.Start = 1;
			}
			break;
		
		// AUTOMATIC MODE
		case ccAUTOMATIC:
			
			switch (AutomaticState) {
			
				case amINIT:
					
					if (!ConveyorControl.Cmd.Start && !ConveyorControl.Status.ReadyForCommand && !ConveyorControl.Status.Error) {
					ConveyorControl.Cmd.Start = 1;
					} 
					// Turn on cutter if it is not already turned on
					if (!CutterControl.Cmd.Start && !CutterControl.Status.ReadyForCommand && !CutterControl.Status.Error) {
						CutterControl.Cmd.Start = 1;
					}
					if (ConveyorControl.Status.ReadyForCommand && CutterControl.Status.ReadyForCommand) {
						AutomaticState = amWAITING;
					}
					break;
					
				case amWAITING:
					if (Trigger1) {
						AutomaticState = amCAM_IN;
					}
					break;
							
				
				case amCAM_IN:
					
					break;
				
			
			
			}
	
			break;
		
	}
	
	// Assigning parameters and calling Cam Sequencer cyclically
	MpAxisCamSequencer_CrossCutter.Parameters = &MpAxisCamSeqParameters;
	MpAxisCamSequencer(&MpAxisCamSequencer_CrossCutter);
			
}

void _EXIT ProgramExit(void)
{

}

