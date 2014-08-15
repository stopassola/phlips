   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.22  06/15/04            */
   /*                                                     */
   /*              FACT FUNCTIONS HEADER FILE             */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#ifndef _H_factfun
#define _H_factfun

#ifndef _H_factmngr
#include "factmngr.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _FACTFUN_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

#if ENVIRONMENT_API_ONLY
#define FactDeftemplate(theEnv,a) EnvFactDeftemplate(theEnv,a)
#define FactExistp(theEnv,a) EnvFactExistp(theEnv,a)
#define FactSlotNames(theEnv,a,b) EnvFactSlotNames(theEnv,a,b)
#define GetFactList(theEnv,a,b) EnvGetFactList(theEnv,a,b)
#else
#define FactDeftemplate(a) EnvFactDeftemplate(GetCurrentEnvironment(),a)
#define FactExistp(a) EnvFactExistp(GetCurrentEnvironment(),a)
#define FactSlotNames(a,b) EnvFactSlotNames(GetCurrentEnvironment(),a,b)
#define GetFactList(a,b) EnvGetFactList(GetCurrentEnvironment(),a,b)
#endif

   LOCALE void                           FactFunctionDefinitions(void *);
   LOCALE void                          *FactRelationFunction(void *);
   LOCALE void                          *FactRelation(void *);
   LOCALE void                          *EnvFactDeftemplate(void *,void *);
   LOCALE long int                       FactExistpFunction(void *);
   LOCALE long int                       EnvFactExistp(void *,void *);
   LOCALE void                           FactSlotValueFunction(void *,DATA_OBJECT *);
   LOCALE void                           FactSlotValue(void *,void *,char *,DATA_OBJECT *);
   LOCALE void                           FactSlotNamesFunction(void *,DATA_OBJECT *);
   LOCALE void                           EnvFactSlotNames(void *,void *,DATA_OBJECT *);
   LOCALE void                           GetFactListFunction(void *,DATA_OBJECT *);
   LOCALE void                           EnvGetFactList(void *,DATA_OBJECT *,void *);
   LOCALE struct fact                   *GetFactAddressOrIndexArgument(void *,char *,int,int);

#endif

