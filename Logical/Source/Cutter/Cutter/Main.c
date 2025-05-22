/*********************************************************************************
 * Copyright: 	B&R Industrial Automation GmbH 
 * Author:    	lauryr 
 * Created:   	May 21, 2025/12:50 PM 
 * Description: Program to implement cutter functionality
 *********************************************************************************/ 

#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

// Helper function to assign parameters cyclically
void AssignParameters()
{
	// Setting the MpAxisParametersCutter structure values cyclically
	MpAxisParametersCutter.Position = CutterControl.Par.Position;
	MpAxisParametersCutter.Velocity = CutterControl.Par.Velocity;
	MpAxisParametersCutter.Acceleration = CutterControl.Par.Acceleration;
	MpAxisParametersCutter.Deceleration = CutterControl.Par.Deceleration;
	MpAxisParametersCutter.Homing.Mode = CutterControl.Par.HomingMode;
	MpAxisParametersCutter.Jog.Velocity = CutterControl.Par.JogVelocity;
	MpAxisParametersCutter.Jog.Acceleration = CutterControl.Par.JogAcceleration;
	MpAxisParametersCutter.Jog.Deceleration = CutterControl.Par.JogDeceleration;
}

// Helper function to clear poorly documented encoder error
void ClearEncoderError()
{
	// Assign data for the function block to use
	EncoderProcessParIDType.ParID = ENCODER_COMMAND_PAR_ID;
	EncoderProcessParIDType.VariableAddress = &ENCODER_FIX_VALUE;
	EncoderProcessParIDType.DataType = mcACPAX_PARTYPE_UDINT;
	// Setting up the function block
	MC_BR_ProcessParID_Cutter.Execute = 1;
	MC_BR_ProcessParID_Cutter.Axis = &gAxis_Cutter;
	MC_BR_ProcessParID_Cutter.DataAddress = &EncoderProcessParIDType;
	MC_BR_ProcessParID_Cutter.Mode = mcACPAX_PARID_SET;
	MC_BR_ProcessParID_Cutter.NumberOfParIDs = 1;
	// Calling the function block
	MC_BR_ProcessParID_AcpAx(&MC_BR_ProcessParID_Cutter);
}

// Init subroutine that runs only once
void _INIT ProgramInit(void)
{
	// Setting default position and maximum values for velocity, acceleration, deceleration
	CutterControl.Par.Position = DEFAULT_CUTTER_POSITION;
	CutterControl.Par.Velocity = MAX_CUTTER_VELOCITY;
	CutterControl.Par.Acceleration = MAX_CUTTER_ACCELERATION;
	CutterControl.Par.Deceleration = MAX_CUTTER_DECELERATION;
	// Setting maximum values for jog velocity, acceleration, deceleration
	CutterControl.Par.JogVelocity = MAX_CUTTER_VELOCITY;
	CutterControl.Par.JogAcceleration = MAX_CUTTER_ACCELERATION;
	CutterControl.Par.JogDeceleration = MAX_CUTTER_DECELERATION;
	
	// Enabling the Cutter MpAxisBasic function block
	MpAxisBasic_Cutter.Enable = 1;
	MpAxisBasic_Cutter.MpLink = &gAxis_Cutter;
	
	// Calling helper function to clear encoder error
	ClearEncoderError();
	
	// Initializing MC_Halt function block
	MC_Halt_Cutter.Axis = &gAxis_Cutter;
}

// Cyclic subroutine, run every cycle
void _CYCLIC ProgramCyclic(void)
{
	// If stop command received, jump straight to Stop state
	if (CutterControl.Cmd.Stop) {
		CutterControl.Cmd.Stop = 0;
		MotionControlState = mcSTOP;
	}
	
	// MOTION CONTROL STATE MACHINE
	switch (MotionControlState) {
		
		// Initial state, wait for Start command and then move to power on state
		case mcINIT:
			// = 'Cutter initializing';
			if (MpAxisBasic_Cutter.Info.PLCopenState != mcAXIS_STARTUP && !MpAxisBasic_Cutter.Error) {
				//Status = 'Cutter ready for command';
				if (CutterControl.Cmd.Start) {
					CutterControl.Cmd.Start = 0;
					if (MpAxisBasic_Cutter.Info.ReadyToPowerOn) {
						MotionControlState = mcPOWER;
					}
				}
			} else if (MpAxisBasic_Cutter.Error) {
				CutterControl.Cmd.Start = 0;
				MotionControlState = mcERROR;
				CutterControl.Status.Error = 1;
			}
			break;
		
		// Power state, if not already powered on, power on the axis and move on to home
		case mcPOWER:
			///////////CutterControl.Status.Status = "Cutter powering on";
			if (!MpAxisBasic_Cutter.PowerOn && !MpAxisBasic_Cutter.Error) {
				MpAxisBasic_Cutter.Power = 1;
				MotionControlState = mcHOME;
			} else if (MpAxisBasic_Cutter.PowerOn && !MpAxisBasic_Cutter.Error) {
				MotionControlState = mcHOME;	
			} else if (MpAxisBasic_Cutter.Error) {
				MotionControlState = mcERROR;
				CutterControl.Status.Error = 1;
			}
			break;
		
		// Homing state, if not already homed, home the axis and move to ready for command
		case mcHOME:
			///////////CutterControl.Status.Status = "Cutter homing";
			if (!MpAxisBasic_Cutter.IsHomed && !MpAxisBasic_Cutter.Error) {
				MpAxisBasic_Cutter.Home = 1;
			} else if (MpAxisBasic_Cutter.IsHomed && !MpAxisBasic_Cutter.Error) {
				MpAxisBasic_Cutter.Home = 0;
				if (MpAxisBasic_Cutter.Position != DEFAULT_CUTTER_POSITION) {
					CutterControl.Par.Position = 0;
					MpAxisBasic_Cutter.MoveAbsolute = 1;
				} else {
					MpAxisBasic_Cutter.MoveAbsolute = 0;
					CutterControl.Status.ReadyForCommand = 1;
					MotionControlState = mcACTIVE;
				}
			} else if (MpAxisBasic_Cutter.Error) {
				MotionControlState = mcERROR;	
			}
			break;

		// Active state, process run commands
		case mcACTIVE:
			if (CutterControl.Cmd.MoveVelocity && !MpAxisBasic_Cutter.Error) {
				///////////CutterControl.Status.Status = "Cutter is moving";
				MpAxisBasic_Cutter.MoveVelocity = 1;
				MpAxisBasic_Cutter.MoveAbsolute = 0;
				MpAxisBasic_Cutter.JogPositive = 0;
				MpAxisBasic_Cutter.JogNegative = 0;
			} else if (CutterControl.Cmd.MoveAbsolute && !MpAxisBasic_Cutter.Error) {
				///////////CutterControl.Status.Status = "Cutter is moving to position";
				MpAxisBasic_Cutter.MoveVelocity = 0;
				MpAxisBasic_Cutter.MoveAbsolute = 1;
				MpAxisBasic_Cutter.JogPositive = 0;
				MpAxisBasic_Cutter.JogNegative = 0;
			} else if (CutterControl.Cmd.JogForward && !MpAxisBasic_Cutter.Error) {
				///////////CutterControl.Status.Status = "Cutter jogging forward";
				// Resetting Halt
				if (MC_Halt_Cutter.Execute) {
					MC_Halt_Cutter.Execute = 0;
				}
				MpAxisBasic_Cutter.MoveVelocity = 0;
				MpAxisBasic_Cutter.MoveAbsolute = 0;
				// Avoid jogging through the cutting position
				if (MpAxisBasic_Cutter.Position < CutterControl.Par.RightJogBoundary || 
				MpAxisBasic_Cutter.Position > CutterControl.Par.LeftJogBoundary) {
					MpAxisBasic_Cutter.JogPositive = 1;
				} else {
					MC_Halt_Cutter.Execute = 1;
					MpAxisBasic_Cutter.JogPositive = 0;
				}
				MpAxisBasic_Cutter.JogNegative = 0;
			} else if (CutterControl.Cmd.JogBackward && !MpAxisBasic_Cutter.Error) {
				///////////CutterControl.Status.Status = "Cutter jogging backward";
				// Resetting Halt
				if (MC_Halt_Cutter.Execute) {
					MC_Halt_Cutter.Execute = 0;
				}
				MpAxisBasic_Cutter.MoveVelocity = 0;
				MpAxisBasic_Cutter.MoveAbsolute = 0;
				MpAxisBasic_Cutter.JogPositive = 0;
				// Avoid jogging through the cutting position
				if (MpAxisBasic_Cutter.Position < CutterControl.Par.RightJogBoundary || 
				MpAxisBasic_Cutter.Position > CutterControl.Par.LeftJogBoundary) {
					MpAxisBasic_Cutter.JogNegative = 1;
				} else {
					MC_Halt_Cutter.Execute = 1;
					MpAxisBasic_Cutter.JogNegative = 0;
				}
			} else if (MpAxisBasic_Cutter.Error) {
				MpAxisBasic_Cutter.MoveVelocity = 0;
				MpAxisBasic_Cutter.MoveAbsolute = 0;
				MpAxisBasic_Cutter.JogPositive = 0;
				MpAxisBasic_Cutter.JogNegative = 0;
				MotionControlState = mcERROR;
				CutterControl.Status.Error = 1;
			} else {
				MpAxisBasic_Cutter.MoveVelocity = 0;
				MpAxisBasic_Cutter.MoveAbsolute = 0;
				MpAxisBasic_Cutter.JogPositive = 0;
				MpAxisBasic_Cutter.JogNegative = 0;
				///////////CutterControl.Status.Status = "Cutter is active, ready for commands";
			}
			break;
		
		// Stop state, Cutter has been stopped
		case mcSTOP:
			CutterControl.Status.ReadyForCommand = 0;
			CutterControl.Cmd.MoveVelocity = 0;
			CutterControl.Cmd.JogForward = 0;
			CutterControl.Cmd.JogBackward = 0;
			///////////CutterControl.Status.Status = "Cutter stopped";
			if (MpAxisBasic_Cutter.MoveActive && !MpAxisBasic_Cutter.Error) {
				MC_Halt_Cutter.Execute = 1;
			} else if (MpAxisBasic_Cutter.Error) {
				CutterControl.Status.Error = 1;
				MotionControlState = mcERROR;
			} else if (CutterControl.Cmd.Start) {
				MotionControlState = mcINIT;
			} else {
				MC_Halt_Cutter.Execute = 0;	
				MpAxisBasic_Cutter.Power = 0;
			}
			break;

		// Error state, must reset to clear
		case mcERROR:
			///////////CutterControl.Status.Status = "Cutter in error, needs to be reset";
			// Reset when signal is received
			CutterControl.Status.ReadyForCommand = 0;
			CutterControl.Cmd.MoveVelocity = 0;
			CutterControl.Cmd.JogForward = 0;
			CutterControl.Cmd.JogBackward = 0;
			MpAxisBasic_Cutter.Power = 0;
			MpAxisBasic_Cutter.ErrorReset = CutterControl.Cmd.Reset;
			if (!MpAxisBasic_Cutter.Error) {
				CutterControl.Cmd.Reset = 0;
				MpAxisBasic_Cutter.ErrorReset = 0;
				CutterControl.Status.Error = 0;
				MotionControlState = mcINIT;
			}
			break;
	}
	
	// Setting the MpAxisParametersCutter structure values cyclically
	AssignParameters();
		
	// Assigning the address of the parameters to the Parameters of the MpAxisBasic function block
	MpAxisBasic_Cutter.Parameters = &MpAxisParametersCutter;
	// Calling the function block cyclically
	MpAxisBasic(&MpAxisBasic_Cutter);
	// Assigning the statuses to the control structure
	CutterControl.Status.Error = MpAxisBasic_Cutter.Error;
	CutterControl.Status.Position = MpAxisBasic_Cutter.Position;
	
	// Calling MC_Halt function block cyclically and assigning deceleration
	MC_Halt_Cutter.Deceleration = MpAxisParametersCutter.Deceleration;
	MC_Halt(&MC_Halt_Cutter);
	
}

void _EXIT ProgramExit(void)
{

}



