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
			// If manual mode is started
			if (ManualStart) {
				// Turn on cutter if it is not already turned on
				if (CutterControl.Status.ReadyToStart && !CutterControl.Status.Error) {
					CutterControl.Cmd.Start = 1;
				}
				// Turn on conveyor if it is not already turned on
				if (ConveyorControl.Status.ReadyToStart && CutterControl.Status.ReadyForCommand && !ConveyorControl.Status.Error) {
					ConveyorControl.Cmd.Start = 1;
   Conveyor	Program impelementing conveyor functionality	Directory="C:\Users\lauryr\Downloads\ecamp\04\CrossCutter\Logical\Source\Conveyor\Conveyor";Path="C:\Users\lauryr\Downloads\ecamp\04\CrossCutter\Logical\Source\Conveyor\Conveyor\ANSIC.prg";Extension=".prg";RefPath="";Reference="False"
				}
			}
			break;
		
		// AUTOMATIC MODE
		case ccAUTOMATIC:
			
			if (AutomaticStop) {
				AutomaticState = amSTOP;
			}
			
			switch (AutomaticState) {
				case amINIT:
					if (AutomaticStart) {
						
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
							MpAxisCamSequencer_CrossCutter.Enable = 1;
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
					if (CutterEnable) {
						MpAxisCamSequencer_CrossCutter.Signal1 = PrintMarkControl.Status.ValidMarkDetected;
					} else {
						MpAxisCamSequencer_CrossCutter.Signal1 = 0;
					}
					break;
			
			case amWAIT_CUTTING:
					if (CutterControl.Status.Position > 180) {
						AutomaticState = amPREPARE_UPDATE;
					}
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

