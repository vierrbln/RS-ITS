#include "RunSequence.h"

int main(int argc, char *argv[])
{

  ERRORINFO errorInfo;
  int iError = 0;
  int hWnd;
  int iSuccess;
  int iCanRunSeq;
  int iStartSequence;
  int idx;
  int iNumberOfSections;
  char *sequenceToExecute = 0;
  char cTextMessage[1024]; 
  char *cErrorMessage;
  
  FixSlowMSVCHeap();  
  
  if (InitCVIRTE (0, argv, 0) == 0)
      return -1; /* out of memory */
  
  
  
  sprintf(gSeqFilePath,"");
  giShowFileSelectPopup = 0; 
  giQuitAfterExecution = 0; 
  giShowLoadAndRunMenu = 1; 
  giAutoStartFixtureCheck = 0;
  giEnableFixtureCheck = 0;
  giFixtureCheckIsActive = 0;
  sprintf(gcNoFixtureCode, ""); 
  sprintf(gPhysicalIniFileName, "");
  sprintf(cRunGlobalBatchFile, "");
  iWaitForGlobalBatchFile = TRUE;
  dDelayAfterGlobalBatchFile = 0.0;

  giFixtureSureCounter = 1; 
  giTraceFlag = 0; 
  giShellMode = 0;
  giSetupComplete = 0;
  
  giSpashCounter = 0;
  
  GetProjectDir (gcProjectDir);
  sprintf(gcPublicDocDir, "C:\\Users\\Public\\Documents\\SW-ITS\\");    
	  

  //Load waiting panel
  gSplash = LoadPanel(0, "Splash.uir", SPLASH);
  if (iCheckUIError(gSplash, "LoadPanel, SPLASH") <= 0) goto GOTO_OUT_ERROR_TAG ;
  
  sprintf(cTextMessage, " RS-ITS RunSequence - Version %s - Load INI-File, please wait ", VERSION);
  SetCtrlVal(gSplash, SPLASH_SELECTEDSEQ, cTextMessage);
  SetCtrlAttribute(gSplash, SPLASH_SELECTEDSEQ, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
  SetCtrlAttribute(gSplash, SPLASH_LABEL, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
  SetCtrlAttribute(gSplash, SPLASH_PRODUCT, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);
  DisplayPanel(gSplash);
  
  /* possible options
  
  /shellmode -> Change menü from Quit to Quit and logout
  /trace     -> Activate the tracing
  /file:     -> file runs immediately
  
  */
  
  iError =  CheckCommandline (argc, argv);
  if (iError) goto GOTO_OUT_ERROR_TAG;
  
  if (giTraceFlag && !pTraceFile) if(iOpenTraceFile()) goto GOTO_OUT_ERROR_TAG ;
  if (giTraceFlag && !pTraceFile) iTraceText ("Program started");
  if (giTraceFlag && !pTraceFile) iTraceText ("Tracing enabled, Flag is true in command line");  
      
  
 
  //trace all program arguments
  if (giTraceFlag)
  {
       iTraceText ("Program arguments:");
        for (idx = 0; idx < argc; idx++)
        {
            sprintf(cTextMessage, "%i -> %s", idx, argv[idx]);
            iTraceText (cTextMessage);      
        }
  }	
  			
  //Delay (0.1);
  
  iError =  LoadIniFile ();  
  if (iError) goto GOTO_OUT_ERROR_TAG;
  
  
  //Load waiting panel
  gPanel = LoadPanel(0, "rs.uir", RS);
  if (iCheckUIError(gPanel, "LoadPanel") <= 0) goto GOTO_OUT_ERROR_TAG ;
  
  gMenuBar = LoadMenuBar (gPanel, "rs.uir", MENUBAR);
  if (iCheckUIError(gMenuBar, "LoadMenuBar") <= 0) goto GOTO_OUT_ERROR_TAG ;
  
  if (giAutoStartFixtureCheck)
  {
	  SetCtrlAttribute (gPanel, RS_SEQUENCES, ATTR_VISIBLE, FALSE);
	  SetCtrlAttribute (gPanel, RS_ENABLEFIXCHECK, ATTR_VISIBLE, FALSE);
	  SetCtrlAttribute (gPanel, RS_RUNSELECTED, ATTR_VISIBLE, FALSE);
  }	
  
  if (giShellMode)
  {
	  SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_QUIT, ATTR_ITEM_NAME,
						   "Quit and logoff");
  }
  
  
  UpDateFixtureCodeList();
  
  //Run batch file if specified
  if (giTraceFlag) iTraceText ("Run global batch file");  
  sprintf(cTextMessage, " RS-ITS RunSequence - Version %s - running global batch file, please wait ", VERSION);
  SetCtrlVal(gSplash, SPLASH_SELECTEDSEQ, cTextMessage);
  ProcessDrawEvents();
  if (strlen(cRunGlobalBatchFile))
  {
	if (iWaitForGlobalBatchFile)
	{
	    system(cRunGlobalBatchFile);	
	} else {
		LaunchExecutable(cRunGlobalBatchFile);
		Delay(dDelayAfterGlobalBatchFile);
	}
  }
  
  
  
  //Load the TestStand Engine
  if (giTraceFlag) iTraceText ("Initialize Teststand Engine");  
  sprintf(cTextMessage, " RS-ITS RunSequence - Version %s - Initiate engine, please wait ", VERSION);
  SetCtrlVal(gSplash, SPLASH_SELECTEDSEQ, cTextMessage);
  ProcessDrawEvents();
  
  if (iCheckError(InitEngine(), "InitEngine")) goto GOTO_OUT_ERROR_TAG ;
  
  if (gSplash) HidePanel(gSplash);
  
  DisplayPanel(gPanel);
  
  RunUserInterface();
  
//If an error occurred, release all resources.
GOTO_OUT_ERROR_TAG:
  
  if (giTraceFlag) iTraceText ("Start free all resources");
  // free all resources
  //if (sequenceToExecute) CA_FreeMemory(sequenceToExecute);
  
  hCheckResult(TSUI_ApplicationMgrGetEngine (ApplicationMgr, &errorInfo, &gEngine), "-->   TSUI_ApplicationMgrGetEngine", errorInfo);
  
  hCheckResult(TS_EngineCommitGlobalsToDisk (gEngine, &errorInfo, VTRUE), "-->   TS_EngineCommitGlobalsToDisk", errorInfo);
  
  hCheckResult(TS_EngineShutDown (gEngine, &errorInfo, VTRUE), "-->   TSUI_ApplicationMgrGetEngine", errorInfo);
  
  if (ApplicationMgr) 
  {
	  hCheckResult(TSUI_ApplicationMgrShutdown (ApplicationMgr, &errorInfo, NULL), "-->   TSUI_ApplicationMgrShutdown", errorInfo); 
  }
  
  if (gReport) 
  {
	  hCheckResult(CA_DiscardObjHandle(gReport), "-->   CA_DiscardObjHandle - gReport", errorInfo);
  }
  if (gCurrentExecution) 
  {
	  hCheckResult(CA_DiscardObjHandle(gCurrentExecution), "-->   CA_DiscardObjHandle - gCurrentExecution", errorInfo);
  }
  if (gLoginLogoutExecution) 
  {
	  hCheckResult(CA_DiscardObjHandle(gLoginLogoutExecution), "-->   CA_DiscardObjHandle - gLoginLogoutExecution", errorInfo);
  }
  				   
  if (gCurrentSequenceFile) 
  {
	  hCheckResult(CA_DiscardObjHandle(gCurrentSequenceFile), "-->   CA_DiscardObjHandle - gCurrentSequenceFile", errorInfo);
  }
  if (gLoginLogoutArgs) 
  {
	  hCheckResult(CA_DiscardObjHandle(gLoginLogoutArgs), "-->   CA_DiscardObjHandle - gLoginLogoutArgs", errorInfo);
  }
  if (gStationOptions) 
  {
	  	hCheckResult(CA_DiscardObjHandle(gStationOptions), "-->   CA_DiscardObjHandle - gStationOptions", errorInfo);
  }
  if (gEngine) 
  {
	  	hCheckResult(CA_DiscardObjHandle(gEngine), "-->   CA_DiscardObjHandle - gEngine", errorInfo); 
  }
  
  if (gPanel) DiscardPanel(gPanel);
  if (gSplash) DiscardPanel(gSplash);

  if (giTraceFlag) iTraceText ("Free all resources done");
  
  //close trace file
  if (giTraceFlag) iTraceText ("Tracing stopped");  
  if (giTraceFlag) iTraceText ("Programm ended");
  if (giTraceFlag) iCloseTraceFile();
  
  if (giShellMode)
  {
	  ExitWindowsEx (EWX_LOGOFF, 0x00040000);
  }
  
  CloseCVIRTE();

  return iError;
  
}

// called when the Open Sequence item is selected from the File menu
int OpenSequence(int panel, int *iFileSelected)
{
  ERRORINFO errorInfo;
  int iError = 0;
  int result;
  VBOOL bRemovedFromCache;
  VBOOL bSuccess; 
  enum TSEnum_ModuleLoadOptions bLoadOption;
  enum TSEnum_ModuleUnloadOptions bUnLoadOption;
  
  char cMsg[1024];

  if (gExiting)
    return 0;

  if (giShowFileSelectPopup)
  {
    if (giTraceFlag) iTraceText ("Show file select popup window"); 
     // display a file dialog
     result = FileSelectPopup (gcDefaultDirectory, "*.seq", "*.seq", 
         "Load Sequence File", VAL_LOAD_BUTTON, 0, 0, 1, 1, gSeqFilePath);
  } else 
  {
     result = VAL_EXISTING_FILE_SELECTED;
  }
  if (result == VAL_EXISTING_FILE_SELECTED)
  {
    
	SplitPath(gSeqFilePath, gSeqDrive, gSeqDirectory, gSeqFileName);
	 if (strlen(gSeqDirectory) > 70) 
	 {
		 CopyString (gTempBuffer, 0, gSeqDirectory, 0, 70); 
		 sprintf(cMsg, "Loading: %s%s ... %s", gSeqDrive, gTempBuffer, gSeqFileName); 
	     
	 } else {
	     sprintf(cMsg, "Loading: %s", gSeqFilePath); 
     }
	 
	 SetCtrlVal (gPanel, RS_SELECTEDSEQ, cMsg);
	 SetCtrlAttribute (gPanel, RS_LED, ATTR_ON_COLOR, VAL_YELLOW);
	 SetCtrlVal (gPanel, RS_LED, 1);
	 ProcessDrawEvents();
	   
	// unload the current seqeunce file if any
    // (this demo only keeps one file loaded at a time)
    if (gCurrentSequenceFile)
    {
      if(hCheckResult(TS_EngineReleaseSeqFileEx (gEngine, &errorInfo, gCurrentSequenceFile,
												 0, &bRemovedFromCache), "-->   TS_EngineReleaseSeqFile", errorInfo)) goto GOTO_OUT_ERROR_TAG;
      CA_DiscardObjHandle(gCurrentSequenceFile);
      gCurrentSequenceFile = 0;
    }

    // load the selected sequence file
	/*if(hCheckResult(TSUI_ApplicationMgrOpenSequenceFile (ApplicationMgr, &errorInfo, gSeqFilePath, &gCurrentSequenceFile)
		, "-->   TSUI_ApplicationMgrOpenSequenceFile", errorInfo)) goto GOTO_OUT_ERROR_TAG; 
	*/ 																		   //GetSeqFile_PreloadModules
    if(hCheckResult(TS_EngineGetSeqFileEx (gEngine, &errorInfo, gSeqFilePath, TS_GetSeqFile_PreloadModules,
										   TS_ConflictHandler_UseGlobalType, &gCurrentSequenceFile), "-->   TS_EngineGetSeqFile", errorInfo)) goto GOTO_OUT_ERROR_TAG;
     
	*iFileSelected = 1;
     if (giTraceFlag) iTraceText ("Sequence file path is:");
     if (giTraceFlag) iTraceText (gSeqFilePath);
	 
	 if(hCheckResult(TS_SeqFileSetModuleLoadOption (gCurrentSequenceFile, &errorInfo, TS_LoadOption_PreloadWhenOpened),
		 "-->   TS_SeqFileGetModuleLoadOption", errorInfo)) goto GOTO_OUT_ERROR_TAG; 
	 if(hCheckResult(TS_SeqFileSetModuleUnloadOption (gCurrentSequenceFile, &errorInfo, TS_UnloadOption_WithSequenceFile),
	 "-->   TS_SeqFileGetModuleUnloadOption", errorInfo)) goto GOTO_OUT_ERROR_TAG; 	
	 if(hCheckResult(TS_SeqFileLoadModules (gCurrentSequenceFile, &errorInfo, 0x0, CA_DEFAULT_VAL, &bSuccess),
		 "-->   TS_SeqFileLoadModules", errorInfo)) goto GOTO_OUT_ERROR_TAG;        
	 if(!bSuccess) MessagePopup("RS-ITS RunSequence", "Error 505");	 
	 
  } else {
      *iFileSelected = 0;
      if (giTraceFlag) iTraceText ("No sequence file was selected");
  }
  
  return 0; 
  
GOTO_OUT_ERROR_TAG: 
  
  return -1; 
  
}

///////////////////////////////////////////////////////////////////////////

int CheckSequence(int *iStartSequence)
{
    VBOOL vSequenceExists[6] = {FALSE};
    ERRORINFO errorInfo; 
    int iShowPopup;
    char cMessageText[1024];
    
    *iStartSequence = 0;
    giRunSequenceFileUnload = 1;
    
    if(hCheckResult(TS_SeqFileSequenceNameExists (gCurrentSequenceFile, &errorInfo, "MainSequence", &vSequenceExists[0]), 
        "-->   TS_SeqFileSequenceNameExists", errorInfo)) goto GOTO_OUT_ERROR_TAG;
    if(hCheckResult(TS_SeqFileSequenceNameExists (gCurrentSequenceFile, &errorInfo, "PreUUT", &vSequenceExists[1]), 
        "-->   TS_SeqFileSequenceNameExists", errorInfo)) goto GOTO_OUT_ERROR_TAG;
    if(hCheckResult(TS_SeqFileSequenceNameExists (gCurrentSequenceFile, &errorInfo, "PostUUT", &vSequenceExists[2]), 
        "-->   TS_SeqFileSequenceNameExists", errorInfo)) goto GOTO_OUT_ERROR_TAG;
    if(hCheckResult(TS_SeqFileSequenceNameExists (gCurrentSequenceFile, &errorInfo, "PreUUTLoop", &vSequenceExists[3]), 
        "-->   TS_SeqFileSequenceNameExists", errorInfo)) goto GOTO_OUT_ERROR_TAG;
    if(hCheckResult(TS_SeqFileSequenceNameExists (gCurrentSequenceFile, &errorInfo, "PostUUTLoop", &vSequenceExists[4]), 
        "-->   TS_SeqFileSequenceNameExists", errorInfo)) goto GOTO_OUT_ERROR_TAG;
    if(hCheckResult(TS_SeqFileSequenceNameExists (gCurrentSequenceFile, &errorInfo, "SequenceFileUnload", &vSequenceExists[5]), 
        "-->   TS_SeqFileSequenceNameExists", errorInfo)) goto GOTO_OUT_ERROR_TAG;
    
    if(!vSequenceExists[0])
    {
        MessagePopup("RS-ITS", "Error: The loaded sequence file did not contain the mainsequence!\n"
            "This sequence cannot executed");
        if (giTraceFlag) iTraceText ("MainSequence not found");
        if (giTraceFlag) iTraceText ("iStartSequence automatically false");
        return 0;
    }
    iShowPopup = 0;
    sprintf(cMessageText, "Warning! The following sequences dont exist in the loaded sequence file!\n\n");
    if (!vSequenceExists[1])
    {
        iShowPopup++;
        strcat(cMessageText, "PreUUT\n");
        if (giTraceFlag) iTraceText ("PreUUT not found"); 
    }
    if (!vSequenceExists[2])
    {
        iShowPopup++;
        strcat(cMessageText, "PostUUT\n");
        if (giTraceFlag) iTraceText ("PostUUT not found"); 
    }
    if (!vSequenceExists[3])
    {
        iShowPopup++;
        strcat(cMessageText, "PreUUTLoop\n");
        if (giTraceFlag) iTraceText ("PreUUTLoop not found"); 
    }
    if (!vSequenceExists[4])
    {
        iShowPopup++;
        strcat(cMessageText, "PostUUTLoop\n");
        if (giTraceFlag) iTraceText ("PostUUTLoop not found"); 
    }
    if (!vSequenceExists[5])
    {
        iShowPopup++;
        strcat(cMessageText, "SequenceFileUnload\n");
        if (giTraceFlag) iTraceText ("SequenceFileUnload not found"); 
        giRunSequenceFileUnload = 1;
    } else
	{
		giRunSequenceFileUnload = 0;
	}
    if(iShowPopup)
    {
        strcat(cMessageText, "\nThe file does not correspond to the SW-ITS standard.");
        strcat(cMessageText, "\nIt can come to mistakes in the execution.");
        strcat(cMessageText, "\n\nIf you would like to start the execution nevertheless.  ");
        if (giTraceFlag) iTraceText ("Show confirm popup");
        *iStartSequence = ConfirmPopup("RS-ITS", cMessageText);
    } else {
        *iStartSequence = 1;
        if (giTraceFlag) iTraceText ("All sequences found, iStartSequence automatically true");
    }
    
  return 0; 
  
GOTO_OUT_ERROR_TAG: 
  
  return -1; 
    
}

///////////////////////////////////////////////////////////////////////////

int ExecuteSequence(char *sequenceName, VBOOL useProcessModel)
{
  ERRORINFO errorInfo;
  int iError = 0;
  CAObjHandle modelSeqFile = 0;
  char *modelDesc = NULL;
  char cMsg[1024];

  if (gReport)
  {
    CA_DiscardObjHandle(gReport);
    gReport = 0;
  }
  
 
  // get the model sequence file if needed
  if (useProcessModel)
    if(hCheckResult(TS_SeqFileGetModelSeqFile(gCurrentSequenceFile, &errorInfo,
      &modelDesc, &modelSeqFile), "-->   TS_SeqFileGetModelSeqFile", errorInfo)) goto GOTO_OUT_ERROR_TAG;

  // run the specified sequence in the currently loaded file
  if (useProcessModel)
    if(hCheckResult(TS_EngineNewExecution (gEngine, &errorInfo, gCurrentSequenceFile,
										   sequenceName, modelSeqFile, VFALSE,
										   TS_ExecTypeMask_InitiallyHidden | TS_ExecTypeMask_TracingInitiallyOff,
										   CA_DEFAULT_VAL, CA_DEFAULT_VAL, CA_DEFAULT_VAL,
										   &gCurrentExecution), "-->   TS_EngineNewExecution useProcessModel", errorInfo)) goto GOTO_OUT_ERROR_TAG;
  
  if (!useProcessModel)
    if(hCheckResult(TS_EngineNewExecution (gEngine, &errorInfo, gCurrentSequenceFile,
										   sequenceName, 0, VFALSE,
										   TS_ExecTypeMask_InitiallyHidden | TS_ExecTypeMask_TracingInitiallyOff,
										   CA_DEFAULT_VAL, CA_DEFAULT_VAL, CA_DEFAULT_VAL,
										   &gCurrentExecution), "-->   TS_EngineNewExecution !useProcessModel", errorInfo)) goto GOTO_OUT_ERROR_TAG;
   
  //Delay (0.1);
  // remember the execution ID so we can tell when this execution terminates  (see HandleTEUIMessage())
  if(hCheckResult(TS_ExecutionGetProperty(gCurrentExecution, &errorInfo,
    TS_ExecutionId, CAVT_LONG, &gCurrentExecutionId), "-->   TS_ExecutionGetProperty", errorInfo)) goto GOTO_OUT_ERROR_TAG;
  //Delay (0.1);
  if(hCheckResult(TS_ExecutionSetTracingDisabled (gCurrentExecution, &errorInfo, VTRUE)
	  , "-->   TS_ExecutionSetTracingDisabled", errorInfo)) goto GOTO_OUT_ERROR_TAG;    
  if(hCheckResult(TS_ExecutionSetTimeResultsEnabled (gCurrentExecution, &errorInfo, VFALSE)
	  , "-->   TS_ExecutionSetTimeResultsEnabled", errorInfo)) goto GOTO_OUT_ERROR_TAG;    
  if(hCheckResult(TS_ExecutionSetStandardResultsEnabled (gCurrentExecution, &errorInfo, VFALSE)
	  , "-->   TS_ExecutionSetStandardResultsEnabled", errorInfo)) goto GOTO_OUT_ERROR_TAG;    
  if(hCheckResult(TS_ExecutionSetDisableResults (gCurrentExecution, &errorInfo, VTRUE)
	  , "-->   TS_ExecutionSetDisableResults", errorInfo)) goto GOTO_OUT_ERROR_TAG;
  
  
  //Delay (0.1); 
  if (strlen(gSeqDirectory) > 70) 
	 {
		 CopyString (gTempBuffer, 0, gSeqDirectory, 0, 70); 
		 sprintf(cMsg, "Running: %s%s ... %s", gSeqDrive, gTempBuffer, gSeqFileName); 
	     
	 } else {
	     sprintf(cMsg, "Running: %s", gSeqFilePath); 
     }	    
  SetCtrlAttribute (gPanel, RS_LED, ATTR_ON_COLOR, VAL_GREEN);
  SetCtrlVal (gPanel, RS_SELECTEDSEQ, cMsg);
  ProcessDrawEvents();	 
  
  
  return 0;
  
  GOTO_OUT_ERROR_TAG: 
  
  if (modelSeqFile)
    CA_DiscardObjHandle(modelSeqFile);
  if (modelDesc)
    CA_FreeMemory(modelDesc);		  
  
  return -1;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

int InitEngine(void)
{
  ERRORINFO errorInfo;
  BOOL errorOccurred = FALSE;
  int iError;
  VBOOL Value;
  
  short sErrorOccurred;
  short sLicenseOK;
  long lErrorCode;
  char cErrorMessage2[1024];
  char cSystemID[1024];
  

  //Get ObjectHandle
  iError = GetObjHandleFromActiveXCtrl (gPanel, RS_APPLICATIONMGR, &ApplicationMgr);
  //Delay(0.1);
  //Get Engine Handle
  if(hCheckResult(TSUI_ApplicationMgrGetEngine (ApplicationMgr, &errorInfo, &gEngine), "-->   TSUI_ApplicationMgrGetEngine", errorInfo)) 
	  goto GOTO_OUT_ERROR_TAG;
  //Delay(0.1);
  
  if(hCheckResult(TS_EngineSetOutputMessagesEnabled (gEngine, &errorInfo, VFALSE), "-->   TS_EngineSetOutputMessagesEnabled", errorInfo)) 
	  goto GOTO_OUT_ERROR_TAG;
  //Delay(0.1);
  // Old license check
  /*if (!DEBUG)
  {
	  //Check license
	  if (giTraceFlag) iTraceText ("Check license ...");
	  RESMGR_Setup (0, gPhysicalIniFileName, "", &sErrorOccurred, &lErrorCode, cErrorMessage2);
	  if (sCheckGTSLError(sErrorOccurred, lErrorCode, cErrorMessage2, "RESMGR_Setup")) goto GOTO_OUT_ERROR_TAG ;      
  	  //Delay(0.1);
	  RESMGR_Read_ROM (0, cSystemID, &sErrorOccurred, &lErrorCode, cErrorMessage2);
	  if (sCheckGTSLError(sErrorOccurred, lErrorCode, cErrorMessage2, "RESMGR_Read_ROM")) goto GOTO_OUT_ERROR_TAG ;       
	  //Delay(0.1);
	  RESMGR_Cleanup (0, &sErrorOccurred, &lErrorCode, cErrorMessage2);
	  if (sCheckGTSLError(sErrorOccurred, lErrorCode, cErrorMessage2, "RESMGR_Cleanup")) goto GOTO_OUT_ERROR_TAG ;    
	  
	  if (giTraceFlag) iTraceText ("System ID is");  
	  if (giTraceFlag) iTraceText (cSystemID);		   
  
	  sLicenseOK = (short) lc_CheckLicense ("SW-ITS", cSystemID);
  
	  if(!sLicenseOK) 
	  {
		  MessagePopup("RS-ITS RunSequence", "No valid license found or Resource Manager locked!\nIf any test programm running in NI Teststand, please close it\nand restart RunSequence again.");
		  goto GOTO_OUT_ERROR_TAG;
	  }		
					  
	  if (giTraceFlag) iTraceText ("Check license o.k.");	   
  } */ 
  //Delay(0.1);
  //Start Engine and Login Process
  if(hCheckResult(TSUI_ApplicationMgrStart (ApplicationMgr, &errorInfo), "-->   TSUI_ApplicationMgrStart", errorInfo)) 
	  goto GOTO_OUT_ERROR_TAG; 
  //Delay(0.1);
  if(hCheckResult(TSUI_ApplicationMgrGetIsStarted (ApplicationMgr, NULL, &Value), "-->   TSUI_ApplicationMgrGetIsStarted", errorInfo)) 
	  goto GOTO_OUT_ERROR_TAG; 
  while (!Value) 
  {
	  //Delay(0.1);
	  if(hCheckResult(TSUI_ApplicationMgrGetIsStarted (ApplicationMgr, NULL, &Value), "-->   TSUI_ApplicationMgrGetIsStarted", errorInfo)) 
	  	goto GOTO_OUT_ERROR_TAG;    
  }
  //Delay(0.1);
  if(hCheckResult(TSUI__ApplicationMgrEventsRegOnEndExecution (ApplicationMgr, AppMgr_EndExec, NULL, 1, NULL)
	  , "-->   TSUI__ApplicationMgrEventsRegOnEndExecution", errorInfo)) 
	  goto GOTO_OUT_ERROR_TAG; 
  
  //Delay(0.1);
  if(hCheckResult(TSUI__ApplicationMgrEventsRegOnBreakOnRunTimeError (ApplicationMgr, AppMgr_RTE, NULL, 1, NULL)
	  , "-->   TSUI__ApplicationMgrEventsRegOnBreakOnRunTimeError", errorInfo)) 
	  goto GOTO_OUT_ERROR_TAG; 
  
  if(hCheckResult(TSUI__ApplicationMgrEventsRegOnUIMessageEvent (ApplicationMgr, AppMgr_UIMsg, NULL, 1, NULL)
	  , "-->   TSUI__ApplicationMgrEventsRegOnUIMessageEvent", errorInfo)) 
	  goto GOTO_OUT_ERROR_TAG;    
  
  return 0; /* return TRUE if no errors occurred */

  
  GOTO_OUT_ERROR_TAG:

  return -1; /* return TRUE if no errors occurred */
}


int DisplayOLEError(int iError, ERRORINFO *errorInfo)
{
  if (iError < 0)
    CA_DisplayErrorInfo(0, "OLE GOTO_OUT_ERROR_TAG", iError, errorInfo);
  return iError;
}


HRESULT hCheckResult (HRESULT hResult, char *cFunctionName, ERRORINFO hErrorInfo)
{
    char cErrorMessage[1024];
    char cTextMessage[1024];
    
    typedef struct {
    	unsigned short wCode;                       // An error code describing the error.
   		SCODE sCode;                                // A return value describing the error.
    	char source[ERRORINFO_SRC_BUF_SIZE];        // Source of the exception.
    	char description[ERRORINFO_DESC_BUF_SIZE];  
		
		// Textual description of the error.
    	char helpFile[MAX_PATHNAME_LEN];            // Help file path.
    	unsigned long helpContext;                  // Help context ID.
    	int errorParamPos;                          // 1 based position of invalid parameter
	} ERRORINFO;

    
    if (hResult) 
    {
        CA_GetAutomationErrorString(hResult, cErrorMessage, 1024);
        sprintf(cTextMessage, "An error has occurred in function: %s\n\n"
            "Error code: %d\n"
            "CA automation error string: %s\n\n", cFunctionName, hResult, cErrorMessage);
        sprintf(cErrorMessage, "Teststand API error information:\n");
        strcat (cTextMessage, cErrorMessage);
        sprintf(cErrorMessage, "wCode: %d\n", hErrorInfo.wCode);
        strcat (cTextMessage, cErrorMessage);
        sprintf(cErrorMessage, "sCode: %d\n", hErrorInfo.sCode);
        strcat (cTextMessage, cErrorMessage);
        sprintf(cErrorMessage, "source: %s\n", hErrorInfo.source);
        strcat (cTextMessage, cErrorMessage);
        sprintf(cErrorMessage, "description: %s\n", hErrorInfo.description);
        strcat (cTextMessage, cErrorMessage);
        sprintf(cErrorMessage, "helpFile: %s\n", hErrorInfo.helpFile);
        strcat (cTextMessage, cErrorMessage);
        sprintf(cErrorMessage, "helpContext: %d\n", hErrorInfo.helpContext);
        strcat (cTextMessage, cErrorMessage);
        sprintf(cErrorMessage, "errorParamPos: %d\n", hErrorInfo.errorParamPos);
        strcat (cTextMessage, cErrorMessage);
        if(giTraceFlag) iTraceText (cTextMessage);
        MessagePopup("RS-ITS", cTextMessage);   
    } else 
    {
        if (giTraceFlag)
        {
            sprintf(cTextMessage, "Function %s finished with no errors", cFunctionName);
            iTraceText (cTextMessage);  
        }
    }
    ghResult = hResult;
    return hResult;
        
}

///////////////////////////////////////////////////////////////////////////

int iCheckError (int iError, char *cFunctionName)
{
    char cErrorMessage[1024];
    char cTextMessage[1024];
    
    if (iError) 
    {
        sprintf(cTextMessage, "An error has occurred in function: %s\n\n"
            "Error code: %d\n"
            "Error message: No error message\n", cFunctionName, iError);    
        if(giTraceFlag) iTraceText (cTextMessage);
        MessagePopup("RS-ITS", cTextMessage);   
    } else 
    {
        if (giTraceFlag)
        {
            sprintf(cTextMessage, "Function %s finished with no errors", cFunctionName);
            iTraceText (cTextMessage);  
        }
    }
    
    return iError;
        
}

///////////////////////////////////////////////////////////////////////////

int iCheckUIError (int iError, char *cFunctionName)
{
    char *cErrorMessage;
    char cTextMessage[1024];
    
    if (iError <= 0) 
    {
        cErrorMessage = GetUILErrorString (iError);
       sprintf(cTextMessage, "An error has occurred in function: %s\n\n"
            "Error code: %d\n"
            "Error message: No error message\n", cFunctionName, iError);    
        if(giTraceFlag) iTraceText (cTextMessage);
        MessagePopup("RS-ITS", cTextMessage);   
    } else 
    {
        if (giTraceFlag)
        {
            sprintf(cTextMessage, "Function %s finished with no errors", cFunctionName);
            iTraceText (cTextMessage);  
        }
    }
    
    return iError;
        
}

///////////////////////////////////////////////////////////////////////////

short sCheckGTSLError (short sErrorOccurred, long lErrorCode, 
    char *cErrorMessage, char *cFunctionName)
{
    char cTextMessage[1024];
    
    if (sErrorOccurred) 
    {
        sprintf(cTextMessage, "An error has occurred in function: %s\n\n"
            "Error code: %d\n"
            "Error message: %s\n", cFunctionName, lErrorCode, cErrorMessage);   
        if(giTraceFlag) iTraceText (cTextMessage);
        MessagePopup("RS-ITS", cTextMessage);   
    } else 
    {
        if (giTraceFlag)
        {
            sprintf(cTextMessage, "Function %s finished with no errors", cFunctionName);
            iTraceText (cTextMessage);  
        }
    }
    
    return sErrorOccurred;
        
}



///////////////////////////////////////////////////////////////////////////

int iOpenTraceFile (void)
{
    char cTraceFileName[1024];
    
    sprintf(cTraceFileName, "%sRS_TraceFile_%s_%s.log", gcPublicDocDir, DateStr(), TimeStr());
    cTraceFileName[strlen(cTraceFileName) - 7] = '_';
    cTraceFileName[strlen(cTraceFileName) - 10] = '_'; 
    cTraceFileName[strlen(cTraceFileName) - 18] = '_'; 
    cTraceFileName[strlen(cTraceFileName) - 21] = '_'; 
    pTraceFile = fopen(cTraceFileName, "w");
    
    if (!pTraceFile) 
    {
        MessagePopup("RS-ITS", "Could not create trace file!");
        return 1;
    }
    
    return 0; 
}

///////////////////////////////////////////////////////////////////////////

int iCloseTraceFile (void)
{
    
    fclose (pTraceFile);
    return 0;
}

///////////////////////////////////////////////////////////////////////////

int iTraceText (char *cTextToTrace)
{
    double dTimeStamp;
        
    gdStopTime = HRESTIM_getTimeStamp(); 
    
    dTimeStamp = gdStopTime - gdStartTime;
    
    fprintf (pTraceFile, "%.3fs      %s\n", dTimeStamp, cTextToTrace);
    
    gdStartTime = HRESTIM_getTimeStamp();
    
    return 0;
}


int CheckForFixtureCodes (IniText IniFileHandle) {
	
	int idx;
	int iNumberOfSections;
	int iFixtureCodeExists;
	int iSequenceExists;
	int iSectionFound;
	int iError;
	int iSize;
	int iFileExists;
	
	char *cSectionName;
	char cSequencePath[1024];
	char cFixtureCode[1024];
	char cRunBatchFile[512];
	int iWaitForBatchFile;
	double dDelayAfterBatchFile;
		
	iNumberOfSections = Ini_NumberOfSections (IniFileHandle); 
	for (idx = 1; idx <= iNumberOfSections; idx++)
	{
		//*************************************************************************/
		//Get section name
		iSectionFound = Ini_NthSectionName (IniFileHandle, idx, &cSectionName);
		if (giTraceFlag) {
			sprintf(gcTraceText, "Section %d: %s", idx, cSectionName);
        	iTraceText (gcTraceText);
		}
        if (!iSectionFound) return 1;
		if (stricmp(cSectionName, "RunSequence"))
		{
			//*************************************************************************/
			//Check for fixture code entry
			iFixtureCodeExists = Ini_ItemExists (IniFileHandle, cSectionName, "FixtureCode");
			if (giTraceFlag) {
				sprintf(gcTraceText, "Fixture Code Exists: %d", iFixtureCodeExists);
	        	iTraceText (gcTraceText);
			}
        	//*************************************************************************/
			//Check for sequence file entry
			iSequenceExists = Ini_ItemExists (IniFileHandle, cSectionName, "Sequence");
			if (giTraceFlag) {
				sprintf(gcTraceText, "Sequence Exists: %d", iSequenceExists);
	        	iTraceText (gcTraceText);
			}
        	if (iFixtureCodeExists && iSequenceExists)
			{
				//*************************************************************************/
				//Check for fixture code value
				iError = Ini_GetRawStringIntoBuffer (IniFileHandle, cSectionName, "FixtureCode", cFixtureCode, 1024);
				if (giTraceFlag) {
					sprintf(gcTraceText, "Fixture Code: %s", cFixtureCode);
		        	iTraceText (gcTraceText);
				}
	        	if (iError != 1) return 1;
				//*************************************************************************/
				//Check for sequence value
				iError = Ini_GetRawStringIntoBuffer (IniFileHandle, cSectionName, "Sequence", cSequencePath, 1024);
				if (giTraceFlag) {
					sprintf(gcTraceText, "Sequence Path: %s", cSequencePath);
		        	iTraceText (gcTraceText);
				}
	        	if (iError != 1) return 1;
				//*************************************************************************/
				//Check sequence path and file name
				iFileExists = FileExists (cSequencePath, &iSize);
				if (giTraceFlag) {
					sprintf(gcTraceText, "Sequence file exists: %d", iFileExists);
		        	iTraceText (gcTraceText);
				}
				//*************************************************************************/
				//Load global batch file
				Ini_GetRawStringIntoBuffer (IniFileHandle, cSectionName, "RunBatchFile", cRunBatchFile, 512);
				if (giTraceFlag) {
					sprintf(gcTraceText, "RunBatchFile: %s", cRunBatchFile);
		        	iTraceText (gcTraceText);
				}
		        if (strlen(cRunBatchFile))
				{
					//*************************************************************************/
					//Load wait for global batch file
			        iError = Ini_GetInt (IniFileHandle, cSectionName, "WaitForBatchFile", &iWaitForBatchFile);
			        if (giTraceFlag) {
						sprintf(gcTraceText, "WaitForBatchFile: %d", iWaitForGlobalBatchFile);
			        	iTraceText (gcTraceText);
					}
		        	if (iError != 1) return 1;
					//*************************************************************************/
					//Load fixture sure counter
			        iError = Ini_GetDouble (IniFileHandle, cSectionName, "DelayAfterBatchFile", &dDelayAfterBatchFile);
			        if (giTraceFlag) {
						sprintf(gcTraceText, "DelayForBatchFile: %e", dDelayAfterGlobalBatchFile);
			        	iTraceText (gcTraceText);
					}
        			if (iError != 1) return 1;
				} else {
					iWaitForBatchFile = 0;
					dDelayAfterBatchFile = 0.0;
				}
		
	        	if(iFileExists)
				{
					iError = AppendFixtureCode (cSectionName, cFixtureCode, cSequencePath, cRunBatchFile, iWaitForBatchFile, dDelayAfterBatchFile);
					if (giTraceFlag) iTraceText ("Append fixture complete");  
		            if (iError != 0) return 1;
				} else {
					sprintf(gTempBuffer, "File does not exists:\n\nSection: %s\nFixtureCode: %s\nSequencePath: %s\n\nContinue?", cSectionName, cFixtureCode, cSequencePath);
					iError = ConfirmPopup("RS-ITS RunSequence", gTempBuffer); 
					if (iError == 0) return 1;
				}
			}
		}
	}
	return 0;
}

int AppendFixtureCode (char *cProductName, char *cFixtureCode, char *cSequencePath, char *cRunBatchFile, int iWaitForBatchFile, double dDelayAfterBatchFile)
{
	struct FIXTURECODE *pPointer;
	int idx = 0;
	
	if (!pFirst)
	{
		pFirst = malloc (sizeof(struct FIXTURECODE));
		if (!pFirst) return -1;
		sprintf(pFirst->cProductName, cProductName);
		sprintf(pFirst->cFixtureCode, cFixtureCode);
		sprintf(pFirst->cSequencePathAndFileName, cSequencePath);
		sprintf(pFirst->cRunBatchFile, cRunBatchFile);
		pFirst->iWaitForBatchFile = iWaitForBatchFile;
		pFirst->dDelayAfterBatchFile = dDelayAfterBatchFile;
		pFirst->iCount = idx;
		pFirst->pNext = 0;
		if (giTraceFlag) {
			sprintf(gcTraceText, "Append fixture code, create first pointer");
        	iTraceText (gcTraceText);
		}
	        	
	} else 
	{
		pPointer = pFirst;
		while (pPointer->pNext)
		{
			pPointer = pPointer->pNext;
			idx++;
		}
		pPointer->pNext = malloc (sizeof(struct FIXTURECODE));
		if (!pPointer->pNext) return -1;
		sprintf(pPointer->pNext->cProductName, cProductName);
		sprintf(pPointer->pNext->cFixtureCode, cFixtureCode);
		sprintf(pPointer->pNext->cSequencePathAndFileName, cSequencePath);
		sprintf(pPointer->pNext->cRunBatchFile, cRunBatchFile);
		pPointer->pNext->iWaitForBatchFile = iWaitForBatchFile;
		pPointer->pNext->dDelayAfterBatchFile = dDelayAfterBatchFile;
		pPointer->pNext->iCount = idx + 1;
		pPointer->pNext->pNext = 0;
		if (giTraceFlag) {
			sprintf(gcTraceText, "Append fixture code, create pointer number %d", idx + 1);
        	iTraceText (gcTraceText);
		}
	    
	}
	return 0;
}

void UpDateFixtureCodeList (void)
{
	struct FIXTURECODE *pPointer;
	
	if (giTraceFlag) {
		sprintf(gcTraceText, "Start UpDateFixtureCodeList");
    	iTraceText (gcTraceText);
	}
	
	if(pFirst)
	{
		pPointer = pFirst;
		ClearListCtrl (gPanel, RS_SEQUENCES);
		while (pPointer)
		{
			InsertListItem (gPanel, RS_SEQUENCES, -1, pPointer->cProductName, pPointer->iCount);
			pPointer = pPointer->pNext;
		}
	}
	
	if (giTraceFlag) {
		sprintf(gcTraceText, "End UpDateFixtureCodeList");
    	iTraceText (gcTraceText);
	}
	
}

int GetFixtureData (int iFixtureListItem, char *cFixtureCode, char *cProductName, char *cSequencePath)
{
	struct FIXTURECODE *pPointer = NULL;
	
	if(!giSetupComplete) return 0;
	
	if (giTraceFlag) {
		sprintf(gcTraceText, "Start GetFixtureData ListItem: %d, FixtureCode: %s", iFixtureListItem, cFixtureCode);
    	iTraceText (gcTraceText);
	}
	
	sprintf(cProductName, "");
	sprintf(cSequencePath, "");
		
	if (iFixtureListItem == -1)
	{
		if(pFirst)
		{
			pPointer = pFirst;
			while (pPointer)
			{
				if(!strcmp(pPointer->cFixtureCode, cFixtureCode)) break;
				pPointer = pPointer->pNext; 
			}
		}
		if (pPointer)
		{
			sprintf(cProductName, pPointer->cProductName);
			sprintf(cSequencePath, pPointer->cSequencePathAndFileName);
			sprintf(cRunSequenceBatchFile, pPointer->cRunBatchFile);
			iWaitForSequenceBatchFile, pPointer->iWaitForBatchFile;
			dDelayAfterSequenceBatchFile, pPointer->dDelayAfterBatchFile;

		}
	} else 
	{
		if(pFirst)
		{
			pPointer = pFirst;
			while (pPointer)
			{
				if(pPointer->iCount == iFixtureListItem) break;
				pPointer = pPointer->pNext; 
			}
		}
		if (pPointer)
		{
			sprintf(cFixtureCode, pPointer->cFixtureCode);
			sprintf(cProductName, pPointer->cProductName);
			sprintf(cSequencePath, pPointer->cSequencePathAndFileName);
			sprintf(cRunSequenceBatchFile, pPointer->cRunBatchFile);
			iWaitForSequenceBatchFile, pPointer->iWaitForBatchFile;
			dDelayAfterSequenceBatchFile, pPointer->dDelayAfterBatchFile;

		}	
	}
	if (giTraceFlag) {
		sprintf(gcTraceText, "End GetFixtureData (%s, %s)", cProductName, cSequencePath);
    	iTraceText (gcTraceText);
	}
	
	return 0;
}

int LoadSequenceAndStartExecution (void)
{
	int iCanRunSeq;
	int iStartSequence;
	
	if (giShowLoadAndRunMenu) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_LOADANDRUN, ATTR_DIMMED, TRUE);
	SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_TERMINATE, ATTR_DIMMED, FALSE);
	SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_ABORT, ATTR_DIMMED, FALSE);
	SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_RESUME, ATTR_DIMMED, FALSE);
	if (pFirst && giEnableFixtureCheck && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_ENABLEFCC, ATTR_DIMMED, TRUE);
	if (pFirst && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_RUNSELECTED, ATTR_DIMMED, TRUE);
	SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_QUIT, ATTR_DIMMED, TRUE);
	if (pFirst) SetCtrlAttribute (gPanel, RS_SEQUENCES, ATTR_DIMMED, TRUE);
	if (pFirst && giEnableFixtureCheck) SetCtrlAttribute (gPanel, RS_ENABLEFIXCHECK, ATTR_DIMMED, TRUE);
	if (pFirst) SetCtrlAttribute (gPanel, RS_RUNSELECTED, ATTR_DIMMED, TRUE);
	SetPanelAttribute (gPanel, ATTR_CLOSE_ITEM_VISIBLE, FALSE);
	ProcessDrawEvents();
	
		
	if (iCheckError (OpenSequence(gPanel, &iCanRunSeq), "OpenSequence")) goto GOTO_OUT_ERROR_TAG;
	if (iCanRunSeq) 
	{
		CheckSequence(&iStartSequence);
		giRTEOccurred = 0;
		giExecutionPaused = 0;
		if (iStartSequence)
		{
			SetCtrlVal (gPanel, RS_SELECTEDSEQ, "Running batch file - please wait!");
			ProcessDrawEvents();
	
			if (strlen(cRunSequenceBatchFile))
		    {
			  if (iWaitForSequenceBatchFile)
			  {
			      system(cRunSequenceBatchFile);	
			  } else {
			      LaunchExecutable(cRunSequenceBatchFile);
				  Delay(dDelayAfterSequenceBatchFile);
			  }
		    }
			
			if(iCheckError(ExecuteSequence("Test UUTs", VTRUE), "ExecuteSequence -> Test UUTs"))
				goto GOTO_OUT_ERROR_TAG;
		}		
		
		
	} else {
		
		if (giShowLoadAndRunMenu) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_LOADANDRUN, ATTR_DIMMED, FALSE);
		SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_TERMINATE, ATTR_DIMMED, TRUE);
		SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_ABORT, ATTR_DIMMED, TRUE);
		SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_RESUME, ATTR_DIMMED, TRUE);
		if (pFirst && giEnableFixtureCheck && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_ENABLEFCC, ATTR_DIMMED, FALSE);
		if (pFirst && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_RUNSELECTED, ATTR_DIMMED, FALSE);
		SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_QUIT, ATTR_DIMMED, FALSE);
		if (pFirst) SetCtrlAttribute (gPanel, RS_SEQUENCES, ATTR_DIMMED, FALSE);
		if (pFirst && giEnableFixtureCheck) SetCtrlAttribute (gPanel, RS_ENABLEFIXCHECK, ATTR_DIMMED, FALSE);
		if (pFirst) SetCtrlAttribute (gPanel, RS_RUNSELECTED, ATTR_DIMMED, FALSE);
		SetPanelAttribute (gPanel, ATTR_CLOSE_ITEM_VISIBLE, TRUE);
		ProcessDrawEvents();
		
		
	}
	
	return 0;
	
GOTO_OUT_ERROR_TAG:
	
	if (giShowLoadAndRunMenu) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_LOADANDRUN, ATTR_DIMMED, FALSE);
	SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_TERMINATE, ATTR_DIMMED, TRUE);
	SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_ABORT, ATTR_DIMMED, TRUE);
	SetMenuBarAttribute (gMenuBar, MENUBAR_DEBUG_RESUME, ATTR_DIMMED, TRUE);
	if (pFirst && giEnableFixtureCheck && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_ENABLEFCC, ATTR_DIMMED, FALSE);
	if (pFirst && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_RUNSELECTED, ATTR_DIMMED, FALSE);
	SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_QUIT, ATTR_DIMMED, FALSE);
	if (pFirst) SetCtrlAttribute (gPanel, RS_SEQUENCES, ATTR_DIMMED, FALSE);
	if (pFirst && giEnableFixtureCheck) SetCtrlAttribute (gPanel, RS_ENABLEFIXCHECK, ATTR_DIMMED, FALSE);
	if (pFirst) SetCtrlAttribute (gPanel, RS_RUNSELECTED, ATTR_DIMMED, FALSE);
	SetPanelAttribute (gPanel, ATTR_CLOSE_ITEM_VISIBLE, TRUE);
	ProcessDrawEvents();
	
	return -1;

	
}

int StartFixtureCheck (void)
{
	if (pFirst) SetCtrlAttribute (gPanel, RS_SEQUENCES, ATTR_DIMMED, giFixtureCheckIsActive);
	if (pFirst) SetCtrlAttribute (gPanel, RS_RUNSELECTED, ATTR_DIMMED, giFixtureCheckIsActive);
	if (giFixtureCheckIsActive)
	{
		SetCtrlVal (gPanel, RS_SELECTEDSEQ, "Fixture check enabled - Please mount fixture!");
		SetCtrlAttribute (gPanel, RS_LED, ATTR_ON_COLOR, VAL_YELLOW);
		SetCtrlAttribute (gPanel, RS_FIXTURECODE, ATTR_ENABLED, TRUE);
		if (giShowLoadAndRunMenu) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_LOADANDRUN, ATTR_DIMMED, TRUE);
		if (pFirst && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_RUNSELECTED, ATTR_DIMMED, TRUE);
		SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_ENABLEFCC, ATTR_ITEM_NAME,
							 "Disable __Fixture Code Check");
		ProcessDrawEvents();
	    gfc_Setup (0, &gsErrorOccurred, &glErrorCode, gcErrorMessage);
		giFixtureCheckCounter = 0;
			
	} else
	{
		SetCtrlVal (gPanel, RS_SELECTEDSEQ, "Fixture check disabled");
		SetCtrlAttribute (gPanel, RS_LED, ATTR_ON_COLOR, VAL_RED);
		SetCtrlAttribute (gPanel, RS_FIXTURECODE, ATTR_ENABLED, FALSE);
		SetCtrlVal (gPanel, RS_LED, 0);
		if (giShowLoadAndRunMenu) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_LOADANDRUN, ATTR_DIMMED, FALSE);
		if (pFirst && !giAutoStartFixtureCheck) SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_RUNSELECTED, ATTR_DIMMED, FALSE);
	    SetMenuBarAttribute (gMenuBar, MENUBAR_FILE_ENABLEFCC, ATTR_ITEM_NAME, "Enable __Fixture Code Check");
	    ProcessDrawEvents();
		gfc_Cleanup (0, &gsErrorOccurred, &glErrorCode, gcErrorMessage);		
	}
	return 0;
}


int LoadIniFile (void)
{
	char cIniFilePath[1024];
	  IniText iniFileHandle;
	  int iError;
	  int iNumberOfSections;
	  iniFileHandle = 0;
  

  
	        //set default directory of file select popup
        sprintf (gcDefaultDirectory, "");
        //try load ini file
        sprintf(cIniFilePath, "%sConfiguration\\RunSequence.ini", gcPublicDocDir);
        iniFileHandle = Ini_New (0);
        if (!iniFileHandle) goto INIFILEERROR;
        iError = Ini_ReadFromFile (iniFileHandle, cIniFilePath);
        if (iError) goto INIFILEERROR;
        //if ini file found, load tracing flag
		if (!giTraceFlag)
		{
	        iError = Ini_GetInt (iniFileHandle, "RunSequence", "TracingEnabled", &giTraceFlag);
	        if (iError <= 0) 
	        {
	            giTraceFlag = 0;
	            goto INIFILEERROR; 
	        }
			//open trace file and start tracing
		    if (giTraceFlag) if(iOpenTraceFile()) goto INIFILEERROR ;
		    //if (giTraceFlag) gdStartTime = HRESTIM_getTimeStamp();
		    if (giTraceFlag) iTraceText ("Program started");
		    if (giTraceFlag) iTraceText ("Tracing enabled, Flag is true in the ini-file"); 
		}
        //if ini file found, load default sequence file directory
        iError = Ini_GetRawStringIntoBuffer (iniFileHandle, "RunSequence", "DefaultDirectory", 
            gcDefaultDirectory, 1024);
        if (iError <= 0) 
        {
            sprintf (gcDefaultDirectory, "");
            goto INIFILEERROR; 
        }
        if (giTraceFlag) {
			sprintf(gcTraceText, "DefaultDirectory: %s", gcDefaultDirectory);
        	iTraceText (gcTraceText);
		}
        //*************************************************************************/
		//Load file select popup flag
        iError = Ini_GetInt (iniFileHandle, "RunSequence", "ShowFileSelectPopup", &giShowFileSelectPopup);
        if (iError <= 0) 
        {
            giTraceFlag = 0;
            goto INIFILEERROR; 
        }				   
		if (giTraceFlag) {
			sprintf(gcTraceText, "ShowFileSelectPopup: %d", giShowFileSelectPopup);
        	iTraceText (gcTraceText);
		}
        //*************************************************************************/
		//Load quit after execution flag
        iError = Ini_GetInt (iniFileHandle, "RunSequence", "QuitAfterExecution", &giQuitAfterExecution);
        if (iError <= 0) 
        {
            giTraceFlag = 0;
            goto INIFILEERROR; 
        }
		if (giTraceFlag) {
			sprintf(gcTraceText, "QuitAfterExecution: %d", giQuitAfterExecution);
        	iTraceText (gcTraceText);
		}
        //*************************************************************************/
		//Load load and run menu entry flag
        iError = Ini_GetInt (iniFileHandle, "RunSequence", "ShowLoadAndRunMenu", &giShowLoadAndRunMenu);
        if (iError <= 0) 
        {
            giTraceFlag = 0;
            goto INIFILEERROR; 
        }
		if (giTraceFlag) {
			sprintf(gcTraceText, "ShowLoadAndRunMenu: %d", giShowLoadAndRunMenu);
        	iTraceText (gcTraceText);
		}
        //*************************************************************************/
		//Load auto start fixture start flag
        iError = Ini_GetInt (iniFileHandle, "RunSequence", "AutoStartFixtureCheck", &giAutoStartFixtureCheck);
        if (iError <= 0) 
        {
            giTraceFlag = 0;
            goto INIFILEERROR; 
        }
		if (giTraceFlag) {
			sprintf(gcTraceText, "AutoStartFixtureCheck: %d", giAutoStartFixtureCheck);
        	iTraceText (gcTraceText);
		}
        
        //*************************************************************************/
		//Load enable fixture check flag
        iError = Ini_GetInt (iniFileHandle, "RunSequence", "EnableFixtureCheck", &giEnableFixtureCheck);
        if (iError <= 0) 
        {
            giTraceFlag = 0;
            goto INIFILEERROR; 
        }
		if (giTraceFlag) {
			sprintf(gcTraceText, "EnableFixtureCheck: %d", giEnableFixtureCheck);
        	iTraceText (gcTraceText);
		}
        //*************************************************************************/
		//AutoStartFixtureCheck overwrites EnableFixtureCheck
		if(giAutoStartFixtureCheck && !giEnableFixtureCheck)
		{
			giEnableFixtureCheck = TRUE;
			if (giTraceFlag) {
				iTraceText ("AutoStartFixtureCheck overwrites EnableFixtureCheck");
				sprintf(gcTraceText, "EnableFixtureCheck: %d", giEnableFixtureCheck);
	        	iTraceText (gcTraceText);
			}
        
		}
		//*************************************************************************/
		//Load no fixture code value
        iError = Ini_GetRawStringIntoBuffer (iniFileHandle, "RunSequence", "NoFixtureCode", gcNoFixtureCode, 512);
		if (iError <= 0) 
        {
            giTraceFlag = 0;
            goto INIFILEERROR; 
        }
		if (giTraceFlag) {
			sprintf(gcTraceText, "NoFixtureCode: %s", gcNoFixtureCode);
        	iTraceText (gcTraceText);
		}
        //*************************************************************************/
		//Load path and file name of pyhsical.ini
        iError = Ini_GetRawStringIntoBuffer (iniFileHandle, "RunSequence", "PhysicalIniFile", gPhysicalIniFileName, 512);
		if (iError <= 0) 
        {
            giTraceFlag = 0;
            goto INIFILEERROR; 
        }
		if (giTraceFlag) {
			sprintf(gcTraceText, "PhysicalIniFile: %s", gPhysicalIniFileName);
        	iTraceText (gcTraceText);
		}
        //*************************************************************************/
		//Load fixture sure counter
        iError = Ini_GetInt (iniFileHandle, "RunSequence", "FixtureSureCounter", &giFixtureSureCounter);
        if (iError <= 0) 
        {
            giTraceFlag = 0;
            goto INIFILEERROR; 
        }
		if (giTraceFlag) {
			sprintf(gcTraceText, "FixtureSureCounter: %d", giFixtureSureCounter);
        	iTraceText (gcTraceText);
		}
        //*************************************************************************/
		//Load global batch file
		iError = Ini_GetRawStringIntoBuffer (iniFileHandle, "RunSequence", "RunGlobalBatchFile", cRunGlobalBatchFile, 512);
		if (iError <= 0) 
        {
            giTraceFlag = 0;
            goto INIFILEERROR; 
        }
		if (giTraceFlag) {
			sprintf(gcTraceText, "RunGlobalBatchFile: %s", cRunGlobalBatchFile);
        	iTraceText (gcTraceText);
		}
        if (strlen(cRunGlobalBatchFile))
		{
			//*************************************************************************/
			//Load wait for global batch file
	        iError = Ini_GetInt (iniFileHandle, "RunSequence", "WaitForGlobalBatchFile", &iWaitForGlobalBatchFile);
	        if (iError <= 0) 
	        {
	            giTraceFlag = 0;
	            goto INIFILEERROR; 
	        }
			if (giTraceFlag) {
				sprintf(gcTraceText, "WaitForGlobalBatchFile: %d", iWaitForGlobalBatchFile);
	        	iTraceText (gcTraceText);
			}
        	//*************************************************************************/
			//Load fixture sure counter
	        iError = Ini_GetDouble (iniFileHandle, "RunSequence", "DelayAfterGlobalBatchFile", &dDelayAfterGlobalBatchFile);
	        if (iError <= 0) 
	        {
	            giTraceFlag = 0;
	            goto INIFILEERROR; 
	        }
			if (giTraceFlag) {
				sprintf(gcTraceText, "DelayAfterGlobalBatchFile: %e", dDelayAfterGlobalBatchFile);
	        	iTraceText (gcTraceText);
			}
        	
		}
		
		
	
        
		//*************************************************************************/
		//Check for defined sequences in ini-file
		iNumberOfSections = Ini_NumberOfSections (iniFileHandle);
		if (giTraceFlag) {
			sprintf(gcTraceText, "Number of sections in ini-file: %d", iNumberOfSections);
        	iTraceText (gcTraceText);
		}
        iError = 0;
		if (iNumberOfSections > 1)
		    iError = CheckForFixtureCodes (iniFileHandle);	
		if (iError) MessagePopup ("RS-ITS RunSequence", "An error occurred while loading fixture codes. Please check RunSequence.ini file!");
        Ini_Dispose (iniFileHandle);
        iniFileHandle = 0;
		
		return 0;
		
  //If an ini file error occurred
INIFILEERROR:
  if (iniFileHandle) Ini_Dispose (iniFileHandle);  
   MessagePopup("RS-ITS", "Error in RunSequence.ini");		
   return -1; 
}


int CheckCommandline (int argc, char *argv[])
{
	int iError;
	int idx;
	
	/*
  /shellmode -> Change menü from Quit to Quit and logout
  /trace     -> Activate the tracing
  /file:     -> file runs immediately
	*/
	
  //check for command line arguments
  //-----> one argument
  if (argc == 1)
  {
         return 0;
  }
  
  for (idx = 1; idx < argc; idx++)
  {
	  if (!stricmp(argv[idx], "/shellmode")) giShellMode = 1;
	  if (!stricmp(argv[idx], "/trace")) giTraceFlag = 1;
	  if (!stricmp(argv[idx], "/file")) 
	  {
		  idx++;
		  while (argv[idx][0] != '/')
          {
            strcat(gSeqFilePath, argv[idx]);
			idx++;
			if (idx == argc) break;
            if (argv[idx][0] != '/') strcat(gSeqFilePath, " ");  
          }
		  if (argv[idx][0] == '/') idx--;
	  }
	  
	  
  }
	
	return 0;
	
GOTO_OUT_ERROR_TAG:
	
	return -1;
}


static void FixSlowMSVCHeap(void)
{
    PIMAGE_DOS_HEADER   pidh;
    PIMAGE_NT_HEADERS   pinh;

    pidh = (PIMAGE_DOS_HEADER)GetModuleHandle(NULL);

    if (!(pidh->e_magic != IMAGE_DOS_SIGNATURE || pidh->e_lfanew == 0))
    {       
        pinh = (PIMAGE_NT_HEADERS)(((PBYTE)pidh) + pidh->e_lfanew);

        if (pinh->OptionalHeader.MajorLinkerVersion < 6)
        {
            DWORD oldProtect, unused;
            if (VirtualProtect(&(pinh->OptionalHeader.MajorLinkerVersion), 4, PAGE_READWRITE, &oldProtect))
            {
                pinh->OptionalHeader.MajorLinkerVersion = 6; // set linker version high enough to make _set_sbh_threshold initialize with __V6_HEAP, not __V5_HEAP, which is three times slower for loading big files
                VirtualProtect(&(pinh->OptionalHeader.MajorLinkerVersion), 4, oldProtect, &unused);
            }
        }
    }    
} 

///////////////////////////////////////////////////////////////////////////
/* EOF */

