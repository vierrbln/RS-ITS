//#include <cvirte.h>		
#include <userint.h>
#include <windows.h>

#include "hrestim.h"  
#include "inifile.h"
#include "resmgr.h"
#include "rs.h"
#include "Splash.h"
#include "tsapicvi.h"   	// TestStand API
#include "tsui.h"			// API's for the TestStand ActiveX controls
#include <utility.h>
#include <formatio.h>
#include "GetFixtureCode.h"

#define DEBUG 0
#define MIN_UIMSG_DELAY    0 /* in milliseconds */

#define VERSION "2.0.0.15"

// Forward declarations:
int OpenSequence(int panel, int *iFileSelected);
int CheckSequence(int *iStartSequence);
int DisplayOLEError(int iError, ERRORINFO *errorInfo);
int ExecuteSequence(char *sequenceName, VBOOL useProcessModel);
int CallLoginLogoutCallback(VBOOL logoutOnly, VBOOL isInitialLogin);
void ClearGlobalDataForShutdown(void);
int InitEngine(void);
void CVICALLBACK UIMessageDeferredCallback(struct IDispatch *msgDisp);
void UIMessageCallback(struct IDispatch *msgDisp);
HRESULT hCheckResult (HRESULT hResult, char *cFunctionName, ERRORINFO hErrorInfo);
int iCheckError (int iError, char *cFunctionName);
short sCheckGTSLError (short sErrorOccurred, long lErrorCode, char *cErrorMessage, char *cFunctionName);
int iCheckUIError (int iError, char *cFunctionName);
int iOpenTraceFile (void);
int iCloseTraceFile (void);
int iTraceText (char *cTextToTrace);
int CheckUserPrivilege (int *iCanRunSeq);
int CheckForFixtureCodes (IniText IniFileHandle);
int AppendFixtureCode (char *cProductName, char *cFixtureCode, char *cSequencePath, char *cRunBatchFile, int iWaitForBatchFile, double dDelayAfterBatchFile);
int GetFixtureData (int iFixtureListItem, char *cFixtureCode, char *cProductName, char *cSequencePath);
void UpDateFixtureCodeList (void);
int LoadSequenceAndStartExecution (void);
int StartFixtureCheck (void);
int LoadIniFile (void);
int CheckCommandline (int argc, char *argv[]);
static void FixSlowMSVCHeap(void);

HRESULT CVICALLBACK AppMgr_RTE (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              TSUIObj_Execution  exec,
                              TSUIObj_Thread  initiatingThread,
                              VBOOL *showDialog,
                              VBOOL *breakExecution);



HRESULT CVICALLBACK AppMgr_EndExec (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              TSUIObj_Execution  exec);

HRESULT CVICALLBACK AppMgr_UIMsg (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              TSUIObj_UIMessage  uiMsg,
                              VBOOL *cancel);

HRESULT CVICALLBACK ExecMgr_RunStateChanged (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              enum TSUIEnum_ExecutionRunStates newRunState);


// Global variables:
char gSeqFilePath[MAX_PATHNAME_LEN];
char gSeqDrive[MAX_PATHNAME_LEN];
char gSeqDirectory[MAX_PATHNAME_LEN];
char gSeqFileName[MAX_PATHNAME_LEN];
char gPhysicalIniFileName[MAX_PATHNAME_LEN];
char gTempBuffer[MAX_PATHNAME_LEN];
int gPanel;
int gSplash;
int giSpashCounter;
int gMenuBar;
CAObjHandle ApplicationMgr;
CAObjHandle gCurrentExecViewMgr;
CAObjHandle gEngine;
CAObjHandle gStationOptions;
CAObjHandle gCurrentSequenceFile;
CAObjHandle gCurrentExecution;
CAObjHandle gCurrentUser;
int gCurrentExecutionId;
CAObjHandle gLoginLogoutExecution;
int gLoginLogoutExecutionId;
CAObjHandle gLoginLogoutArgs;
CAObjHandle gUser;
int gExiting;
CAObjHandle gReport;
struct IDispatch *sQueuedEvent;
double gTimeOut;
double gdStartTime;
double gdStopTime; 
int giFixtureCheckIsActive;
short gsErrorOccurred;
long glErrorCode;
char gcErrorMessage[1024];
int giExecutionPaused;
int giRTEOccurred;
int giSetupComplete;

int giTraceFlag;
FILE *pTraceFile;
char gcTraceText[1024];
int giShowFileSelectPopup;
int giRunSequenceFileUnload;
int giQuitAfterExecution; 
int giShowLoadAndRunMenu; 
int giAutoStartFixtureCheck;
int giEnableFixtureCheck;
char gcNoFixtureCode[1024];
int giFixtureSureCounter;
int giFixtureCheckCounter;
int giShellMode;
char giLastFixtureCode[1024];
char gcProjectDir[1024];
char gcPublicDocDir[1024];
char gcDefaultDirectory[1024];
char cRunGlobalBatchFile[512];
int iWaitForGlobalBatchFile;
double dDelayAfterGlobalBatchFile;
char cRunSequenceBatchFile[512];
int iWaitForSequenceBatchFile;
double dDelayAfterSequenceBatchFile;
int gShutdownPass;
int gNotShuttingDown;


HRESULT ghResult;

struct FIXTURECODE {
	char cProductName[512];
	char cFixtureCode[512];
	char cSequencePathAndFileName[512];
	char cRunBatchFile[512];
	int iWaitForBatchFile;
	double dDelayAfterBatchFile;
	int iCount;
	struct FIXTURECODE *pNext;
};

struct FIXTURECODE *pFirst;


