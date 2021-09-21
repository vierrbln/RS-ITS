/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  RS                               1       /* callback function: MainPanelCallBack */
#define  RS_LED                           2       /* control type: LED, callback function: (none) */
#define  RS_SELECTEDSEQ                   3       /* control type: textMsg, callback function: (none) */
#define  RS_TEXTMSG_2                     4       /* control type: textMsg, callback function: (none) */
#define  RS_SEQUENCES                     5       /* control type: listBox, callback function: SequenceListCallBack */
#define  RS_ENABLEFIXCHECK                6       /* control type: textButton, callback function: EnableFixtureCheck */
#define  RS_RUNSELECTED                   7       /* control type: command, callback function: RunSelectedSequence */
#define  RS_FIXTURECODE                   8       /* control type: timer, callback function: FixtureCodeCheck */
#define  RS_TIMER_RTE_CHECK               9       /* control type: timer, callback function: CheckRunState */
#define  RS_SEQUENCEFILEVIEWMGR           10      /* control type: activeX, callback function: (none) */
#define  RS_APPLICATIONMGR                11      /* control type: activeX, callback function: (none) */
#define  RS_TITEL2                        12      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_FILE                     2
#define  MENUBAR_FILE_LOADANDRUN          3       /* callback function: LoadAndRunSeq */
#define  MENUBAR_FILE_RUNSELECTED         4       /* callback function: RunSelectedSeq_menu */
#define  MENUBAR_FILE_ENABLEFCC           5       /* callback function: Enable_FCC_menu */
#define  MENUBAR_FILE_SEPARATOR_3         6
#define  MENUBAR_FILE_QUIT                7       /* callback function: QUIT */
#define  MENUBAR_DEBUG                    8
#define  MENUBAR_DEBUG_RESUME             9       /* callback function: ResumeExecution */
#define  MENUBAR_DEBUG_TERMINATE          10      /* callback function: TerminateExecution */
#define  MENUBAR_DEBUG_ABORT              11      /* callback function: ABORTEXECUTION */
#define  MENUBAR_HELP                     12
#define  MENUBAR_HELP_HELP                13      /* callback function: OpenHelp */
#define  MENUBAR_HELP_SEPARATOR           14
#define  MENUBAR_HELP_INFO                15      /* callback function: ShowSplashPanel */


     /* Callback Prototypes: */

void CVICALLBACK ABORTEXECUTION(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK CheckRunState(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK Enable_FCC_menu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK EnableFixtureCheck(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FixtureCodeCheck(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK LoadAndRunSeq(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK MainPanelCallBack(int panel, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK OpenHelp(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK QUIT(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK ResumeExecution(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK RunSelectedSeq_menu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK RunSelectedSequence(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SequenceListCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK ShowSplashPanel(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TerminateExecution(int menubar, int menuItem, void *callbackData, int panel);


#ifdef __cplusplus
    }
#endif
