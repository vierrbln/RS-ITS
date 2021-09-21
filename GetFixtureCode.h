#include <cvidef.h>
#include <tsapicvi.h>

void __stdcall gfc_Setup (CAObjHandle sequenceContext, short *errorOccurred,
                         long *errorCode, char errorMessage[]);

void __stdcall gfc_GetFixtureCode (CAObjHandle sequenceContext,
                                  char fixtureCode[], short *errorOccurred,
                                  long *errorCode, char errorMessage[]);

void __stdcall gfc_Cleanup (CAObjHandle sequenceContext, short *errorOccurred,
                           long *errorCode, char errorMessage[]);
