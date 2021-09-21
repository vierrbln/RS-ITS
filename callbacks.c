#include "RunSequence.h"

void CVICALLBACK LoadAndRunSeq (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	
	giShowFileSelectPopup = 1;
	sprintf (cRunSequenceBatchFile, "");
	LoadSequenceAndStartExecution();   
}

void CVICALLBACK ResumeExecution (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	ERRORINFO errorInfo;
    hCheckResult(TS_ExecutionResume (gCurrentExecution, &errorInfo), "-->   TS_ExecutionResume", errorInfo);
}


void CVICALLBACK TerminateExecution (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	ERRORINFO errorInfo;
    hCheckResult(TS_ExecutionTerminate (gCurrentExecution, &errorInfo), "-->   TS_ExecutionTerminate", errorInfo);
}

void CVICALLBACK ABORTEXECUTION (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	ERRORINFO errorInfo;
    hCheckResult(TS_ExecutionAbort (gCurrentExecution, &errorInfo), "-->   TS_ExecutionAbort", errorInfo);
}

void CVICALLBACK QUIT (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	QuitUserInterface(0);
}

void CVICALLBACK OpenHelp (int menuBar, int menuItem, void *callbackData,
		int panel)
{
  char cPath[300];
  int iError;
  sprintf(cPath, "%s\\resources\\RS-ITS.chm", gcProjectDir);
  iError = ShowHtmlHelp(cPath, HH_DISPLAY_TOPIC, 0);
  if (iError)
  {
    MessagePopup("RS-ITS",
      "Cannot open help file of this tool. Check install folder!");
  }

}

void CVICALLBACK ShowSplashPanel (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	char cTextMessage[1024];
	char *cVersionString;
	TS_EngineGetVersionString (gEngine, NULL, &cVersionString);
	sprintf(cTextMessage, "RS-ITS RunSequence - Version: %s , NI Teststand - Version: %s", VERSION, cVersionString);
    CA_FreeMemory(cVersionString);
	SetCtrlVal(gSplash, SPLASH_SELECTEDSEQ, cTextMessage);
    DisplayPanel(gSplash);
	
}

int CVICALLBACK PictureCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			HidePanel(gSplash);
			break;
	}	
	return 0;
}


int CVICALLBACK SlpashCallBack (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel(gSplash);  
			break;
 	}
	return 0;
}



int CVICALLBACK MainPanelCallBack (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
	}
	return 0;
}


int CVICALLBACK SequenceListCallBack (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int iFixtureListItem;
	char cFixtureCode[512];
	char cProductName[512];
	char cSequencePath[512];
	char cSeqDrive[512];
	char cSeqDirectory[512];
	char cSeqFileName[512];
	char cTempBuffer[512];
	char cMsg[1024];
	
	switch (event)
	{
		case EVENT_GOT_FOCUS:
			 GetCtrlVal(gPanel, RS_SEQUENCES, &iFixtureListItem);
			 sprintf(cFixtureCode, "");
			 GetFixtureData (iFixtureListItem, cFixtureCode, cProductName, cSequencePath); 
			 SplitPath(cSequencePath, cSeqDrive, cSeqDirectory, cSeqFileName);
			 if (strlen(cSeqDirectory) > 70) 
			 {
				 CopyString (cTempBuffer, 0, cSeqDirectory, 0, 70); 
				 sprintf(cMsg, "FixCode %s - %s%s ... %s", cFixtureCode, cSeqDrive, cTempBuffer, cSeqFileName); 
	     
			 } else {
			     sprintf(cMsg, "FixCode %s - %s", cFixtureCode, cSequencePath); 
		     }	    
			 SetCtrlVal (gPanel, RS_SELECTEDSEQ, cMsg);
			break;
		case EVENT_VAL_CHANGED:
			 GetCtrlVal(gPanel, RS_SEQUENCES, &iFixtureListItem);
			 sprintf(cFixtureCode, "");
			 GetFixtureData (iFixtureListItem, cFixtureCode, cProductName, cSequencePath); 
			 SplitPath(cSequencePath, cSeqDrive, cSeqDirectory, cSeqFileName);
			 if (strlen(cSeqDirectory) > 70) 
			 {
				 CopyString (cTempBuffer, 0, cSeqDirectory, 0, 70); 
				 sprintf(cMsg, "FixCode %s - %s%s ... %s", cFixtureCode, cSeqDrive, cTempBuffer, cSeqFileName); 
	     
			 } else {
			     sprintf(cMsg, "FixCode %s - %s", cFixtureCode, cSequencePath); 
		     }	    
			 SetCtrlVal (gPanel, RS_SELECTEDSEQ, cMsg);
			break;
		case EVENT_RIGHT_CLICK:
			 GetCtrlVal(gPanel, RS_SEQUENCES, &iFixtureListItem);
			 sprintf(cFixtureCode, "");
			 GetFixtureData (iFixtureListItem, cFixtureCode, cProductName, cSequencePath);
			 sprintf(cMsg, "Your actual selected list entry have the following parameters.\n\nProduct name: %s\nFixture code: %s\nSequence: %s", cProductName, cFixtureCode,
				 cSequencePath);
			 MessagePopup("RS-ITS RunSequence", cMsg);
			 break;
			 
	}
	return 0;
}

int CVICALLBACK FixtureCodeCheck (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int iValue, iButton;
	char cFixtureCode[512];
	char cProductName[512];
	char cResponse[1024];
	char cMsg[1024];
	
	CAObjHandle StationGlobals;
	ERRORINFO errorInfo;
  
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
			GetCtrlVal (gPanel, RS_LED, &iValue);
			SetCtrlVal (gPanel, RS_LED, !iValue);
			sprintf(cFixtureCode, "");
			gfc_GetFixtureCode (0, cFixtureCode, &gsErrorOccurred, &glErrorCode, gcErrorMessage);
			if (strcmp(cFixtureCode, gcNoFixtureCode))
			{
				giFixtureCheckCounter++;
				if (giFixtureSureCounter > giFixtureCheckCounter)
				{
					if (giFixtureCheckCounter >= 1) sprintf(giLastFixtureCode, cFixtureCode); 
						 sprintf(cMsg, "Get this fixture code \"%s\" for %d times.", cFixtureCode, giFixtureCheckCounter); 
				 		 SetCtrlVal (gPanel, RS_SELECTEDSEQ, cMsg);
					return 0;
				} else 
				{
					if (strcmp(giLastFixtureCode,cFixtureCode))
					{	
						giFixtureCheckCounter = 0;
						sprintf(giLastFixtureCode, "");	
						return 0;
					}
				}
				SetCtrlAttribute (gPanel, RS_FIXTURECODE, ATTR_ENABLED, FALSE);
				GetFixtureData (-1, cFixtureCode, cProductName, gSeqFilePath);
				if (strlen(gSeqFilePath))
				{
					giShowFileSelectPopup = 0;
					gfc_Cleanup (0, &gsErrorOccurred, &glErrorCode, gcErrorMessage);
					hCheckResult(TS_EngineGetGlobals (gEngine, &errorInfo, &StationGlobals)
						, "-->   TS_EngineGetGlobals", errorInfo); 
	  					
					hCheckResult(TS_PropertySetValString (StationGlobals, &errorInfo, 
						"TSVP.ActualFixtureCode", TS_PropOption_InsertIfMissing, cFixtureCode)
						, "-->   TS_PropertySetValString", errorInfo); 
	  
					LoadSequenceAndStartExecution();
					
				} else 
				{
					sprintf(cMsg, "No sequence was found for the fixture code: \"%s\". Please select an option.", cFixtureCode); 
					iButton = GenericMessagePopup ("RS-ITS RunSequence", cMsg, "Fixture was removed. Continue.", "Disable fixture check",
												   "Select a sequence manually", 0, 0, 0, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN1,
												   VAL_GENERIC_POPUP_BTN2);
					if (iButton == 1)
					{
						SetCtrlAttribute (gPanel, RS_FIXTURECODE, ATTR_ENABLED, TRUE);
						break;
					} else if (iButton == 2)
					{
						giFixtureCheckIsActive = 0;
						StartFixtureCheck(); 
						SetCtrlVal(gPanel, RS_ENABLEFIXCHECK, FALSE); 
					} else 
					{
						giShowFileSelectPopup = 1;
						giFixtureCheckIsActive = 0;
						StartFixtureCheck(); 
						SetCtrlVal(gPanel, RS_ENABLEFIXCHECK, FALSE); 
						gfc_Cleanup (0, &gsErrorOccurred, &glErrorCode, gcErrorMessage);  
						LoadSequenceAndStartExecution();
					}
				}
			} else
			{
				giFixtureCheckCounter = 0;
				sprintf(giLastFixtureCode, ""); 
				SetCtrlVal (gPanel, RS_SELECTEDSEQ, "Fixture check enabled - Please mount fixture!");
						
			}
			break;
	}
	return 0;
}

int CVICALLBACK EnableFixtureCheck (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(gPanel, RS_ENABLEFIXCHECK, &giFixtureCheckIsActive);
			StartFixtureCheck();
			break;
	}
	return 0;
	
}

int CVICALLBACK RunSelectedSequence (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int iFixtureListItem;
	char cFixtureCode[512];
	char cProductName[512];
	
	switch (event)
	{
		case EVENT_COMMIT:
			sprintf (cRunSequenceBatchFile, "");
			GetCtrlVal(gPanel, RS_SEQUENCES, &iFixtureListItem);
			GetFixtureData (iFixtureListItem, cFixtureCode, cProductName, gSeqFilePath); 
			giShowFileSelectPopup = 0;
			LoadSequenceAndStartExecution();
   			break;
	}
	return 0;
}

HRESULT CVICALLBACK AppMgr_RTE (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              TSUIObj_Execution  exec,
                              TSUIObj_Thread  initiatingThread,
                              VBOOL *showDialog,
                              VBOOL *breakExecution)

{
	HRESULT error = 0;
	if (giTraceFlag) iTraceText ("Call function AppMgr_RTE");  
  
	SetPanelAttribute(gPanel, ATTR_FLOATING, VAL_FLOAT_ALWAYS);
	
	giRTEOccurred = 1;
	
	SetCtrlAttribute (gPanel, RS_LED, ATTR_ON_COLOR, VAL_RED);
	SetCtrlAttribute (gPanel, RS_TIMER_RTE_CHECK, ATTR_ENABLED, TRUE);
	
	 
	
	return error;
	
	
}

HRESULT CVICALLBACK AppMgr_EndExec (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              TSUIObj_Execution  exec)

{
	HRESULT error = 0;
	ERRORINFO errorInfo;
	CAObjHandle EndExecutionId;
	VBOOL bRemovedFromCache;
	
  VBOOL bIsRunning;
  int iStartSequence;
  int iCanRunSeq;
  int iConfirm;
  char *cLoginName;
  char cTextMessage[1024];
  
  short sErrorOccurred;
  short sLicenseOK;
  long lErrorCode;
  char cErrorMessage2[1024];
  char cSystemID[1024];
  
  
  VBOOL bTracing;
  VBOOL bResults;

  
	int iFixtureRemoved;
	if (giTraceFlag) iTraceText ("Call function AppMgr_EndExec");  
   
    if(hCheckResult(TS_ExecutionGetProperty(exec, &errorInfo,
      TS_ExecutionId, CAVT_LONG, &EndExecutionId), "-->   TS_ExecutionGetProperty", errorInfo)) goto GOTO_OUT_ERROR_TAG;
	
	if(EndExecutionId == 1)
	{
				SetCtrlVal (gPanel, RS_SELECTEDSEQ, "Login complete");
				TS_EngineGetCurrentUser (gEngine, NULL, &gUser);
				if (!gUser) 
				{
					QuitUserInterface(0);
					return 0;
				}
				TS_UserGetLoginName (gUser, NULL, &cLoginName);
				sprintf(cTextMessage, "RS-ITS RunSequence - Current User: %s", cLoginName);
				SetPanelAttribute (gPanel, ATTR_TITLE, cTextMessage);
				ProcessDrawEvents();
				  if(hCheckResult(TS_EngineGetStationOptions (gEngine, &errorInfo, &gStationOptions), "-->   TS_EngineGetStationOptions", errorInfo)) 
								  
					  goto GOTO_OUT_ERROR_TAG;  

				  if(hCheckResult(TS_StationOptionsSetUIMessageMinDelay (gStationOptions, &errorInfo, MIN_UIMSG_DELAY), "-->   TS_StationOptionsSetUIMessageMin//Delay", errorInfo)) 
					  goto GOTO_OUT_ERROR_TAG;  

				  if(hCheckResult(TS_StationOptionsGetTracingEnabled (gStationOptions, &errorInfo, &bTracing), "-->   TS_StationOptionsSetTracingEnabled", errorInfo)) 
					  goto GOTO_OUT_ERROR_TAG;  

				  if(hCheckResult(TS_StationOptionsGetDisableResults (gStationOptions, &errorInfo, &bResults), "-->   TS_StationOptionsSetDisableResults", errorInfo))
					  goto GOTO_OUT_ERROR_TAG; 
				  
				  if(bTracing || !bResults)
				  {
					  sprintf(cTextMessage, "Teststand options can impact performance by decreasing execution speed.\n\n");
					  if (bTracing) strcat(cTextMessage, "Tracing is enabled.\n");
					  if (!bResults) strcat(cTextMessage, "Record results is enabled.\n");
					  if (!bResults) strcat(cTextMessage, "\nDisable these options to improve perfomance?");
					  iConfirm = ConfirmPopup ("SW-ITS RunSequence", cTextMessage);
					  if (iConfirm)
					  {
						  if(hCheckResult(TS_StationOptionsSetTracingEnabled (gStationOptions, &errorInfo, VFALSE), "-->   TS_StationOptionsSetTracingEnabled", errorInfo)) 
							  goto GOTO_OUT_ERROR_TAG;  

						  if(hCheckResult(TS_StationOptionsSetDisableResults (gStationOptions, &errorInfo, VTRUE), "-->   TS_StationOptionsSetDisableResults", errorInfo))
							  goto GOTO_OUT_ERROR_TAG;   
					  }
				  }
				  
				  if(hCheckResult(TS_StationOptionsSetRTEOption (gStationOptions, &errorInfo, TS_RTEOption_ShowDialog), "-->   TS_StationOptionsSetRTEOption", errorInfo))
  				  	  goto GOTO_OUT_ERROR_TAG; 
				  
				if (!giAutoStartFixtureCheck)
				{
					if (giShowFileSelectPopup) {
						LoadSequenceAndStartExecution();   
					} else {
						if (giShowLoadAndRunMenu) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_LOADANDRUN, ATTR_DIMMED, FALSE);
						if (pFirst && giEnableFixtureCheck && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_ENABLEFCC, ATTR_DIMMED, FALSE);
						if (pFirst && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_RUNSELECTED, ATTR_DIMMED, FALSE);
						SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_TERMINATE, ATTR_DIMMED, TRUE);
						SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_ABORT, ATTR_DIMMED, TRUE);
						SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_RESUME, ATTR_DIMMED, TRUE);
						if (pFirst && giEnableFixtureCheck && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_ENABLEFCC, ATTR_DIMMED, FALSE);
						if (pFirst && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_RUNSELECTED, ATTR_DIMMED, FALSE);
						SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_QUIT, ATTR_DIMMED, FALSE);
						if(pFirst && giEnableFixtureCheck) SetCtrlAttribute (gPanel, RS_ENABLEFIXCHECK, ATTR_DIMMED, FALSE);
						if(pFirst) SetCtrlAttribute (gPanel, RS_RUNSELECTED, ATTR_DIMMED, FALSE);
						SetPanelAttribute (gPanel, ATTR_CLOSE_ITEM_VISIBLE, TRUE);
						if(pFirst) SetCtrlAttribute (gPanel, RS_SEQUENCES, ATTR_DIMMED, FALSE);
						ProcessDrawEvents();
						//Delay(0.5);
					}
				} else
				{
					giFixtureCheckIsActive = TRUE;
					StartFixtureCheck();
				}
				giSetupComplete++;
	}
 
	if ((gCurrentExecutionId == EndExecutionId) && !giRunSequenceFileUnload)
    {
		giRunSequenceFileUnload = 1;
		iCheckError(ExecuteSequence("SequenceFileUnload", VFALSE), "SequenceFileUnload");
		SetCtrlVal (gPanel, RS_SELECTEDSEQ, "Running SequenceFileUnload Callback");					
	} else if  ((gCurrentExecutionId == EndExecutionId) && giRunSequenceFileUnload)
			{
				SetCtrlVal (gPanel, RS_LED, 0);
				//Delay(0.250);
				hCheckResult(TS_EngineReleaseSeqFileEx (gEngine, &errorInfo, gCurrentSequenceFile, 0x2, &bRemovedFromCache), "-->   TS_EngineReleaseSeqFileEx", errorInfo);
				if (giQuitAfterExecution) QuitUserInterface(0);
				SetCtrlVal (gPanel, RS_SELECTEDSEQ, "Execution ended and sequence released!");  
				if (giShowLoadAndRunMenu) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_LOADANDRUN, ATTR_DIMMED, FALSE);
				SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_TERMINATE, ATTR_DIMMED, TRUE);
				SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_ABORT, ATTR_DIMMED, TRUE);
				SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_RESUME, ATTR_DIMMED, TRUE);
				if(pFirst) SetCtrlAttribute (gPanel, RS_SEQUENCES, ATTR_DIMMED, FALSE);
				if(pFirst && giEnableFixtureCheck) SetCtrlAttribute (gPanel, RS_ENABLEFIXCHECK, ATTR_DIMMED, FALSE);
				if(pFirst) SetCtrlAttribute (gPanel, RS_RUNSELECTED, ATTR_DIMMED, FALSE);
				if (pFirst && giEnableFixtureCheck && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_ENABLEFCC, ATTR_DIMMED, FALSE);
				if (pFirst && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_RUNSELECTED, ATTR_DIMMED, FALSE);
				SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_QUIT, ATTR_DIMMED, FALSE);
				SetPanelAttribute (gPanel, ATTR_CLOSE_ITEM_VISIBLE, TRUE);
				if (giFixtureCheckIsActive) 
				{
					iFixtureRemoved = GenericMessagePopup ("RS-ITS RunSequence", "Please remove the fixture and select an option!", "Fixture removed", "Exit Fixture Code Check",
										 "Quit Program", NULL, 0, 0, VAL_GENERIC_POPUP_BTN1, VAL_GENERIC_POPUP_BTN1,
										 VAL_GENERIC_POPUP_BTN3);
					//iFixtureRemoved = ConfirmPopup("RS-ITS RunSequence", "Please remove the fixture and press \"Yes\". Press \"No\" to deactive the\nfixture code check or quit the program!");
					if (iFixtureRemoved == 1) 
					{
						SetActiveCtrl(gPanel, RS_ENABLEFIXCHECK);
						SetCtrlVal (gPanel, RS_SELECTEDSEQ, "Fixture check enabled - Please mount fixture!");
						gfc_Setup (0, &gsErrorOccurred, &glErrorCode, gcErrorMessage);
					    SetCtrlAttribute (gPanel, RS_FIXTURECODE, ATTR_ENABLED, TRUE);
						giFixtureCheckCounter = 0;
						sprintf(giLastFixtureCode, ""); 
				
				    } else if (iFixtureRemoved == 2)
					{
						if (!giAutoStartFixtureCheck) 
						{
							SetCtrlVal(gPanel, RS_ENABLEFIXCHECK, FALSE);
							giFixtureCheckIsActive = FALSE;
							StartFixtureCheck();
						} else
						{
							QuitUserInterface(0); 
						}
					} else if (iFixtureRemoved == 3) 
					{
						QuitUserInterface(0);
					}
				}
			}
	return 0;
	
	
GOTO_OUT_ERROR_TAG:
	QuitUserInterface(0);
	return error;        
	
}

HRESULT CVICALLBACK AppMgr_UIMsg (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              TSUIObj_UIMessage  uiMsg,
                              VBOOL *cancel)
{
	HRESULT error = 0;
	ERRORINFO errorInfo;
	enum TSEnum_UIMessageCodes Value;
	if (giTraceFlag) iTraceText ("Call function AppMgr_UIMsg");  
   
	TS_UIMessageGetEvent (uiMsg, &errorInfo, &Value); 
	switch (Value)
	{
		case TS_UIMsg_ResumeFromBreak:
			{
			 if (giRTEOccurred == 1)
			 {
				SetPanelAttribute (gPanel, ATTR_FLOATING, VAL_FLOAT_NEVER);
	            giRTEOccurred = 0;
				SetCtrlAttribute (gPanel, RS_LED, ATTR_ON_COLOR, VAL_GREEN);
			 }
			}
			break ;
		
		default: return 0;
		
	}
	return 0;
}


HRESULT CVICALLBACK ExecMgr_RunStateChanged (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              enum TSUIEnum_ExecutionRunStates newRunState)

{
	HRESULT error = 0;
	if (giTraceFlag) iTraceText ("Call function ExecMgr_RunStateChanged");  
    if (newRunState == TSUIConst_ExecRunState_Running)
	 {
		SetPanelAttribute (gPanel, ATTR_FLOATING, VAL_FLOAT_NEVER);
        giRTEOccurred = 0;
		SetCtrlAttribute (gPanel, RS_LED, ATTR_ON_COLOR, VAL_GREEN);
	 }
	
	return error;
	
	
}



void CVICALLBACK RunSelectedSeq_menu (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	int iFixtureListItem;
	char cFixtureCode[512];
	char cProductName[512];
	
	if (giTraceFlag) iTraceText ("Call function RunSelectedSeq_menu");
	GetCtrlVal(gPanel, RS_SEQUENCES, &iFixtureListItem);
	GetFixtureData (iFixtureListItem, cFixtureCode, cProductName, gSeqFilePath); 
	giShowFileSelectPopup = 0;
	LoadSequenceAndStartExecution();
   	return;
}

void CVICALLBACK Enable_FCC_menu (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	if (giTraceFlag) iTraceText ("Call function Enable_FCC_menu");
	GetCtrlVal(gPanel, RS_ENABLEFIXCHECK, &giFixtureCheckIsActive);
	giFixtureCheckIsActive = !giFixtureCheckIsActive;
	SetCtrlVal(gPanel, RS_ENABLEFIXCHECK, giFixtureCheckIsActive);
	StartFixtureCheck();
				
}

int CVICALLBACK CheckRunState (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
ERRORINFO errorInfo;
  	
enum TSUIEnum_ExecutionRunStates val;	
	if (giTraceFlag) iTraceText ("Call function CheckRunState");  
   switch (event)
	{
		case EVENT_TIMER_TICK:
			if(hCheckResult(TSUI_ApplicationMgrGetRunState (ApplicationMgr, &errorInfo, gCurrentExecution, &val)
				, "-->   TSUI_ApplicationMgrGetRunState", errorInfo)) goto GOTO_OUT_ERROR_TAG;  
			if (val == TSUIConst_ExecRunState_Running) 
			{
				SetCtrlAttribute (gPanel, RS_TIMER_RTE_CHECK, ATTR_ENABLED, FALSE);
				SetPanelAttribute (gPanel, ATTR_FLOATING, VAL_FLOAT_NEVER);
		        giRTEOccurred = 0;
				SetCtrlAttribute (gPanel, RS_LED, ATTR_ON_COLOR, VAL_GREEN);
			}
	
			break;
	}   
	return 0;
	
GOTO_OUT_ERROR_TAG:
	return -1;
	
}


