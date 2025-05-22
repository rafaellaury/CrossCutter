/*********************************************************************************
 * Copyright: 	B&R Industrial Automation GmbH 
 * Author:    	lauryr 
 * Created:   	May 21, 2025/12:50 PM 
 * Description: Program to implement conveyor functionality
 *********************************************************************************/ 

#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
#include <AsDefault.h>
#include <AsBrStr.h>
#endif

// Helper function to assign parameters cyclically
void AssignParameters()
{
	// Setting the MpAxisParametersConveyor structure values cyclically
	MpAxisParametersConveyor.Velocity = ConveyorControl.Par.Velocity;
	MpAxisParametersConveyor.Acceleration = ConveyorControl.Par.Acceleration;
	MpAxisParametersConveyor.Deceleration = ConveyorControl.Par.Deceleration;
	MpAxisParametersConveyor.Jog.Velocity = ConveyorControl.Par.JogVelocity;
	MpAxisParametersConveyor.Jog.Acceleration = ConveyorControl.Par.JogAcceleration;
	MpAxisParametersConveyor.Jog.Deceleration = ConveyorControl.Par.JogDeceleration;
}

// Helper function to clear poorly documented encoder error
void ClearEncoderError()
{
	// Assign data for the function block to use
	EncoderProcessParIDType.ParID = ENCODER_COMMAND_PAR_ID;
	EncoderProcessParIDType.VariableAddress = &ENCODER_FIX_VALUE;
	EncoderProcessParIDType.DataType = mcACPAX_PARTYPE_UDINT;
	// Setting up the function block
	MC_BR_ProcessParID_Conveyor.Execute = 1;
	MC_BR_ProcessParID_Conveyor.Axis = &gAxis_Conveyor;
	MC_BR_ProcessParID_Conveyor.DataAddress = &EncoderProcessParIDType;
	MC_BR_ProcessParID_Conveyor.Mode = mcACPAX_PARID_SET;
	MC_BR_ProcessParID_Conveyor.NumberOfParIDs = 1;
	// Calling the function block
	MC_BR_ProcessParID_AcpAx(&MC_BR_ProcessParID_Conveyor);
}

void _INIT ProgramInit(void)
{
	
	// Setting maximum values for velocity, acceleration, deceleration
	ConveyorControl.Par.Velocity = MAX_CONVEYOR_VELOCITY;
	ConveyorControl.Par.Acceleration = MAX_CONVEYOR_ACCELERATION;
	ConveyorControl.Par.Deceleration = MAX_CONVEYOR_DECELERATION;
	// Setting maximum values for jog velocity, acceleration, deceleration
	ConveyorControl.Par.JogVelocity = MAX_CONVEYOR_VELOCITY;
	ConveyorControl.Par.JogAcceleration = MAX_CONVEYOR_ACCELERATION;
	ConveyorControl.Par.JogDeceleration = MAX_CONVEYOR_DECELERATION;
	
	// Enabling the Conveyor MpAxisBasic function block
	MpAxisBasic_Conveyor.Enable = 1;
	MpAxisBasic_Conveyor.MpLink = &gAxis_Conveyor;
	
	// Calling helper function to clear encoder error
	ClearEncoderError();
	
	// Initializing MC_Halt function block
	MC_Halt_Conveyor.Axis = &gAxis_Conveyor;
}

void _CYCLIC ProgramCyclic(void)
{
	// If stop command received, jump straight to Stop state
	if (ConveyorControl.Cmd.Stop) {
		ConveyorControl.Cmd.Stop = 0;
		MotionControlState = mcSTOP;
	}
	
	// MOTION CONTROL STATE MACHINE
	switch (MotionControlState) {
		
		// Initial state, wait for Start command and then move to power on state
		case mcINIT:
			// = 'Conveyor initializing';
			if (MpAxisBasic_Conveyor.Info.PLCopenState != mcAXIS_STARTUP && !MpAxisBasic_Conveyor.Error) {
				//Status = 'Conveyor ready for command';
				if (ConveyorControl.Cmd.Start) {
					ConveyorControl.Cmd.Start = 0;
					if (MpAxisBasic_Conveyor.Info.ReadyToPowerOn) {
						MotionControlState = mcPOWER;
					} 
				}
			} else if (MpAxisBasic_Conveyor.Error) {
				MotionControlState = mcERROR;
				ConveyorControl.Status.Error = 1;
			}
			break;
		
		// Power state, if not already powered on, power on the axis and move on to home
		case mcPOWER:
			//brsmemset();
			
			//ConveyorControl.Status.Status = "Conveyor powering on";
			if (!MpAxisBasic_Conveyor.PowerOn && !MpAxisBasic_Conveyor.Error) {
				MpAxisBasic_Conveyor.Power = 1;
				MotionControlState = mcHOME;
			} else if (MpAxisBasic_Conveyor.PowerOn && !MpAxisBasic_Conveyor.Error) {
				MotionControlState = mcHOME;	
			} else if (MpAxisBasic_Conveyor.Error) {
				MotionControlState = mcERROR;
				ConveyorControl.Status.Error = 1;
			}
			break;
		
		// Homing state, if not already homed, home the axis and move to ready for command
		case mcHOME:
			///////////ConveyorControl.Status.Status = "Conveyor homing";
			if (!MpAxisBasic_Conveyor.IsHomed && !MpAxisBasic_Conveyor.Error) {
				MpAxisBasic_Conveyor.Home = 1;
			} else if (MpAxisBasic_Conveyor.IsHomed && !MpAxisBasic_Conveyor.Error) {
				MpAxisBasic_Conveyor.Home = 0;
				ConveyorControl.Status.ReadyForCommand = 1;
				MotionControlState = mcACTIVE;
			} else if (MpAxisBasic_Conveyor.Error) {
				MpAxisBasic_Conveyor.Home = 0;
				MotionControlState = mcERROR;	
			}
			break;

		// Active state, process run commands
		case mcACTIVE:
			if (ConveyorControl.Cmd.MoveVelocity && !MpAxisBasic_Conveyor.Error) {
				///////////ConveyorControl.Status.Status = "Conveyor is moving";
				MpAxisBasic_Conveyor.MoveVelocity = 1;
				MpAxisBasic_Conveyor.JogPositive = 0;
				MpAxisBasic_Conveyor.JogNegative = 0;
			} else if (ConveyorControl.Cmd.JogForward && !MpAxisBasic_Conveyor.Error) {
				///////////ConveyorControl.Status.Status = "Conveyor jogging forward";
				MpAxisBasic_Conveyor.MoveVelocity = 0;
				MpAxisBasic_Conveyor.JogPositive = 1;
				MpAxisBasic_Conveyor.JogNegative = 0;
			} else if (ConveyorControl.Cmd.JogBackward && !MpAxisBasic_Conveyor.Error) {
				///////////ConveyorControl.Status.Status = "Conveyor jogging backward";
				// Resetting Halt
				if (MC_Halt_Conveyor.Execute) {
					MC_Halt_Conveyor.Execute = 0;
				}
				MpAxisBasic_Conveyor.MoveVelocity = 0;
				MpAxisBasic_Conveyor.JogPositive = 0;
				if (!MpAxisBasic_Conveyor.JogNegative) {
					JogPositionCheckpoint = MpAxisBasic_Conveyor.Position;
					MpAxisBasic_Conveyor.JogNegative = 1;
				} else {
					if (MpAxisBasic_Conveyor.Position <= JogPositionCheckpoint - 100) {
						MC_Halt_Conveyor.Execute = 1;
						MpAxisBasic_Conveyor.JogNegative = 0;
						ConveyorControl.Cmd.JogBackward = 0;
					}
				}
			} else if (MpAxisBasic_Conveyor.Error) {
				MpAxisBasic_Conveyor.MoveVelocity = 0;
				MpAxisBasic_Conveyor.JogPositive = 0;
				MpAxisBasic_Conveyor.JogNegative = 0;
				MotionControlState = mcERROR;
				ConveyorControl.Status.Error = 1;
			} else {
				MpAxisBasic_Conveyor.MoveVelocity = 0;
				MpAxisBasic_Conveyor.JogPositive = 0;
				MpAxisBasic_Conveyor.JogNegative = 0;
				///////////ConveyorControl.Status.Status = "Conveyor is active, ready for commands";
			}
			break;
		
		// Stop state, conveyor has been stopped
		case mcSTOP:
			ConveyorControl.Status.ReadyForCommand = 0;	
			ConveyorControl.Cmd.MoveVelocity = 0;
			ConveyorControl.Cmd.JogForward = 0;
			ConveyorControl.Cmd.JogBackward = 0;
			///////////ConveyorControl.Status.Status = "Conveyor stopped";
			if (MpAxisBasic_Conveyor.MoveActive && !MpAxisBasic_Conveyor.Error) {
				MC_Halt_Conveyor.Execute = 1;
			} else if (MpAxisBasic_Conveyor.Error) {
				ConveyorControl.Status.Error = 1;
				MotionControlState = mcERROR;
			} else if (ConveyorControl.Cmd.Start) {	
				MotionControlState = mcINIT;
			} else {
				MC_Halt_Conveyor.Execute = 0;
				MpAxisBasic_Conveyor.Power = 0;
			}
			break;

		// Error state, must reset to clear
		case mcERROR:
			///////////ConveyorControl.Status.Status = "Conveyor in error, needs to be reset";
			// Reset when signal is received
			ConveyorControl.Status.ReadyForCommand = 0;
			ConveyorControl.Cmd.MoveVelocity = 0;
			ConveyorControl.Cmd.JogForward = 0;
			ConveyorControl.Cmd.JogBackward = 0;
			MpAxisBasic_Conveyor.Power = 0;
			MpAxisBasic_Conveyor.ErrorReset = ConveyorControl.Cmd.Reset;
			if (!MpAxisBasic_Conveyor.Error) {
				ConveyorControl.Cmd.Reset = 0;
				MpAxisBasic_Conveyor.ErrorReset = 0;
				ConveyorControl.Status.Error = 0;
				MotionControlState = mcINIT;
			}
			break;
	}
	
	// Setting the MpAxisParametersConveyor structure values cyclically
	AssignParameters();
		
	// Assigning the address of the parameters to the Parameters of the MpAxisBasic function block
	MpAxisBasic_Conveyor.Parameters = &MpAxisParametersConveyor;
	// Calling the function block cyclically
	MpAxisBasic(&MpAxisBasic_Conveyor);
	// Assigning the statuses to the control structure
	ConveyorControl.Status.Error = MpAxisBasic_Conveyor.Error;
	ConveyorControl.Status.Position = MpAxisBasic_Conveyor.Position;
	
	// Calling MC_Halt function block cyclically and assigning deceleration
	MC_Halt_Conveyor.Deceleration = MpAxisParametersConveyor.Deceleration;
	MC_Halt(&MC_Halt_Conveyor);
	
}

void _EXIT ProgramExit(void)
{

}



