(*********************************************************************************
 * Copyright:   B&R Industrial Automation GmbH 
 * Author:      lauryr 
 * Created:     May 26, 2025/5:12 PM 
 * Description: Action file to create new recipes and edit recipes
 *********************************************************************************)

ACTION CreateNewRecipeAction: 

	IF CancelCreation THEN
		RecipeCreateStepNew := DONE_1;
	END_IF
		
	CASE RecipeCreateStepNew OF
		IDLE:	
			
			IF MpRecipeXml_0.Active = TRUE AND MpRecipeUI_0.Active = TRUE THEN
				
				// if New Recipe is clicked
				IF (NewRecipe) THEN
					RecipeStatus := 'Please enter new recipe data';
					CreateAccepted := FALSE;
					RecipeCreateStepNew := CREATE_1;
				END_IF

			END_IF
			
		CREATE_1:	// Default recipe 
			
			// Checking values entered on HMI
			MpRecipeRegPar_0.PVName := ADR('::Recipe:NewSyncRecipe');
			
			IF NewSyncRecipe.DegreesAfter <> NewSyncRecipe.DegreesBefore THEN
				RecipeStatus := 'Degrees Before and Degrees After must be equal';
				CreateAccepted := FALSE;
			ELSIF NewSyncRecipe.ConveyorDistance <> (NewSyncRecipe.DegreesAfter + NewSyncRecipe.DegreesBefore) THEN
				RecipeStatus := 'Distance must equal the sum of degrees before and after';
				CreateAccepted := FALSE;
			ELSE
				RecipeStatus := 'Valid entry';
				CreateAccepted := TRUE;
			END_IF
			
			IF CreateNewRecipe THEN
				MpRecipeUIConnect.New.Create := TRUE;
				NewListBoxEntry := '{"value":"';
				Piece1 := '","text":"';
				Piece2 := '"}';
				NewFileName := MpRecipeUIConnect.New.FileName;
				brsstrcat(ADR(NewListBoxEntry),ADR(NewFileName));
				brsstrcat(ADR(NewListBoxEntry),ADR(Piece1));
				brsstrcat(ADR(NewListBoxEntry),ADR(NewFileName));
				brsstrcat(ADR(NewListBoxEntry),ADR(Piece2));
				
				brsstrcpy(ADR(RecipeList[MpRecipeUIConnect.Recipe.List.MaxSelection + 1]),ADR(NewListBoxEntry))	;
				RecipeCreateStepNew := REFRESH;
			END_IF
					
			
		REFRESH:	// refresh the recipe list to make sure all the recipes are shown
			
			RecipeCreateStepNew := DONE_1;
			MpRecipeUIConnect.New.Create := FALSE;
			MpRecipeUIConnect.Recipe.Refresh := TRUE;
			
		DONE_1:	// if creation was successful or the creation was cancelled, the creation of the recipes is done
		
			IF(MpRecipeUIConnect.Status = mpRECIPE_UI_STATUS_IDLE)THEN
				MpRecipeRegPar_0.PVName := ADR('::MachineControl.Par.SyncRecipe');
				RecipeCreateStepNew := IDLE;
				MpRecipeUIConnect.Recipe.Refresh := FALSE;
			END_IF;
		
	END_CASE;
	
END_ACTION
