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

// Helper function to set jogging boundaries according to current position
void SetJoggingBoundaries()
{
	if (MpAxisParametersCutter.Position < 350.0 &&  MpAxisParametersCutter.Position > 180.0) {
		CutterOnLeft = 1;
		JogPositionLowerLimit = 200.0;
		JogPositionUpperLimit = 360.0;
		if (MpAxisBasic_Cutter.UpdateDone) {
			MpAxisBasic_Cutter.Update = 0;
		} 
		if (CutterOnRight) {
			MpAxisBasic_Cutter.Update = 1;
			CutterOnRight = 0;
		}
	} else if (MpAxisParametersCutter.Position > 10.0 &&  MpAxisParametersCutter.Position < 180.0) {
		CutterOnRight = 1;
		JogPositionLowerLimit = 0.0;
		JogPositionUpperLimit = 160.0;
		if (MpAxisBasic_Cutter.UpdateDone) {
			MpAxisBasic_Cutter.Update = 0;
		} 
		if (CutterOnLeft) {
			MpAxisBasic_Cutter.Update = 1;
			CutterOnLeft = 0;
		}
	} else {
		JogPositionLowerLimit = 0;
		JogPositionUpperLimit = 0;
		if (MpAxisBasic_Cutter.UpdateDone) {
			MpAxisBasic_Cutter.Update = 0;
		} 
		if (CutterOnLeft || CutterOnRight) {
			MpAxisBasic_Cutter.Update = 1;
			CutterOnLeft = 0;
			CutterOnRight = 0;
		}
	}
}

// Helper function to assign parameters cyclically
void AssignParameters()
{
	// Setting the MpAxisParametersCutter structure values cyclically
	MpAxisParametersCutter.Position = CutterControl.Par.Position;
	MpAxisParametersCutter.Velocity = CutterControl.Par.Velocity;
	MpAxisParametersCutter.Acceleration = CutterControl.Par.Acceleration;
	MpAxisParametersCutter.Deceleration = CutterControl.Par.Deceleration;
	MpAxisParametersCutter.Direction = CutterControl.Par.Direction;
	MpAxisParametersCutter.Homing.Mode = CutterControl.Par.HomingMode;
	MpAxisParametersCutter.Jog.Velocity = CutterControl.Par.JogVelocity;
	MpAxisParametersCutter.Jog.Acceleration = CutterControl.Par.JogAcceleration;
	MpAxisParametersCutter.Jog.Deceleration = CutterControl.Par.JogDeceleration;
	// Set the limits for jogging
	MpAxisParametersCutter.Jog.LimitPosition.FirstPosition = JogPositionLowerLimit;
	MpAxisParametersCutter.Jog.LimitPosition.LastPosition = JogPositionUpperLimit;
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
			brsstrcpy((UDINT) CutterControl.Status.Status, (UDINT) "Cutter initializing");
			if (MpAxisBasic_Cutter.Info.PLCopenState != mcAXIS_STARTUP && !MpAxisBasic_Cutter.Error) {
				if (ErrorClearAttempt) {
					MpAxisBasic_Cutter.ErrorReset = 0;
				}
				CutterControl.Status.ReadyToStart = 1;
				brsstrcpy((UDINT) CutterControl.Status.Status, (UDINT) "Cutter ready to start");
				if (CutterControl.Cmd.Start) {
					if (MpAxisBasic_Cutter.Info.ReadyToPowerOn) {
						CutterControl.Status.ReadyToStart = 0;
						// Calling helper function to clear encoder error
						ClearEncoderError();
						MotionControlState = mcPOWER;
					}
				}
			} else if (MpAxisBasic_Cutter.Error) {
				if (!ErrorClearAttempt) {
					MpAxisBasic_Cutter.ErrorReset = 1;
					ErrorClearAttempt = 1;
				} 
				else {
					CutterControl.Status.ReadyToStart = 0;
					MotionControlState = mcERROR;
					CutterControl.Status.Error = 1;
				}
			}
			break;
		
		// Power state, if not already powered on, power on the axis and move on to home
		case mcPOWER:
			brsstrcpy((UDINT) CutterControl.Status.Status, (UDINT) "Cutter powering on");
			if (!MpAxisBasic_Cutter.PowerOn && !MpAxisBasic_Cutter.Error) {
				MpAxisBasic_Cutter.Power = 1;
			} else if (MpAxisBasic_Cutter.PowerOn && !MpAxisBasic_Cutter.Error) {
				MotionControlState = mcHOME;	
			} else if (MpAxisBasic_Cutter.Error) {
				MotionControlState = mcERROR;
				CutterControl.Status.Error = 1;
			}
			break;
		
		// Homing state, if not already homed, home the axis and move to ready for command
		case mcHOME:
			brsstrcpy((UDINT) CutterControl.Status.Status, (UDINT) "Cutter homing");
			if (!MpAxisBasic_Cutter.IsHomed && !MpAxisBasic_Cutter.Error) {
				MpAxisBasic_Cutter.Home = 1;
			} else if (MpAxisBasic_Cutter.IsHomed && !MpAxisBasic_Cutter.Error) {
				MpAxisBasic_Cutter.Home = 0;
				if (MpAxisBasic_Cutter.Position != DEFAULT_CUTTER_POSITION) {
					CutterControl.Par.Position = 0;
					CutterControl.Par.Direction = mcDIR_POSITIVE;
					MpAxisBasic_Cutter.MoveAbsolute = 1;
				} else {
					MpAxisBasic_Cutter.MoveAbsolute = 0;
					CutterControl.Par.Direction = mcDIR_NEGATIVE;
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
				brsstrcpy((UDINT) CutterControl.Status.Status, (UDINT) "Cutter is moving");
				MpAxisBasic_Cutter.MoveVelocity = 1;
				MpAxisBasic_Cutter.MoveAbsolute = 0;
				MpAxisBasic_Cutter.JogPositive = 0;
				MpAxisBasic_Cutter.JogNegative = 0;
			} else if (CutterControl.Cmd.MoveAbsolute && !MpAxisBasic_Cutter.Error) {
				brsstrcpy((UDINT) CutterControl.Status.Status, (UDINT) "Cutter moving to set position");
				MpAxisBasic_Cutter.MoveVelocity = 0;
				MpAxisBasic_Cutter.MoveAbsolute = 1;
				MpAxisBasic_Cutter.JogPositive = 0;
				MpAxisBasic_Cutter.JogNegative = 0;
			} else if (CutterControl.Cmd.JogForward && !MpAxisBasic_Cutter.Error) {
				brsstrcpy((UDINT) CutterControl.Status.Status, (UDINT) "Cutter jogging forward");
				MpAxisBasic_Cutter.MoveVelocity = 0;
				MpAxisBasic_Cutter.MoveAbsolute = 0;
				MpAxisBasic_Cutter.JogNegative = 0;
				MpAxisBasic_Cutter.JogPositive = 1;
				// Avoid jogging through the cutting position
				SetJoggingBoundaries();
			} else if (CutterControl.Cmd.JogBackward && !MpAxisBasic_Cutter.Error) {
				brsstrcpy((UDINT) CutterControl.Status.Status, (UDINT) "Cutter jogging backward");
				MpAxisBasic_Cutter.MoveVelocity = 0;
				MpAxisBasic_Cutter.MoveAbsolute = 0;
				MpAxisBasic_Cutter.JogNegative = 1;
				MpAxisBasic_Cutter.JogPositive = 0;
				// Avoid jogging through the cutting position
				SetJoggingBoundaries();
			} else if (MpAxisBasic_Cutter.Error) {
				MpAxisBasic_Cutter.MoveVelocity = 0;
				MpAxisBasic_Cutter.MoveAbsolute = 0;
				MpAxisBasic_Cutter.JogPositive = 0;
				MpAxisBasic_Cutter.JogNegative = 0;
				MotionControlState = mcERROR;
				CutterControl.Status.Error = 1;
			} else if (MpAxisBasic_Cutter.Info.PLCopenState == mcAXIS_SYNCHRONIZED_MOTION) {
				MotionControlState = mcIN_CAM;
			} else {
				brsstrcpy((UDINT) CutterControl.Status.Status, (UDINT) "Cutter is active, ready for commands");
				MpAxisBasic_Cutter.MoveVelocity = 0;
				MpAxisBasic_Cutter.MoveAbsolute = 0;
				MpAxisBasic_Cutter.JogPositive = 0;
				MpAxisBasic_Cutter.JogNegative = 0;
				MpAxisBasic_Cutter.Update = 0;
			}
			break;
		
		// Cam is activated
		case mcIN_CAM:
			CutterControl.Status.ReadyForCommand = 0;
			//wait until cam is done
			//if (MpAxisBasic_Cutter.Info.PLCopenState != mcAXIS_SYNCHRONIZED_MOTION) {
		//		MotionControlState = mcINIT;
			//}
			break;
		
		// Stop state, Cutter has been stopped
		case mcSTOP:
			brsstrcpy((UDINT) CutterControl.Status.Status, (UDINT) "Cutter stopped");
			CutterControl.Status.ReadyForCommand = 0;
			CutterControl.Cmd.MoveVelocity = 0;
			CutterControl.Cmd.JogForward = 0;
			CutterControl.Cmd.JogBackward = 0;
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
			brsstrcpy((UDINT) CutterControl.Status.Status, (UDINT) "Cutter in error, needs to be reset");
			// Reset when signal is received
			CutterControl.Status.ReadyForCommand = 0;
			CutterControl.Cmd.MoveVelocity = 0;
			CutterControl.Cmd.JogForward = 0;
			CutterControl.Cmd.JogBackward = 0;
			MpAxisBasic_Cutter.Power = 0;
			MpAxisBasic_Cutter.ErrorReset = CutterControl.Cmd.Reset;
			if (!MpAxisBasic_Cutter.Error) {
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
	CutterControl.Status.MoveActive = MpAxisBasic_Cutter.MoveActive;
	
	// Calling MC_Halt function block cyclically and assigning deceleration
	MC_Halt_Cutter.Deceleration = MpAxisParametersCutter.Deceleration;
	MC_Halt(&MC_Halt_Cutter);
	
}

void _EXIT ProgramExit(void)
{

}



