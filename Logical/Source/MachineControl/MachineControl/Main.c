/*********************************************************************************
// Copyright: 	B&R Industrial Automation GmbH 
// Author:    	lauryr 
// Created:   	May 23, 2025/9:06 PM 
// Description: Main control program for cross cutter
//********************************************************************************/ 

#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
#include <AsDefault.h>
#endif

void _INIT ProgramInit(void)
{
	// Initializing Cam Sequencer function block
	MpAxisCamSequencer_CrossCutter.MpLink = &gAxis_Cutter;
	MpAxisCamSequencer_CrossCutter.MpLinkMaster = &gAxis_Conveyor;
	// Setting Cam Automat data object to the correct address and size
	MpAxisCamSeqParameters.CamSequence.Data.DataAddress = &CamAutomatParameters;
	MpAxisCamSeqParameters.CamSequence.Data.DataSize = sizeof(McAcpAxCamAutParType);
	// Passing the Cam Automat object name as a string
	brsstrcpy((UDINT) MpAxisCamSeqParameters.CamSequence.Data.DataObjectName, (UDINT) "CrossCamAut");
	// Initializing the Cam Automat parameters
	MpAxisCamSeqParameters.CamSequence.Get.Command = mcGET_PAR_FROM_OBJECT;
	MpAxisCamSeqParameters.CamSequence.Set.Command = mcSET_ALL_PAR_FROM_OBJ;
	MpAxisCamSeqParameters.CamSequence.Set.Mode = mcAXIS_CAM_SEQ_SET_ON_START;
	MpAxisCamSeqParameters.Deceleration = CAM_AUTOMAT_DECELERATION;
	
}

void _CYCLIC ProgramCyclic(void)
{
	
	// OVERALL OPERATION STATE MACHINE 
	switch (OperatingState) {
		// MANUAL MODE
		case ccMANUAL:
			// Setting the state string
			brsstrcpy((UDINT) MachineOperatingStateText, (UDINT) "Manual Mode");
			// If AutomaticMode toggle button is pressed, switch to Automatic Mode
			if (AutomaticMode) {
				if (CutterControl.Status.MoveActive) {
					brsstrcpy((UDINT) MachineStatus, (UDINT) "Machine in error");
					brsstrcpy((UDINT) MachineErrors, (UDINT) "Error switching to Automatic Mode: please stop the cutter first");
				} else {
					OperatingState = ccAUTOMATIC;
					ConveyorControl.Cmd.Stop = 1;
					CutterControl.Cmd.Stop = 1;
					AutomaticState = amINIT;
					ManualMode = 0;
				}
			}
			// Checking for errors and updating statuses accordingly
			if (ConveyorControl.Status.Error || CutterControl.Status.Error) {
				brsstrcpy((UDINT) MachineStatus, (UDINT) "Machine in error");
				brsstrcpy((UDINT) MachineErrors, (UDINT) "Manual mode error: check the Manual Page");
			} else {
				brsstrcpy((UDINT) MachineStatus, (UDINT) "Machine operating normally");
				brsstrcpy((UDINT) MachineErrors, (UDINT) "No errors present");
			}
			// All control is done manually via the HMI
			ConveyorControl;
			CutterControl;
			break;
		
		// AUTOMATIC MODE
		case ccAUTOMATIC:
			// Setting the state string
			brsstrcpy((UDINT) MachineOperatingStateText, (UDINT) "Automatic Mode");
			// Checking stop button
			if (AutomaticStop) {
				AutomaticState = amSTOP;
			}
			
			if (AutomaticReset) {
				ConveyorControl.Cmd.Reset = 1;
				CutterControl.Cmd.Reset = 1;
				MpAxisCamSequencer_CrossCutter.ErrorReset = 1;
				MpAxisCamSequencer_CrossCutter.GetSequence = 0;
				MpAxisCamSequencer_CrossCutter.StartSequence = 0;
				AutomaticState = amINIT;
				
			}
			// If ManualMode toggle button is pressed, switch to Manual Mode
			if (ManualMode) {
				if (CutterControl.Status.MoveActive || ConveyorControl.Status.MoveActive) { 
					brsstrcpy((UDINT) MachineStatus, (UDINT) "Machine in error");
					brsstrcpy((UDINT) MachineErrors, (UDINT) "Error switching to Manual Mode: please stop Automatic Mode first");
				} else {
					OperatingState = ccMANUAL;
					AutomaticMode = 0;
				}
			}
			// AUTOMATIC STATE MACHINE
			switch (AutomaticState) {
				case amINIT:
				
					if (AutomaticStart) {
						
						ConveyorControl.Cmd.Reset = 0;
						CutterControl.Cmd.Reset = 0;
						ConveyorControl.Cmd.Stop = 0;
						CutterControl.Cmd.Stop = 0;
						MpAxisCamSequencer_CrossCutter.ErrorReset = 0;
						
						MpAxisCamSequencer_CrossCutter.Enable = 1;
						
						if (ConveyorControl.Status.ReadyToStart && !ConveyorControl.Status.Error) {
							ConveyorControl.Cmd.Start = 1;
						} else if (ConveyorControl.Status.Error) {
							AutomaticState = amERROR;
						}
						// Turn on cutter if it is not already turned on
						if (CutterControl.Status.ReadyToStart && !CutterControl.Status.Error) {
							CutterControl.Cmd.Start = 1;
						}  else if (CutterControl.Status.Error) {
							AutomaticState = amERROR;
						}
						
						if (!PrintMarkControl.Status.Active) {
							PrintMarkControl.Cmd.Start = 1;
						} else if (PrintMarkControl.Status.Error) {
							AutomaticState = amERROR;
						}
						if (ConveyorControl.Status.ReadyForCommand && CutterControl.Status.ReadyForCommand) {
							ConveyorControl.Cmd.Start = 0;
							CutterControl.Cmd.Start = 0;
							AutomaticState = amGET_CAMAUT_DATA;
						} else if (ConveyorControl.Status.Error || CutterControl.Status.Error) {
							ConveyorControl.Cmd.Start = 0;
							CutterControl.Cmd.Start = 0;
							AutomaticState = amERROR;
						}
					}
					break;
					
				case amGET_CAMAUT_DATA:
					MpAxisCamSequencer_CrossCutter.GetSequence = 1;
					
					if (MpAxisCamSequencer_CrossCutter.GetSequenceDone) {
						AutomaticState = amWAITING;
					} else if (MpAxisCamSequencer_CrossCutter.Error) {
						AutomaticState = amERROR;
					}
				
					break;
			
			
			
				case amWAITING:
					if (Trigger1) {
						AutomaticState = amSTART_CAMAUT;
					}
					break;
			
				case amSTART_CAMAUT:
					if (MpAxisCamSequencer_CrossCutter.Active) {
						MpAxisCamSequencer_CrossCutter.StartSequence = 1;
						AutomaticState = amSTART_CONVEYOR;
					}
					break;
				
				case amSTART_CONVEYOR:
					if (!ConveyorControl.Status.MoveActive) {
						ConveyorControl.Cmd.MoveVelocity = 1;
					}
					if (ConveyorControl.Status.MoveActive) {
						AutomaticState = amSTART_MOVEMENT;
					}
					break;
			
				case amSTART_MOVEMENT:
					if (CutterEnable && PrintMarkControl.Status.ValidMarkDetected) {
						AutomaticState = amMOVING;
					}
					break;
			
				case amMOVING:
					if (!CutterEnable) {
						MpAxisCamSequencer_CrossCutter.Signal1 = 0;
						AutomaticState = amSTART_MOVEMENT;
					} else {
						MpAxisCamSequencer_CrossCutter.Signal1 = !PrintMarkControl.Status.ValidMarkDetected;
					}
			
				case amWAIT_CUTTING:
					
					break;
			
				case amPREPARE_UPDATE:
					break;
			
				case amERROR:
				
			
					break;
			
				case amSTOP:
					ConveyorControl.Cmd.Stop = 1;
					CutterControl.Cmd.Stop = 1;
					
					MpAxisCamSequencer_CrossCutter.StartSequence = 0;
					MpAxisCamSequencer_CrossCutter.Enable = 0;
						
					if (AutomaticStart) {
						OperatingState = amINIT;
					}
					break;
			
			
			}
	
			break;
		
	}
	
	// Assigning parameters and calling Cam Sequencer cyclically
	MpAxisCamSeqParameters.CamSequence.Data.DataAddress = &CamAutomatParameters;
	MpAxisCamSequencer_CrossCutter.Parameters = &MpAxisCamSeqParameters;
	MpAxisCamSequencer(&MpAxisCamSequencer_CrossCutter);
			
}

void _EXIT ProgramExit(void)
{

}

