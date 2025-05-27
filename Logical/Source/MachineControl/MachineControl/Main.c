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
// Init subroutine
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
// Cyclic subroutine
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
				// If the cutter is still moving, ensure that it is stopped
				if (CutterControl.Status.MoveActive) {
					brsstrcpy((UDINT) MachineStatus, (UDINT) "Machine in error");
					brsstrcpy((UDINT) MachineErrors, (UDINT) "Error switching to Automatic Mode: please stop the cutter first");
					MachineError = 1;
					MachineControl.Status.ManAlarm1 = 1;
				} else {
					// Transition to automatic mode
					MachineError = 0;
					OperatingState = ccAUTOMATIC;
					MpAxisCamSequencer_CrossCutter.GetSequence = 0;
					MpAxisCamSequencer_CrossCutter.StartSequence = 0;
					MpAxisCamSequencer_CrossCutter.Enable = 0;
					MachineControl.Status.ManAlarm1 = 0;
					AutomaticState = amINIT;
					ManualMode = 0;
				}
			}
			// Checking for errors and updating statuses accordingly
			if (ConveyorControl.Status.Error || CutterControl.Status.Error) {
				brsstrcpy((UDINT) MachineStatus, (UDINT) "Machine in error");
				brsstrcpy((UDINT) MachineErrors, (UDINT) "Manual mode error: check the Manual Page");
				MachineError = 1;
			} else {
				brsstrcpy((UDINT) MachineStatus, (UDINT) "Machine operating normally");
				brsstrcpy((UDINT) MachineErrors, (UDINT) "No errors present");
				MachineError = 0;
			}
			// All control is done manually via the HMI
			ConveyorControl;
			CutterControl;
			break;
		// AUTOMATIC MODE
		case ccAUTOMATIC:
			// Setting the state string
			brsstrcpy((UDINT) MachineOperatingStateText, (UDINT) "Automatic Mode");
			brsstrcpy((UDINT) AutomaticStatus, (UDINT) "Automatic Mode starting up");
			// Checking stop button
			if (AutomaticStop) {
				AutomaticState = amSTOP;
			}
			// If Manual Mode button pressed, transition to Manual Mode
			if (ManualMode) {
				// If the Cutter is moving, warn that it must be stopped
				if (CutterControl.Status.MoveActive || ConveyorControl.Status.MoveActive) { 
					brsstrcpy((UDINT) MachineStatus, (UDINT) "Machine in error");
					brsstrcpy((UDINT) MachineErrors, (UDINT) "Error switching to Manual Mode: please stop Automatic Mode first");
					MachineControl.Status.AutoAlarm5 = 1;
					ManualMode = 0;
				} else {
					// If all looks good, update errors and move to Manual
					brsstrcpy((UDINT) AutomaticStatus, (UDINT) "Machine in Manual Mode");
					OperatingState = ccMANUAL;
					MachineControl.Status.AutoAlarm5 = 0;
					AutomaticMode = 0;
				}
			}
			// AUTOMATIC STATE MACHINE
			switch (AutomaticState) {
				case amINIT:
					brsstrcpy((UDINT) AutomaticStateText, (UDINT) "Initializing");
					brsstrcpy((UDINT) AutomaticStatus, (UDINT) "Machine powering and homing");
					brsstrcpy((UDINT) AutomaticErrorText, (UDINT) "No errors, normal operation");
					// Initializing variables
					PreviousCutPos = 0;
					ConveyorControl.Cmd.Reset = 0;
					CutterControl.Cmd.Reset = 0;
					ConveyorControl.Cmd.Stop = 0;
					CutterControl.Cmd.Stop = 0;
					PrintMarkControl.Cmd.Stop = 0;
					MpAxisCamSequencer_CrossCutter.ErrorReset = 0;
					// Enable CamSequencer
					MpAxisCamSequencer_CrossCutter.Enable = 1;
					// If conveyor is ready to start, send the Start command
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
					// Turn on Print Mark control
					if (!PrintMarkControl.Status.Active) {
						PrintMarkControl.Cmd.Start = 1;
					} else if (PrintMarkControl.Status.Error) {
						AutomaticState = amERROR;
					}
					// If both axes ready for command, disable start command and go to Get Cam Automat Data step
					if (ConveyorControl.Status.ReadyForCommand && CutterControl.Status.ReadyForCommand) {
						ConveyorControl.Cmd.Start = 0;
						CutterControl.Cmd.Start = 0;
						AutomaticState = amGET_CAMAUT_DATA;
					// If error, go to error state
					} else if (ConveyorControl.Status.Error || CutterControl.Status.Error) {
						brsstrcpy((UDINT) AutomaticErrorText, (UDINT) "Error while initializing");
						MachineControl.Status.AutoAlarm4 = 1;
						MachineError = 1;
						ConveyorControl.Cmd.Start = 0;
						CutterControl.Cmd.Start = 0;
						AutomaticState = amERROR;
					}
					break;
				// GET CAM AUTOMAT DATA - state to send get command to Cam Sequencer
				case amGET_CAMAUT_DATA:
					brsstrcpy((UDINT) AutomaticStateText, (UDINT) "Get Cam Automat Data");
					brsstrcpy((UDINT) AutomaticStatus, (UDINT) "Preparing the cam automat");
					// Trigger the GetSequence command
					MpAxisCamSequencer_CrossCutter.GetSequence = 1;
					// When done, move to Waiting state
					if (MpAxisCamSequencer_CrossCutter.GetSequenceDone) {
						AutomaticState = amWAITING;
					} else if (MpAxisCamSequencer_CrossCutter.Error) {
						AutomaticState = amERROR;
						MachineControl.Status.AutoAlarm3 = 1;
						brsstrcpy((UDINT) AutomaticErrorText, (UDINT) "Error getting cam automat data");
					}
					break;
				// WAITING state - machine is initialized, waiting for Start command
				case amWAITING:
					brsstrcpy((UDINT) AutomaticStateText, (UDINT) "Waiting");
					brsstrcpy((UDINT) AutomaticStatus, (UDINT) "Press Start to run machine");
					// When Start button pressed, move to the next state
					if (AutomaticStart) {
						AutomaticState = amSTART_CAMAUT;
					}
					break;
				// START_CAMAUT state - beginnig the Cam Automat sequencer
				case amSTART_CAMAUT:
					brsstrcpy((UDINT) AutomaticStateText, (UDINT) "Start Cam Automat");
					brsstrcpy((UDINT) AutomaticStatus, (UDINT) "Cam Automat has been enabled");
					// If the Sequencer is active, advance to the next state
					if (MpAxisCamSequencer_CrossCutter.Active) {
						MpAxisCamSequencer_CrossCutter.StartSequence = 1;
						AutomaticState = amSTART_CONVEYOR;
					}
					break;
				// START_CONVEYOR state - start the cam automat master axis
				case amSTART_CONVEYOR:
					brsstrcpy((UDINT) AutomaticStateText, (UDINT) "Start Conveyor");
					brsstrcpy((UDINT) AutomaticStatus, (UDINT) "Starting master axis motion");
					if (!ConveyorControl.Status.MoveActive) {
						ConveyorControl.Cmd.MoveVelocity = 1;
					}
					// If it is moving, advance to the next state
					if (ConveyorControl.Status.MoveActive) {
						AutomaticState = amSTART_MOVEMENT;
					} else if (ConveyorControl.Status.Error) {
						brsstrcpy((UDINT) AutomaticErrorText, (UDINT) "Error in conveyor axis");
						MachineControl.Status.AutoAlarm2 = 1;
						MachineError = 1;
						AutomaticState = amERROR;
					}
					break;
				// START_MOVEMENT state - movement has begun and now waiting for Cutter
				case amSTART_MOVEMENT:
					brsstrcpy((UDINT) AutomaticStateText, (UDINT) "Start Movement");
					brsstrcpy((UDINT) AutomaticStatus, (UDINT) "Enable Cutter to begin cam");
					// If cutter is enabled, AND Print Mark has been detected, move to next state
					if (CutterEnable && PrintMarkControl.Status.ValidMarkDetected) {
						AutomaticState = amMOVING;
					}
					break;
				// MOVING STATE - cammed motion state
				case amMOVING:
					brsstrcpy((UDINT) AutomaticStateText, (UDINT) "Moving");
					brsstrcpy((UDINT) AutomaticStatus, (UDINT) "Cross cutter running in cam");
					// If the Cutter is not enabled, go back to START_MOVEMENT state
					if (!CutterEnable) {
						MpAxisCamSequencer_CrossCutter.Signal1 = 0;
						AutomaticState = amSTART_MOVEMENT;
					} else {
						// If it is enabled, set the Cam signal to the inverse of the PrintMark
						MpAxisCamSequencer_CrossCutter.Signal1 = !PrintMarkControl.Status.ValidMarkDetected;
					}
					// If a cut has been made, update the factors and compensation according to inputs from recipe
					if (CutterControl.Status.Position > 180.0) {
						CamAutomatParameters.State[2].MasterFactor = (MachineControl.Par.SyncRecipe.ConveyorDistance) * 100.0;
						CamAutomatParameters.State[2].SlaveFactor = (MachineControl.Par.SyncRecipe.DegreesAfter + MachineControl.Par.SyncRecipe.DegreesBefore) * -100.0;
						CamAutomatParameters.State[3].MasterFactor = (MachineControl.Par.SyncRecipe.ConveyorDistance) * 100.0;
						CamAutomatParameters.State[3].SlaveFactor = (MachineControl.Par.SyncRecipe.DegreesAfter + MachineControl.Par.SyncRecipe.DegreesBefore) * -100.0;
						CamAutomatParameters.State[4].CompensationParameters.SlaveCompDistance = (180.0 - MachineControl.Par.SyncRecipe.DegreesAfter) * -1;MpAxisCamSeqParameters.CamSequence.Set.Command = mcSET_UPDATE_FROM_ADR;
						MpAxisCamSequencer_CrossCutter.Update = 1;
						// Update the parameters
						if (MpAxisCamSequencer_CrossCutter.UpdateDone) {
							MpAxisCamSequencer_CrossCutter.Update = 0;
						}
					}
					if (MpAxisCamSequencer_CrossCutter.Error) {
						brsstrcpy((UDINT) AutomaticErrorText, (UDINT) "Error during cammed motion");
						MachineControl.Status.AutoAlarm1 = 1;
						MachineError = 1;
						AutomaticState = amERROR;
					}
					break;
				// ERROR STATE - any error is encountered will cause this state, reset to get out
				case amERROR:
					brsstrcpy((UDINT) AutomaticStateText, (UDINT) "Error");
					brsstrcpy((UDINT) AutomaticStatus, (UDINT) "Error during operation");
					// Setting overall machine error to the sum of the individual components
					MachineError = ConveyorControl.Status.Error && CutterControl.Status.Error && MpAxisCamSequencer_CrossCutter.Error && PrintMarkControl.Status.Error;
					ConveyorControl.Cmd.Reset = AutomaticReset;
					CutterControl.Cmd.Reset = AutomaticReset;
					MpAxisCamSequencer_CrossCutter.ErrorReset = AutomaticReset;
					// If Reset is triggered, clear all errors
					if (AutomaticReset) {
						MachineControl.Status.AutoAlarm1 = 0;
						MachineControl.Status.AutoAlarm2 = 0;
						MachineControl.Status.AutoAlarm3 = 0;
						MachineControl.Status.AutoAlarm4 = 0;
						MachineControl.Status.AutoAlarm5 = 0;
						MpAxisCamSequencer_CrossCutter.Enable = 0;
						MpAxisCamSequencer_CrossCutter.Update = 0;
						PrintMarkControl.Status.AverageDistance = 0;
						Index = 1;
					}
					// If Errors cleared, go to INIT state
					if (MachineError = 0) {
						brsstrcpy((UDINT) MachineErrors, (UDINT) "No errors present");
						brsstrcpy((UDINT) AutomaticErrorText, (UDINT) "No errors, normal operation");
						AutomaticState = amINIT;
					}
					break;
				// STOP STATE - when stop button is pushed, machine is turned off
				case amSTOP:
					brsstrcpy((UDINT) AutomaticStateText, (UDINT) "Stop");
					brsstrcpy((UDINT) AutomaticStatus, (UDINT) "Machine is stopped");
					// Set conveyor and print mark and sequencer to turn off
					ConveyorControl.Cmd.Stop = 1;
					PrintMarkControl.Cmd.Stop = 1;
					MpAxisCamSequencer_CrossCutter.Update = 0;
					MpAxisCamSequencer_CrossCutter.GetSequence = 0;
					MpAxisCamSequencer_CrossCutter.Enable = 0;
					CutterEnable = 0;
					// If Start command received, turn the machine back on
					if (AutomaticStart) {
						AutomaticStop = 0;
						AutomaticState = amINIT;
					}
					break;
			}
			// During automatic state, assign speed to the Conveyor converting from products per minute
			ConveyorControl.Par.Velocity = ProductsPerMinVelocity * VELOCITY_SCALING_FACTOR; // Experimentally determined to get back to mm/s
			// End automatic state
			break;
	}
	// Overall machine error boolean
	MachineControl.Status.OverallErrorPresent = MachineError || ConveyorControl.Status.Error ||
		CutterControl.Status.Error || PrintMarkControl.Status.Error;
	// Updating print mark width
	if (PrintMarkControl.Status.ValidMarkDetected) {
		if (PreviousCutPos == 0) {
			PreviousCutPos = ConveyorControl.Status.Position;
		}
	} 
	if (PreviousCutPos != 0 && !PrintMarkControl.Status.ValidMarkDetected) {
		PrintMarkControl.Status.LastPrintMarkWidth = ConveyorControl.Status.Position - PreviousCutPos;
		PreviousCutPos = 0;
	}
	// Updating average and last distance
	if (PrintMarkControl.Status.LastCutDistance != OldLastCut) {
		PrintMarkControl.Status.AverageDistance = (PrintMarkControl.Status.LastCutDistance + PrintMarkControl.Status.AverageDistance) / Index;
		Index = Index + 1;
	}
	OldLastCut = PrintMarkControl.Status.LastCutDistance;
	
	// Assigning parameters and calling Cam Sequencer cyclically
	MpAxisCamSeqParameters.CamSequence.Data.DataAddress = &CamAutomatParameters;
	MpAxisCamSequencer_CrossCutter.Parameters = &MpAxisCamSeqParameters;
	MpAxisCamSequencer(&MpAxisCamSequencer_CrossCutter);
}

void _EXIT ProgramExit(void)
{

}

