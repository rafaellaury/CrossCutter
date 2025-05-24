(*local recipe state machine type*)

TYPE
	RecipeCreateSteps : 
		( (*Steps for creating recipes*)
		IDLE := 0, (*Idle State *)
		CREATE_1 := 1, (*Create first recipe*)
		DONE_1 := 2, (*First recipe done*)
		REFRESH := 3, (*Reload the recipe object*)
		LOAD := 4, (*Load the recipe*)
		ALL_DONE := 5 (*All recipes were created*)
		);
END_TYPE
