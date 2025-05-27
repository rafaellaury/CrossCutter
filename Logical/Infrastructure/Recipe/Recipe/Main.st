(*********************************************************************************
 * Copyright:   B&R Industrial Automation GmbH 
 * Author:      lauryr 
 * Created:     May 22, 2025/10:31 PM 
 * Description: Recipe system to specify the synchronized section
 *********************************************************************************)

PROGRAM _INIT
	
	// Initialize MpRecipeXml function block
	MpRecipeXml_0.Enable := TRUE;
	MpRecipeXml_0.MpLink := ADR(gCrossCutRecipeXml);
	MpRecipeXml_0.DeviceName := ADR('USER');
	// Initialize MpRecipeRegPar function block and assign PV
	MpRecipeRegPar_0.Enable := TRUE;
	MpRecipeRegPar_0.MpLink := ADR(gCrossCutRecipeXml);
	MpRecipeRegPar_0.PVName := ADR('::MachineControl.Par.SyncRecipe');
	// Initialize MpRecipeUI function block
	MpRecipeUI_0.Enable := TRUE;
	MpRecipeUI_0.MpLink := ADR(gCrossCutRecipeXml);
	MpRecipeUI_0.UIConnect := ADR(MpRecipeUIConnect);
	MpRecipeUIConnect.Recipe.SortOrder := mpRECIPE_UI_SORT_DATE_ASCENDING;
	 
END_PROGRAM

PROGRAM _CYCLIC
	
	// Calling selected value for use in other tasks
	selectedValue;
	// Call CreateRecipe action to create default recipe
	CreateRecipes;
	// Call NewRecipe action to check if new recipe is being created
	IF MpRecipeUIConnect.Recipe.List.MaxSelection <= 19 THEN
		CreateNewRecipeAction;
	END_IF
	
	IF LoadRecipe THEN
		MpRecipeXml_0.FileName := ADR(selectedValue);
		MpRecipeXml_0.Load := TRUE;
	END_IF
	
	IF MpRecipeXml_0.Load = TRUE AND MpRecipeXml_0.CommandDone THEN
		MpRecipeXml_0.Load := FALSE;
	END_IF
	
		
	// Cyclically call all function blocks
	MpRecipeXml_0();
	MpRecipeRegPar_0();
	MpRecipeUI_0();
	 
END_PROGRAM

PROGRAM _EXIT
	
	// Disable MpRecipeXml function block
	MpRecipeXml_0.Enable := FALSE;
	MpRecipeXml_0();
	// Disable MpRecipeRegPar function block
	MpRecipeRegPar_0.Enable := FALSE;
	MpRecipeRegPar_0();
	// Disable MpRecipeUI function block
	MpRecipeUI_0.Enable := FALSE;
	MpRecipeUI_0();
		
END_PROGRAM

