   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.22  06/15/04            */
   /*                                                     */
   /*                    AGENDA MODULE                    */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*   Provides functionality for examining, manipulating,     */
/*   adding, and removing activations from the agenda.       */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Donnell                                     */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#define _AGENDA_SOURCE_

#include <stdio.h>
#define _STDIO_INCLUDED_
#include <string.h>

#include "setup.h"

#if DEFRULE_CONSTRUCT

#include "argacces.h"
#include "constant.h"
#include "crstrtgy.h"
#include "engine.h"
#include "envrnmnt.h"
#include "extnfunc.h"
#include "memalloc.h"
#include "moduldef.h"
#include "modulutl.h"
#include "multifld.h"
#include "reteutil.h"
#include "retract.h"
#include "router.h"
#include "rulebsc.h"
#include "ruledef.h"
#include "strngrtr.h"
#include "sysdep.h"
#include "watch.h"

#include "agenda.h"

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static void                    PrintActivation(void *,char *,void *);
   static void                    AgendaClearFunction(void *);
#if DYNAMIC_SALIENCE
   static char                   *SalienceEvaluationName(int);
   static int                     EvaluateSalience(void *,void *);
#endif

/*************************************************/
/* InitializeAgenda: Initializes the activations */
/*   watch item and the H/L commands for         */
/*   manipulating the agenda.                    */
/*************************************************/
globle void InitializeAgenda(
  void *theEnv)
  {   
   AllocateEnvironmentData(theEnv,AGENDA_DATA,sizeof(struct agendaData),NULL);
   
   AgendaData(theEnv)->SalienceEvaluation = WHEN_DEFINED;
#if CONFLICT_RESOLUTION_STRATEGIES
   AgendaData(theEnv)->Strategy = DEFAULT_STRATEGY;
#endif
   
   EnvAddClearFunction(theEnv,"agenda",AgendaClearFunction,0);
#if DEBUGGING_FUNCTIONS
   AddWatchItem(theEnv,"activations",1,&AgendaData(theEnv)->WatchActivations,40,DefruleWatchAccess,DefruleWatchPrint);
#endif
#if ! RUN_TIME
   EnvDefineFunction2(theEnv,"refresh", 'v', PTIEF RefreshCommand, "RefreshCommand", "11w");
#if DYNAMIC_SALIENCE
   EnvDefineFunction2(theEnv,"refresh-agenda",'v',
                   PTIEF RefreshAgendaCommand,"RefreshAgendaCommand", "01w");
   EnvDefineFunction2(theEnv,"get-salience-evaluation",'w',
                   PTIEF GetSalienceEvaluationCommand,
                   "GetSalienceEvaluationCommand", "00");
   EnvDefineFunction2(theEnv,"set-salience-evaluation",'w',
                   PTIEF SetSalienceEvaluationCommand,
                   "SetSalienceEvaluationCommand",
                   "11w");
#endif
#if DEBUGGING_FUNCTIONS
   EnvDefineFunction2(theEnv,"agenda", 'v', PTIEF AgendaCommand, "AgendaCommand", "01w");
#endif
#endif
  }

/*****************************************************************/
/* AddActivation: Creates a rule activation to be added to the   */
/*   Agenda and links the activation with its associated partial */
/*   match. The function PlaceActivation is then called to place */
/*   the activation on the Agenda. Typically called when all     */
/*   patterns on the LHS of a rule have been satisfied.          */
/*****************************************************************/
globle void AddActivation(
  void *theEnv,
  void *vTheRule,
  void *vBinds)
  {
   struct activation *newActivation;
   struct defrule *theRule = (struct defrule *) vTheRule;
   struct partialMatch *binds = (struct partialMatch *) vBinds;
   struct defruleModule *theModuleItem;

   /*=======================================*/
   /* Focus on the module if the activation */
   /* is from an auto-focus rule.           */
   /*=======================================*/

   if (theRule->autoFocus)
     { EnvFocus(theEnv,(void *) theRule->header.whichModule->theModule); }

   /*=======================================================*/
   /* Create the activation. The activation stores pointers */
   /* to its associated partial match and defrule. The      */
   /* activation is given a time tag, its salience is       */
   /* evaluated, and it is assigned a random number for use */
   /* with the random conflict resolution strategy.         */
   /*=======================================================*/

   newActivation = get_struct(theEnv,activation);
   newActivation->theRule = theRule;
   newActivation->basis = binds;
   newActivation->timetag = AgendaData(theEnv)->CurrentTimetag++;
#if DYNAMIC_SALIENCE
   newActivation->salience = EvaluateSalience(theEnv,theRule);
#else
   newActivation->salience = theRule->salience;
#endif
#if CONFLICT_RESOLUTION_STRATEGIES
   newActivation->sortedBasis = NULL;
   newActivation->randomID = genrand();
#endif
   newActivation->prev = NULL;
   newActivation->next = NULL;

   AgendaData(theEnv)->NumberOfActivations++;

   /*=======================================================*/
   /* Point the partial match to the activation to complete */
   /* the link between the join network and the agenda.     */
   /*=======================================================*/

   binds->binds[binds->bcount].gm.theValue = (void *) newActivation;

   /*====================================================*/
   /* If activations are being watch, display a message. */
   /*====================================================*/

#if DEBUGGING_FUNCTIONS
   if (newActivation->theRule->watchActivation)
     {
      EnvPrintRouter(theEnv,WTRACE,"==> Activation ");
      PrintActivation(theEnv,WTRACE,(void *) newActivation);
      EnvPrintRouter(theEnv,WTRACE,"\n");
     }
#endif

    /*=====================================*/
    /* Place the activation on the agenda. */
    /*=====================================*/

    theModuleItem = (struct defruleModule *) theRule->header.whichModule;
    PlaceActivation(theEnv,&(theModuleItem->agenda),newActivation);
   }

/***************************************************************/
/* ClearRuleFromAgenda: Clears the agenda of a specified rule. */
/***************************************************************/
globle void ClearRuleFromAgenda(
  void *theEnv,
  void *vTheRule)
  {
   struct defrule *theRule = (struct defrule *) vTheRule;
   struct defrule *tempRule;
   struct activation *agendaPtr, *agendaNext;

   /*============================================*/
   /* Get a pointer to the agenda for the module */
   /* in which the rule is contained.            */
   /*============================================*/

   agendaPtr = ((struct defruleModule *) theRule->header.whichModule)->agenda;

   /*==============================================*/
   /* Loop through every activation on the agenda. */
   /*==============================================*/

   while (agendaPtr != NULL)
     {
      agendaNext = agendaPtr->next;

      /*========================================================*/
      /* Check each disjunct of the rule against the activation */
      /* to determine if the activation points to the rule. If  */
      /* it does, then remove the activation from the agenda.   */
      /*========================================================*/

      for (tempRule = theRule;
           tempRule != NULL;
           tempRule = tempRule->disjunct)
        {
         if (agendaPtr->theRule == tempRule)
           {
            RemoveActivation(theEnv,agendaPtr,TRUE,TRUE);
            break;
           }
        }

      agendaPtr = agendaNext;
     }
  }

/****************************************************************/
/* EnvGetNextActivation: Returns an activation from the Agenda. */
/*   If its argument is NULL, then the first activation on the  */
/*   Agenda is returned. If its argument is not NULL, the next  */
/*   activation after the argument is returned.                 */
/****************************************************************/
globle void *EnvGetNextActivation(
  void *theEnv,
  void *actPtr)
  {
   struct defruleModule *theModuleItem;
   
   if (actPtr == NULL)
     {
      theModuleItem = (struct defruleModule *) GetModuleItem(theEnv,NULL,DefruleData(theEnv)->DefruleModuleIndex);
      if (theModuleItem == NULL) return(NULL);
      return((void *) theModuleItem->agenda);
     }
   else
     { return((void *) (((struct activation *) actPtr)->next)); }
  }

/*********************************************/
/* EnvGetActivationName: Returns the name of */
/*   the rule associated with an activation. */
/*********************************************/
#if IBM_TBC
#pragma argsused
#endif
globle char *EnvGetActivationName(
  void *theEnv,
  void *actPtr)
  {
#if MAC_MCW || IBM_MCW
#pragma unused(theEnv)
#endif

   return(ValueToString(((struct activation *) actPtr)->theRule->header.name)); 
  }

/**************************************/
/* EnvSetActivationSalience: Sets the */
/*   salience value of an activation. */
/**************************************/
#if IBM_TBC
#pragma argsused
#endif
globle int EnvSetActivationSalience(
  void *theEnv,
  void *actPtr,
  int value)
  {
   int temp;
#if MAC_MCW || IBM_MCW
#pragma unused(theEnv)
#endif

   temp = ((struct activation *) actPtr)->salience;
   ((struct activation *) actPtr)->salience = value;
   return(temp);
  }

/**********************************************/
/* EnvGetActivationPPForm: Returns the pretty */
/*   print representation of an activation.   */
/**********************************************/
globle void EnvGetActivationPPForm(
  void *theEnv,
  char *buffer,
  unsigned bufferLength,
  void *theActivation)
  {
   OpenStringDestination(theEnv,"ActPPForm",buffer,bufferLength);
   PrintActivation(theEnv,"ActPPForm",(void *) theActivation);
   CloseStringDestination(theEnv,"ActPPForm");
  }

/********************************************/
/* MoveActivationToTop: Moves the specified */
/*   activation to the top of the agenda.   */
/********************************************/
globle BOOLEAN MoveActivationToTop(
  void *theEnv,
  void *vtheActivation)
  {
   struct activation *prevPtr;
   struct activation *theActivation = (struct activation *) vtheActivation;
   struct defruleModule *theModuleItem;

   /*====================================*/
   /* Determine the module of the agenda */
   /* in which the activation is stored. */
   /*====================================*/

   theModuleItem = (struct defruleModule *) theActivation->theRule->header.whichModule;

   /*============================================*/
   /* If the activation is already at the top of */
   /* the agenda, then nothing needs to be done. */
   /*============================================*/

   if (theActivation == theModuleItem->agenda) return(FALSE);

   /*=================================================*/
   /* Update the pointers of the activation preceding */
   /* and following the activation being moved.       */
   /*=================================================*/

   prevPtr = theActivation->prev;
   prevPtr->next = theActivation->next;
   if (theActivation->next != NULL) theActivation->next->prev = prevPtr;

   /*=======================================================*/
   /* Move the activation and then update its pointers, the */
   /* pointers of the activation following it, and the      */
   /* module pointer to the top activation on the agenda.   */
   /*=======================================================*/

   theActivation->next = theModuleItem->agenda;
   theModuleItem->agenda->prev = theActivation;
   theActivation->prev = NULL;
   theModuleItem->agenda = theActivation;

   /*=============================*/
   /* Mark the agenda as changed. */
   /*=============================*/

   AgendaData(theEnv)->AgendaChanged = TRUE;

   return(TRUE);
  }

/**********************************************/
/* EnvDeleteActivation: Removes the specified */
/*   activation from the agenda.              */
/**********************************************/
globle BOOLEAN EnvDeleteActivation(
  void *theEnv,
  void *theActivation)
  {
   if (theActivation == NULL) RemoveAllActivations(theEnv);
   else RemoveActivation(theEnv,(struct activation *) theActivation,TRUE,TRUE);

   return(TRUE);
  }

/*******************************************************/
/* DetachActivation: Detaches the specified activation */
/*   from the list of activations on the Agenda.       */
/*******************************************************/
globle BOOLEAN DetachActivation(
  void *theEnv,
  void *vTheActivation)
  {
   struct defruleModule *theModuleItem;
   struct activation *theActivation = (struct activation *) vTheActivation;

   /*============================*/
   /* A NULL pointer is invalid. */
   /*============================*/

   if (theActivation == NULL) SystemError(theEnv,"AGENDA",1);

   /*====================================*/
   /* Determine the module of the agenda */
   /* in which the activation is stored. */
   /*====================================*/

   theModuleItem = (struct defruleModule *) theActivation->theRule->header.whichModule;

   /*========================================================*/
   /* If the activation is the top activation on the agenda, */
   /* then update the module pointer to agenda.              */
   /*========================================================*/

   if (theActivation == theModuleItem->agenda)
     { theModuleItem->agenda = theActivation->next; }

   /*==================================================*/
   /* Update the pointers in the preceding activation. */
   /*==================================================*/

   if (theActivation->prev != NULL)
     { theActivation->prev->next = theActivation->next; }

   /*==================================================*/
   /* Update the pointers in the following activation. */
   /*==================================================*/

   if (theActivation->next != NULL)
     { theActivation->next->prev = theActivation->prev; }

   /*=================================================*/
   /* Update the pointers in the detached activation. */
   /*=================================================*/

   theActivation->prev = NULL;
   theActivation->next = NULL;

   /*=============================*/
   /* Mark the agenda as changed. */
   /*=============================*/

   AgendaData(theEnv)->AgendaChanged = TRUE;

   return(TRUE);
  }

/****************************************************************************/
/* PrintActivation: Prints an activation in a "pretty" format. Salience,    */
/*   rule name, and the partial match which activated the rule are printed. */
/****************************************************************************/
static void PrintActivation(
  void *theEnv,
  char *logicalName,
  void *vTheActivation)
  {
   struct activation *theActivation = (struct activation *) vTheActivation;
   char printSpace[20];

   sprintf(printSpace,"%-6d ",theActivation->salience);
   EnvPrintRouter(theEnv,logicalName,printSpace);
   EnvPrintRouter(theEnv,logicalName,ValueToString(theActivation->theRule->header.name));
   EnvPrintRouter(theEnv,logicalName,": ");
   PrintPartialMatch(theEnv,logicalName,theActivation->basis);
  }

/*******************************/
/* EnvAgenda: C access routine */
/*   for the agenda command.   */
/*******************************/
globle void EnvAgenda(
  void *theEnv,
  char *logicalName,
  void *vTheModule)
  {
   struct defmodule *theModule = (struct defmodule *) vTheModule;

   ListItemsDriver(theEnv,logicalName,theModule,"activation","activations",
                   EnvGetNextActivation,NULL,PrintActivation,NULL);
  }

/*******************************************************************/
/* RemoveActivation: Returns an activation and its associated data */
/*   structures to the Memory Manager. Links to other activations  */
/*   and partial matches may also be updated.                      */
/*******************************************************************/
globle void RemoveActivation(
  void *theEnv,
  void *vTheActivation,
  int updateAgenda,
  int updateLinks)
  {
   struct defruleModule *theModuleItem;
   struct activation *theActivation = (struct activation *) vTheActivation;

   /*====================================*/
   /* Determine the module of the agenda */
   /* in which the activation is stored. */
   /*====================================*/

   theModuleItem = (struct defruleModule *) theActivation->theRule->header.whichModule;

   /*=================================*/
   /* Update the agenda if necessary. */
   /*=================================*/

   if (updateAgenda == TRUE)
     {
      /*===============================================*/
      /* Update the pointer links between activations. */
      /*===============================================*/

      if (theActivation->prev == NULL)
        {
         theModuleItem->agenda = theModuleItem->agenda->next;
         if (theModuleItem->agenda != NULL) theModuleItem->agenda->prev = NULL;
        }
      else
        {
         theActivation->prev->next = theActivation->next;
         if (theActivation->next != NULL)
           { theActivation->next->prev = theActivation->prev; }
        }

      /*===================================*/
      /* Indicate removal of activation if */
      /* activations are being watched.    */
      /*===================================*/

#if DEBUGGING_FUNCTIONS
      if (theActivation->theRule->watchActivation)
        {
         EnvPrintRouter(theEnv,WTRACE,"<== Activation ");
         PrintActivation(theEnv,WTRACE,(void *) theActivation);
         EnvPrintRouter(theEnv,WTRACE,"\n");
        }
#endif

      /*=============================*/
      /* Mark the agenda as changed. */
      /*=============================*/

      AgendaData(theEnv)->AgendaChanged = TRUE;
     }

   /*============================================*/
   /* Update join and agenda links if necessary. */
   /*============================================*/

   if ((updateLinks == TRUE) && (theActivation->basis != NULL))
     { theActivation->basis->binds[theActivation->basis->bcount].gm.theValue = NULL; }

   /*================================================*/
   /* Return the activation to the free memory pool. */
   /*================================================*/

   AgendaData(theEnv)->NumberOfActivations--;

#if CONFLICT_RESOLUTION_STRATEGIES
   if (theActivation->sortedBasis != NULL)
     { ReturnPartialMatch(theEnv,theActivation->sortedBasis); }
#endif

   rtn_struct(theEnv,activation,theActivation);
  }

/**************************************************************/
/* AgendaClearFunction: Agenda clear routine for use with the */
/*   clear command. Resets the current time tag to zero.      */
/**************************************************************/
static void AgendaClearFunction(
  void *theEnv)
  {
   AgendaData(theEnv)->CurrentTimetag = 0;
  }

/*************************************************/
/* RemoveAllActivations: Removes all activations */
/*   from the agenda of the current module.      */
/*************************************************/
globle void RemoveAllActivations(
  void *theEnv)
  {
   struct activation *tempPtr, *theActivation;

   theActivation = GetDefruleModuleItem(theEnv,NULL)->agenda;
   while (theActivation != NULL)
     {
      tempPtr = theActivation->next;
      RemoveActivation(theEnv,theActivation,TRUE,TRUE);
      theActivation = tempPtr;
     }
  }

/*********************************************************/
/* EnvGetAgendaChanged: Returns the value of the boolean */
/*   flag which indicates whether any changes have been  */
/*   made to the agenda.                                 */
/*********************************************************/
globle int EnvGetAgendaChanged(
  void *theEnv)
  {
   return(AgendaData(theEnv)->AgendaChanged);
  }

/*****************************************************************/
/* EnvSetAgendaChanged: Sets the value of the boolean flag which */
/*   indicates whether any changes have been made to the agenda. */
/*****************************************************************/
globle void EnvSetAgendaChanged(
  void *theEnv,
  int value)
  {
   AgendaData(theEnv)->AgendaChanged = value;
  }

/**********************************************************/
/* EnvReorderAgenda: Completely reorders the agenda based */
/*   on the current conflict resolution strategy.         */
/**********************************************************/
globle void EnvReorderAgenda(
  void *theEnv,
  void *vTheModule)
  {
   struct activation *theActivation, *tempPtr;
   struct defmodule *theModule = (struct defmodule *) vTheModule;
   int allModules = FALSE;
   struct defruleModule *theModuleItem;

   /*=============================================*/
   /* If the module specified is a NULL pointer,  */
   /* then every module has its agenda reordered. */
   /*=============================================*/

   if (theModule == NULL)
     {
      allModules = TRUE;
      theModule = (struct defmodule *) EnvGetNextDefmodule(theEnv,NULL);
     }

   /*========================*/
   /* Reorder the agenda(s). */
   /*========================*/

   for (;
        theModule != NULL;
        theModule = (struct defmodule *) EnvGetNextDefmodule(theEnv,theModule))
     {
      /*=================================*/
      /* Get the list of activations and */
      /* remove them from the agenda.    */
      /*=================================*/

      theModuleItem = GetDefruleModuleItem(theEnv,theModule);
      theActivation = theModuleItem->agenda;
      theModuleItem->agenda = NULL;

      /*=========================================*/
      /* Reorder the activations by placing them */
      /* back on the agenda one by one.          */
      /*=========================================*/

      while (theActivation != NULL)
        {
         tempPtr = theActivation->next;
         theActivation->next = NULL;
         theActivation->prev = NULL;
         PlaceActivation(theEnv,&(theModuleItem->agenda),theActivation);
         theActivation = tempPtr;
        }

      /*===============================================*/
      /* Return if only one agenda is being reordered. */
      /*===============================================*/

      if (! allModules) return;
     }
  }

/****************************************************/
/* GetNumberOfActivations: Returns the value of the */
/*   total number of activations on all agendas.    */
/****************************************************/
globle unsigned long GetNumberOfActivations(
  void *theEnv)
  {  
   return(AgendaData(theEnv)->NumberOfActivations); 
  }

/******************************************************/
/* RefreshCommand: H/L Command for refreshing a rule. */
/*   Syntax: (refresh <defrule-name>)                 */
/******************************************************/
globle void RefreshCommand(
  void *theEnv)
  {
   char *ruleName;
   void *rulePtr;

   /*===========================*/
   /* Get the name of the rule. */
   /*===========================*/

   ruleName = GetConstructName(theEnv,"refresh","rule name");
   if (ruleName == NULL) return;

   /*===============================*/
   /* Determine if the rule exists. */
   /*===============================*/

   rulePtr = EnvFindDefrule(theEnv,ruleName);
   if (rulePtr == NULL)
     {
      CantFindItemErrorMessage(theEnv,"defrule",ruleName);
      return;
     }

   /*===================*/
   /* Refresh the rule. */
   /*===================*/

   EnvRefresh(theEnv,rulePtr);
  }

/************************************************************/
/* EnvRefresh: Refreshes a defrule. Activations of the rule */
/*   that have already been fired are added to the agenda.  */
/************************************************************/
globle BOOLEAN EnvRefresh(
  void *theEnv,
  void *theRule)
  {
   struct defrule *rulePtr;
   struct partialMatch *listOfMatches;
   
   /*====================================*/
   /* Refresh each disjunct of the rule. */
   /*====================================*/

   for (rulePtr = (struct defrule *) theRule;
        rulePtr != NULL;
        rulePtr = rulePtr->disjunct)
     {
      /*================================*/
      /* Check each partial match that  */
      /* satisfies the LHS of the rule. */
      /*================================*/

      for (listOfMatches = rulePtr->lastJoin->beta;
           listOfMatches != NULL;
           listOfMatches = listOfMatches->next)
        {
         /*=======================================================*/
         /* If the partial match is associated with an activation */
         /* (which it should always be) and it isn't associated   */
         /* with a not CE that still has matches, then place a    */
         /* new activation on the agenda if this partial match    */
         /* doesn't have an activation associated with it.        */
         /*=======================================================*/

         if ((listOfMatches->activationf) && (! listOfMatches->counterf))
           {
            if (listOfMatches->binds[listOfMatches->bcount].gm.theValue == NULL)
              { AddActivation(theEnv,rulePtr,listOfMatches); }
           }
        }
     }

   return(TRUE);
  }

#if DYNAMIC_SALIENCE

/**********************************************/
/* RefreshAgendaCommand: H/L access routine   */
/*   for the refresh-agenda command.          */
/**********************************************/
globle void RefreshAgendaCommand(
  void *theEnv)
  {
   int numArgs, error;
   struct defmodule *theModule;

   /*==============================================*/
   /* This function can have at most one argument. */
   /*==============================================*/

   if ((numArgs = EnvArgCountCheck(theEnv,"refresh-agenda",NO_MORE_THAN,1)) == -1) return;

   /*===============================================================*/
   /* If a module name is specified, then the agenda of that module */
   /* is refreshed. Otherwise, the agenda of the current module is  */
   /* refreshed.                                                    */
   /*===============================================================*/

   if (numArgs == 1)
     {
      theModule = GetModuleName(theEnv,"refresh-agenda",1,&error);
      if (error) return;
     }
   else
     { theModule = ((struct defmodule *) EnvGetCurrentModule(theEnv)); }

   /*===============================================*/
   /* Refresh the agenda of the appropriate module. */
   /*===============================================*/

   EnvRefreshAgenda(theEnv,theModule);
  }

/**************************************/
/* EnvRefreshAgenda: C access routine */
/*   for the refresh-agenda command.  */
/**************************************/
globle void EnvRefreshAgenda(
  void *theEnv,
  void *vTheModule)
  {
   struct activation *theActivation;
   struct defmodule *theModule = (struct defmodule *) vTheModule;
   BOOLEAN oldValue;
   int allModules = FALSE;
   
   /*==========================*/
   /* Save the current module. */
   /*==========================*/

   SaveCurrentModule(theEnv);

   /*=============================================*/
   /* If the module specified is a NULL pointer,  */
   /* then every module has its agenda refreshed. */
   /*=============================================*/

   if (theModule == NULL)
     {
      allModules = TRUE;
      theModule = (struct defmodule *) EnvGetNextDefmodule(theEnv,NULL);
     }

   /*=======================================================*/
   /* Remember the current setting for salience evaluation. */
   /* To perform the refresh, the when activated setting is */
   /* used to recompute the salience values.                */
   /*=======================================================*/

   oldValue = EnvGetSalienceEvaluation(theEnv);
   EnvSetSalienceEvaluation(theEnv,WHEN_ACTIVATED);

   /*========================*/
   /* Refresh the agenda(s). */
   /*========================*/

   for (;
        theModule != NULL;
        theModule = (struct defmodule *) EnvGetNextDefmodule(theEnv,theModule))
     {
      /*=========================================*/
      /* Change the current module to the module */
      /* of the agenda being refreshed.          */
      /*=========================================*/

      EnvSetCurrentModule(theEnv,(void *) theModule);

      /*================================================================*/
      /* Recompute the salience values for the current module's agenda. */
      /*================================================================*/

      for (theActivation = (struct activation *) EnvGetNextActivation(theEnv,NULL);
           theActivation != NULL;
           theActivation = (struct activation *) EnvGetNextActivation(theEnv,theActivation))
        { theActivation->salience = EvaluateSalience(theEnv,theActivation->theRule); }

      /*======================================================*/
      /* Reorder the agenda based on the new salience values. */
      /*======================================================*/

      EnvReorderAgenda(theEnv,theModule);

      /*===============================================*/
      /* Return if only one agenda is being refreshed. */
      /*===============================================*/

      if (! allModules)
        {
         EnvSetSalienceEvaluation(theEnv,oldValue);
         RestoreCurrentModule(theEnv);
         return;
        }
     }

   /*==========================================*/
   /* Restore the salience evaluation setting. */
   /*==========================================*/

   EnvSetSalienceEvaluation(theEnv,oldValue);

   /*=============================*/
   /* Restore the current module. */
   /*=============================*/

   RestoreCurrentModule(theEnv);
  }

/*********************************************************/
/* SetSalienceEvaluationCommand: H/L Command for setting */
/*   the salience evaluation behavior.                   */
/*   Syntax: (set-salience-evaluation-behavior <symbol>) */
/*********************************************************/
globle SYMBOL_HN *SetSalienceEvaluationCommand(
  void *theEnv)
  {
   DATA_OBJECT argPtr;
   char *argument, *oldValue;

   /*==================================================*/
   /* Get the current setting for salience evaluation. */
   /*==================================================*/

   oldValue = SalienceEvaluationName(EnvGetSalienceEvaluation(theEnv));

   /*=========================================*/
   /* This function expects a single argument */
   /* which must be a symbol.                 */
   /*=========================================*/

   if (EnvArgCountCheck(theEnv,"set-salience-evaluation",EXACTLY,1) == -1)
     { return((SYMBOL_HN *) EnvAddSymbol(theEnv,oldValue)); }

   if (EnvArgTypeCheck(theEnv,"set-salience-evaluation",1,SYMBOL,&argPtr) == FALSE)
     { return((SYMBOL_HN *) EnvAddSymbol(theEnv,oldValue)); }

   /*=============================================================*/
   /* The allowed symbols to pass as an argument to this function */
   /* are when-defined, when-activated, and every-cycle.          */
   /*=============================================================*/

   argument = DOToString(argPtr);

   if (strcmp(argument,"when-defined") == 0)
     { EnvSetSalienceEvaluation(theEnv,WHEN_DEFINED); }
   else if (strcmp(argument,"when-activated") == 0)
     { EnvSetSalienceEvaluation(theEnv,WHEN_ACTIVATED); }
   else if (strcmp(argument,"every-cycle") == 0)
     { EnvSetSalienceEvaluation(theEnv,EVERY_CYCLE); }
   else
     {
      ExpectedTypeError1(theEnv,"set-salience-evaluation",1,
      "symbol with value when-defined, when-activated, or every-cycle");
      return((SYMBOL_HN *) EnvAddSymbol(theEnv,oldValue));
     }

   /*=================================================*/
   /* Return the old setting for salience evaluation. */
   /*=================================================*/

   return((SYMBOL_HN *) EnvAddSymbol(theEnv,oldValue));
  }

/*********************************************************/
/* GetSalienceEvaluationCommand: H/L Command for getting */
/*   the salience evaluation behavior.                   */
/*   Syntax: (get-salience-evaluation-behavior)          */
/*********************************************************/
globle SYMBOL_HN *GetSalienceEvaluationCommand(
  void *theEnv)
  {
   EnvArgCountCheck(theEnv,"get-salience-evaluation",EXACTLY,0);

   return((SYMBOL_HN *) EnvAddSymbol(theEnv,SalienceEvaluationName(EnvGetSalienceEvaluation(theEnv))));
  }

/*****************************************************************/
/* SalienceEvaluationName: Given the integer value corresponding */
/*   to a specified salience evaluation behavior, returns a      */
/*   character string of the behavior's name.                    */
/*****************************************************************/
static char *SalienceEvaluationName(
  int strategy)
  {
   char *sname;

   switch (strategy)
     {
      case WHEN_DEFINED:
        sname = "when-defined";
        break;
      case WHEN_ACTIVATED:
        sname = "when-activated";
        break;
      case EVERY_CYCLE:
        sname = "every-cycle";
        break;
      default:
        sname = "unknown";
        break;
     }

   return(sname);
  }

/****************************************************************/
/* EnvGetSalienceEvaluation: Returns the value of current type  */
/*  of salience evaluation (e.g., when defined, when activated, */
/*  or every cycle).                                            */
/****************************************************************/
globle BOOLEAN EnvGetSalienceEvaluation(
  void *theEnv)
  {   
   return(AgendaData(theEnv)->SalienceEvaluation); 
  }

/***********************************************/
/* EnvSetSalienceEvaluation: Sets the value of */
/*   the current type of salience evaluation.  */
/***********************************************/
globle BOOLEAN EnvSetSalienceEvaluation(
  void *theEnv,
  int value)
  {
   int ov;

   ov = AgendaData(theEnv)->SalienceEvaluation;
   AgendaData(theEnv)->SalienceEvaluation = value;
   return(ov);
  }

/*****************************************************************/
/* EvaluateSalience: Returns the salience value of the specified */
/*   defrule. If salience evaluation is currently set to         */
/*   when-defined, then the current value of the rule's salience */
/*   is returned. Otherwise the salience expression associated   */
/*   with the rule is reevaluated, the value is stored as the    */
/*   rule's current salience, and it is then returned.           */
/*****************************************************************/
static int EvaluateSalience(
  void *theEnv,
  void *vPtr)
  {
   struct defrule *rPtr = (struct defrule *) vPtr;
   DATA_OBJECT salienceValue;
   int salience;

  /*==================================================*/
  /* If saliences are only being evaluated when rules */
  /* are defined, then just return the last salience  */
  /* value evaluated for the rule.                    */
  /*==================================================*/

  if (EnvGetSalienceEvaluation(theEnv) == WHEN_DEFINED)
    { return(rPtr->salience); }

  /*=================================================================*/
  /* If the rule's salience value was defined as an integer constant */
  /* (i.e., not an expression or global variable which could change  */
  /* on reevaluation), then just return the salience value computed  */
  /* for the rule when it was defined.                               */
  /*=================================================================*/

  if (rPtr->dynamicSalience == NULL) return(rPtr->salience);

  /*====================================================*/
  /* Reevaluate the rule's salience. If an error occurs */
  /* during evaluation, print an error message.         */
  /*====================================================*/

  SetEvaluationError(theEnv,FALSE);
  if (EvaluateExpression(theEnv,rPtr->dynamicSalience,&salienceValue))
    {
     SalienceInformationError(theEnv,"defrule",ValueToString(rPtr->header.name));
     return(rPtr->salience);
    }

  /*========================================*/
  /* The salience value must be an integer. */
  /*========================================*/

  if (salienceValue.type != INTEGER)
    {
     SalienceNonIntegerError(theEnv);
     SalienceInformationError(theEnv,"defrule",ValueToString(rPtr->header.name));
     SetEvaluationError(theEnv,TRUE);
     return(rPtr->salience);
    }

  /*==========================================*/
  /* The salience value must fall between the */
  /* minimum and maximum allowed values.      */
  /*==========================================*/

  salience = (int) ValueToLong(salienceValue.value);

  if ((salience > MAX_DEFRULE_SALIENCE) || (salience < MIN_DEFRULE_SALIENCE))
    {
     SalienceRangeError(theEnv,MIN_DEFRULE_SALIENCE,MAX_DEFRULE_SALIENCE);
     SetEvaluationError(theEnv,TRUE);
     SalienceInformationError(theEnv,"defrule",ValueToString(((struct defrule *) rPtr)->header.name));
     return(rPtr->salience);
    }

  /*===================================*/
  /* Store the new salience value with */
  /* the rule and return this value.   */
  /*===================================*/

  rPtr->salience = salience;
  return(rPtr->salience);
 }

#endif /* DYNAMIC_SALIENCE */

#if DEBUGGING_FUNCTIONS

/***********************************************/
/* AgendaCommand: Prints out the agenda of the */
/*   rules that are ready to fire.             */
/*   Syntax: (agenda)                          */
/***********************************************/
globle void AgendaCommand(
  void *theEnv)
  {
   int numArgs, error;
   struct defmodule *theModule;

   /*==============================================*/
   /* This function can have at most one argument. */
   /*==============================================*/

   if ((numArgs = EnvArgCountCheck(theEnv,"agenda",NO_MORE_THAN,1)) == -1) return;

   /*===============================================================*/
   /* If a module name is specified, then the agenda of that module */
   /* is displayed. Otherwise, the agenda of the current module is  */
   /* displayed.                                                    */
   /*===============================================================*/

   if (numArgs == 1)
     {
      theModule = GetModuleName(theEnv,"agenda",1,&error);
      if (error) return;
     }
   else
     { theModule = ((struct defmodule *) EnvGetCurrentModule(theEnv)); }

   /*===============================================*/
   /* Display the agenda of the appropriate module. */
   /*===============================================*/

   EnvAgenda(theEnv,WDISPLAY,theModule);
  }

#endif /* DEBUGGING_FUNCTIONS */

#endif /* DEFRULE_CONSTRUCT */

