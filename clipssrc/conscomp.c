   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.22  06/15/04            */
   /*                                                     */
   /*              CONSTRUCT COMPILER MODULE              */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides core routines for the constructs-to-c   */
/*   command.                                                */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*      Brian L. Donnell                                     */
/*      Barry Cameron                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

#define _CONSCOMP_SOURCE_

#include "setup.h"

#if CONSTRUCT_COMPILER && (! RUN_TIME)

#include <stdio.h>
#define _STDIO_INCLUDED_
#include <stdlib.h>
#include <string.h>

#include "symbol.h"
#include "memalloc.h"
#include "constant.h"
#include "exprnpsr.h"
#include "cstrccom.h"
#include "constrct.h"
#include "argacces.h"
#include "cstrncmp.h"
#include "router.h"
#include "sysdep.h"
#include "utility.h"
#include "modulcmp.h"
#include "envrnmnt.h"

#if DEFRULE_CONSTRUCT
#include "network.h"
#endif

#if DEFFUNCTION_CONSTRUCT
#include "dffnxcmp.h"
#endif

#if DEFTEMPLATE_CONSTRUCT
#include "tmpltcmp.h"
#endif

#if DEFGLOBAL_CONSTRUCT
#include "globlcmp.h"
#endif

#if DEFGENERIC_CONSTRUCT
#include "genrccmp.h"
#endif

#if OBJECT_SYSTEM
#include "objcmp.h"
#endif

#include "conscomp.h"

/***************/
/* DEFINITIONS */
/***************/

#define FSIZE 80

/**********************************************/
/* CONSTRUCT CODES DEFINITIONS: The codes F,  */
/*   I, B, S, E, P, L, and C are not included */
/*   because those are already taken.         */
/*                                            */
/*   B: BitMap hash nodes                     */
/*   C: Constraint hash nodes                 */
/*   E: Expression hash nodes                 */
/*   F: Float hash nodes                      */
/*   I: Integer hash nodes                    */
/*   L: Bitmaps                               */
/*   P: Functions                             */
/*   S: Symbol hash nodes                     */
/**********************************************/

#define PRIMARY_CODES   "ADGHJKMNOQRTUVWXYZ"
#define PRIMARY_LEN     18
#define SECONDARY_CODES "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define SECONDARY_LEN   26

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   void                               ConstructsToCCommand(void *);
   static int                         ConstructsToC(void *,char *,int,int);
   static void                        WriteFunctionExternDeclarations(void *,FILE *);
   static int                         FunctionsToCode(void *theEnv,char *);
   static int                         WriteInitializationFunction(void *,char *);
   static void                        DumpExpression(void *,struct expr *);
   static void                        MarkConstruct(void *,struct constructHeader *,void *);
   static void                        HashedExpressionsToCode(void *);
   static void                        DeallocateConstructCompilerData(void *);

/**********************************************************/
/* InitializeConstructCompilerData: Allocates environment */
/*    data for the constructs-to-c command.               */
/**********************************************************/
globle void InitializeConstructCompilerData(
  void *theEnv)
  {
   AllocateEnvironmentData(theEnv,CONSTRUCT_COMPILER_DATA,sizeof(struct constructCompilerData),DeallocateConstructCompilerData);
   
   ConstructCompilerData(theEnv)->MaxIndices = 2000;
   ConstructCompilerData(theEnv)->CodeGeneratorCount = 0;
  }
  
/************************************************************/
/* DeallocateConstructCompilerData: Deallocates environment */
/*    data for the constructs-to-c command.                 */
/************************************************************/
static void DeallocateConstructCompilerData(
  void *theEnv)
  {
   struct CodeGeneratorItem *tmpPtr, *nextPtr;
   int i;
   
   tmpPtr = ConstructCompilerData(theEnv)->ListOfCodeGeneratorItems;
   while (tmpPtr != NULL)
     {
      nextPtr = tmpPtr->next;

      for (i = 0; i < tmpPtr->arrayCount ; i++)
        { rm(theEnv,tmpPtr->arrayNames[i],strlen(tmpPtr->arrayNames[i]) + 1); }
        
      if (tmpPtr->arrayCount != 0)
        { rm(theEnv,tmpPtr->arrayNames,sizeof(char *) * tmpPtr->arrayCount); }
      
      rtn_struct(theEnv,CodeGeneratorItem,tmpPtr);
      tmpPtr = nextPtr;
     }
  }

/**********************************************/
/* ConstructsToCCommand: H/L access routine   */
/*   for the constructs-to-c command.         */
/**********************************************/
globle void ConstructsToCCommand(
  void *theEnv)
  {
   char *fileName;
   DATA_OBJECT theArg;
   int argCount;
   int id, max;
#if VAX_VMS || IBM_MSC || IBM_TBC || IBM_ICB || IBM_ZTC || IBM_SC
   int i;
#endif

   /*============================================*/
   /* Check for appropriate number of arguments. */
   /*============================================*/

   if ((argCount = EnvArgRangeCheck(theEnv,"constructs-to-c",2,3)) == -1) return;

   /*====================================================*/
   /* Get the name of the file in which to place C code. */
   /*====================================================*/

   if (EnvArgTypeCheck(theEnv,"constructs-to-c",1,SYMBOL_OR_STRING,&theArg) == FALSE)
     { return; }

   fileName = DOToString(theArg);

   /*================================*/
   /* File names for the VAX and IBM */
   /* PCs can't contain a period.    */
   /*================================*/

#if VAX_VMS || IBM_MSC || IBM_TBC || IBM_ICB || IBM_ZTC || IBM_SC
   for (i = 0 ; *(fileName+i) ; i++)
     {
      if (*(fileName+i) == '.')
        {
         PrintErrorID(theEnv,"CONSCOMP",1,FALSE);
         EnvPrintRouter(theEnv,WERROR,"Invalid file name ");
         EnvPrintRouter(theEnv,WERROR,fileName);
         EnvPrintRouter(theEnv,WERROR," contains \'.\'\n");
         return;
        }
      }
#endif

   /*===========================================*/
   /* If the base file name is greater than 3   */
   /* characters, issue a warning that the file */
   /* name lengths may exceed what is allowed   */
   /* under some operating systems.             */
   /*===========================================*/

   if (((int) strlen(fileName)) > 3)
     {
      PrintWarningID(theEnv,"CONSCOMP",1,FALSE);
      EnvPrintRouter(theEnv,WWARNING,"Base file name exceeds 3 characters.\n");
      EnvPrintRouter(theEnv,WWARNING,"  This may cause files to be overwritten if file name length\n");
      EnvPrintRouter(theEnv,WWARNING,"  is limited on your platform.\n");
     }

   /*====================================*/
   /* Get the runtime image ID argument. */
   /*====================================*/

   if (EnvArgTypeCheck(theEnv,"constructs-to-c",2,INTEGER,&theArg) == FALSE)
     { return; }

   id = DOToInteger(theArg);
   if (id < 0)
     {
      ExpectedTypeError1(theEnv,"constructs-to-c",2,"positive integer");
      return;
     }

   /*===========================================*/
   /* Get the maximum number of data structures */
   /* to store per file argument (if supplied). */
   /*===========================================*/

   if (argCount == 3)
     {
      if (EnvArgTypeCheck(theEnv,"constructs-to-c",3,INTEGER,&theArg) == FALSE)
        { return; }

      max = DOToInteger(theArg);

      if (max < 0)
        {
         ExpectedTypeError1(theEnv,"constructs-to-c",3,"positive integer");
         return;
        }
     }
   else
     { max = 10000; }

   /*============================*/
   /* Call the driver routine to */
   /* generate the C code.       */
   /*============================*/

   ConstructsToC(theEnv,fileName,id,max);
  }

/***************************************/
/* ConstructsToC: C access routine for */
/*   the constructs-to-c command.      */
/***************************************/
static int ConstructsToC(
  void *theEnv,
  char *fileName,
  int theImageID,
  int max)
  {
   char fname[FSIZE];
   int fileVersion;
   struct CodeGeneratorItem *cgPtr;

   /*===============================================*/
   /* Set the global MaxIndices variable indicating */
   /* the maximum number of data structures to save */
   /* in each file.                                 */
   /*===============================================*/

   ConstructCompilerData(theEnv)->MaxIndices = max;

   /*==================================*/
   /* Call the list of functions to be */
   /* executed before generating code. */
   /*==================================*/

   for (cgPtr = ConstructCompilerData(theEnv)->ListOfCodeGeneratorItems;
        cgPtr != NULL;
        cgPtr = cgPtr->next)
     { if (cgPtr->beforeFunction != NULL) (*cgPtr->beforeFunction)(theEnv); }

   /*=================================================*/
   /* Do a periodic cleanup without using heuristics  */
   /* to get rid of as much garbage as possible so    */
   /* that it isn't written out as C data structures. */
   /*=================================================*/

   PeriodicCleanup(theEnv,FALSE,FALSE);

   /*=====================================*/
   /* Initialize some global information. */
   /*=====================================*/

   ConstructCompilerData(theEnv)->FilePrefix = fileName;
   ConstructCompilerData(theEnv)->ImageID = theImageID;
   ConstructCompilerData(theEnv)->ExpressionFP = NULL;
   ConstructCompilerData(theEnv)->ExpressionVersion = 1;
   ConstructCompilerData(theEnv)->ExpressionHeader = TRUE;
   ConstructCompilerData(theEnv)->ExpressionCount = 0;

   /*=====================================================*/
   /* Open a header file for dumping general information. */
   /*=====================================================*/

   sprintf(fname,"%s.h",fileName);
   if ((ConstructCompilerData(theEnv)->HeaderFP = GenOpen(fname,"w")) == NULL)
     {
      OpenErrorMessage(theEnv,"constructs-to-c",fname);
      return(0);
     }

   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"#ifndef _CONSTRUCT_COMPILER_HEADER_\n");
   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"#define _CONSTRUCT_COMPILER_HEADER_\n\n");

   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"#include <stdio.h>\n");
   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"#include \"setup.h\"\n");
   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"#include \"expressn.h\"\n");
   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"#include \"extnfunc.h\"\n");
   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"#include \"%s\"\n",API_HEADER);
   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"\n#define VS (void *)\n");
   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"\n");

   /*=========================================================*/
   /* Give extern declarations for user and system functions. */
   /*=========================================================*/

   WriteFunctionExternDeclarations(theEnv,ConstructCompilerData(theEnv)->HeaderFP);

   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"\n#endif\n\n");
   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"/****************************/\n");
   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"/* EXTERN ARRAY DEFINITIONS */\n");
   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"/****************************/\n\n");

   /*============================================*/
   /* Open a file for dumping fixup information. */
   /*============================================*/

   sprintf(fname,"%s_init.c",fileName);
   if ((ConstructCompilerData(theEnv)->FixupFP = GenOpen(fname,"w")) == NULL)
     {
      OpenErrorMessage(theEnv,"constructs-to-c",fname);
      return(0);
     }

   fprintf(ConstructCompilerData(theEnv)->FixupFP,"#include \"%s.h\"\n",fileName);
   fprintf(ConstructCompilerData(theEnv)->FixupFP,"\n");
   
   fprintf(ConstructCompilerData(theEnv)->FixupFP,"\n");
   fprintf(ConstructCompilerData(theEnv)->FixupFP,"/**********************************/\n");
   fprintf(ConstructCompilerData(theEnv)->FixupFP,"/* CONSTRUCT IMAGE FIXUP FUNCTION */\n");
   fprintf(ConstructCompilerData(theEnv)->FixupFP,"/**********************************/\n");

   fprintf(ConstructCompilerData(theEnv)->FixupFP,"\nvoid FixupCImage_%d(\n",ConstructCompilerData(theEnv)->ImageID);
   fprintf(ConstructCompilerData(theEnv)->FixupFP,"  void *theEnv)\n");
   fprintf(ConstructCompilerData(theEnv)->FixupFP,"  {\n");

   /*==================================*/
   /* Generate code for atomic values, */
   /* function definitions, hashed     */
   /* expressions, and constructs.     */
   /*==================================*/

   AtomicValuesToCode(theEnv,fileName);

   FunctionsToCode(theEnv,fileName);

   HashedExpressionsToCode(theEnv);

   ConstraintsToCode(theEnv,fileName,4,ConstructCompilerData(theEnv)->HeaderFP,ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->MaxIndices);

   /*===============================*/
   /* Call each code generator item */
   /* for the various constructs.   */
   /*===============================*/

   fileVersion = 5;
   for (cgPtr = ConstructCompilerData(theEnv)->ListOfCodeGeneratorItems;
        cgPtr != NULL;
        cgPtr = cgPtr->next)
     {
      if (cgPtr->generateFunction != NULL)
        {
         (*cgPtr->generateFunction)(theEnv,fileName,fileVersion,ConstructCompilerData(theEnv)->HeaderFP,ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->MaxIndices);
         fileVersion++;
        }
     }

   /*=========================================*/
   /* Restore the atomic data bucket values   */
   /* (which were set to an index reference). */
   /*=========================================*/

   RestoreAtomicValueBuckets(theEnv);

   /*============================*/
   /* Close the expression file. */
   /*============================*/

   if (ConstructCompilerData(theEnv)->ExpressionFP != NULL)
     {
      fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"};\n");
      GenClose(ConstructCompilerData(theEnv)->ExpressionFP);
     }

   /*=======================*/
   /* Close the fixup file. */
   /*=======================*/

   if (ConstructCompilerData(theEnv)->FixupFP != NULL)
     {
      fprintf(ConstructCompilerData(theEnv)->FixupFP,"  };\n");
      GenClose(ConstructCompilerData(theEnv)->FixupFP);
     }

   /*====================================*/
   /* Write the initialization function. */
   /*====================================*/

   WriteInitializationFunction(theEnv,fileName);

   /*========================*/
   /* Close the header file. */
   /*========================*/

   GenClose(ConstructCompilerData(theEnv)->HeaderFP);

   /*==================================================*/
   /* Return TRUE to indicate that the constructs-to-c */
   /* command was successfully executed.               */
   /*==================================================*/

   return(TRUE);
  }

/*******************************************************/
/* WriteFunctionExternDeclarations: Loop through the   */
/*   list of function definitions and generates extern */
/*   declarations for them in the specified file.      */
/*******************************************************/
static void WriteFunctionExternDeclarations(
  void *theEnv,
  FILE *fp)
  {
   struct FunctionDefinition *theFunction;

   fprintf(fp,"\n");
   fprintf(fp,"/************************************/\n");
   fprintf(fp,"/* EXTERNAL FUNCTION DEFINITIONS    */\n");
   fprintf(fp,"/************************************/\n\n");

   for (theFunction = GetFunctionList(theEnv);
        theFunction != NULL;
        theFunction = theFunction->next)
     {
      fprintf(fp,"extern ");
      switch(theFunction->returnValueType)
        {
         case 'i':
         case 'b':
           fprintf(fp,"int ");
           break;

         case 'l':
           fprintf(fp,"long ");
           break;

         case 'f':
           fprintf(fp,"float ");
           break;

         case 'd':
           fprintf(fp,"double ");
           break;

         case 'w':
         case 's':
         case 'o':
           fprintf(fp,"SYMBOL_HN *");
           break;

         case 'c':
           fprintf(fp,"char ");
           break;

         case 'a':
         case 'x':
           fprintf(fp,"void * ");
           break;

         case 'v':
         case 'm':
         case 'u':
         case 'n':
         case 'j':
         case 'k':
           fprintf(fp,"void ");
           break;

         default:
           SystemError(theEnv,"CONSCOMP",1);
           break;
        }

      fprintf(fp,"%s(",theFunction->actualFunctionName);
      
      switch(theFunction->returnValueType)
        {
         case 'i':
         case 'b':
         case 'l':
         case 'f':
         case 'd':
         case 'w':
         case 's':
         case 'o':
         case 'c':
         case 'a':
         case 'x':
         case 'v':
           if (theFunction->environmentAware) 
             { fprintf(fp,"void *"); }
           else
             { fprintf(fp,"void"); }
           break;

         case 'm':
         case 'u':
         case 'n':
         case 'j':
         case 'k':
           if (theFunction->environmentAware) 
             { fprintf(fp,"void *,DATA_OBJECT_PTR_ARG"); }
           else
             { fprintf(fp,"DATA_OBJECT_PTR_ARG"); }
           break;
        }

      fprintf(fp,");\n");
     }
  }

/****************************************************/
/* FunctionsToCode: Generates C code to represent   */
/*   the function declaration data structures (used */
/*   to declare system and user defined functions). */
/****************************************************/
static int FunctionsToCode(
  void *theEnv,
  char *fileName)
  {
   short i = 0;
   FILE *fp;
   int version = 1;
   int newHeader = TRUE;
   struct FunctionDefinition *fctnPtr;

   /*=============================*/
   /* Assign a reference index to */
   /* each of the functions.      */
   /*=============================*/

   for (fctnPtr = GetFunctionList(theEnv);
        fctnPtr != NULL;
        fctnPtr = fctnPtr->next)
     { fctnPtr->bsaveIndex = i++; }

   /*=======================================*/
   /* Create the file in which to store the */
   /* function definition data structures.  */
   /*=======================================*/

   if ((fp = NewCFile(theEnv,fileName,2,version,FALSE)) == NULL)
     { return(0); }

   /*===============================================*/
   /* Construct the definition of the function list */
   /* from the definitions of the functions.        */
   /*===============================================*/

   fprintf(fp,"\n\n");
   fprintf(fp,"/************************************/\n");
   fprintf(fp,"/* FUNCTION LIST DEFINITION         */\n");
   fprintf(fp,"/************************************/\n\n");

   i = 1;
   fctnPtr = GetFunctionList(theEnv);
   while (fctnPtr != NULL)
     {
      if (newHeader)
        {
         fprintf(fp,"struct FunctionDefinition P%d_%d[] = {\n",ConstructCompilerData(theEnv)->ImageID,version);
         fprintf(ConstructCompilerData(theEnv)->HeaderFP,"extern struct FunctionDefinition P%d_%d[];\n",ConstructCompilerData(theEnv)->ImageID,version);
         newHeader = FALSE;
        }

      fprintf(fp,"{");
      PrintSymbolReference(theEnv,fp,fctnPtr->callFunctionName);
      fprintf(fp,",\"%s\",",fctnPtr->actualFunctionName);
      fprintf(fp,"'%c',",fctnPtr->returnValueType);
      fprintf(fp,"PTIF %s,",fctnPtr->actualFunctionName);
      fprintf(fp,"NULL,");
      if (fctnPtr->restrictions != NULL) fprintf(fp,"\"%s\",",fctnPtr->restrictions);
      else fprintf(fp,"NULL,");
      fprintf(fp,"0,0,%d,0,",(fctnPtr->environmentAware ? 1 : 0));
      PrintFunctionReference(theEnv,fp,fctnPtr->next);

      i++;
      fctnPtr = fctnPtr->next;
      if ((i > ConstructCompilerData(theEnv)->MaxIndices) || (fctnPtr == NULL))
        {
         fprintf(fp,"}};\n");
         GenClose(fp);
         i = 1;
         version++;
         if (fctnPtr != NULL)
           {
            if ((fp = NewCFile(theEnv,fileName,2,version,FALSE)) == NULL) return(0);
            newHeader = TRUE;
           }
        }
      else
        { fprintf(fp,"},\n"); }
     }

   return(TRUE);
  }

/************************************************************/
/* PrintFunctionReference: Writes the C code representation */
/*   of a pointer to a function definition data structure.  */
/************************************************************/
globle void PrintFunctionReference(
  void *theEnv,
  FILE *fp,
  struct FunctionDefinition *funcPtr)
  {
   if (funcPtr == NULL) fprintf(fp,"NULL");
   else
      fprintf(fp,"&P%d_%d[%d]",ConstructCompilerData(theEnv)->ImageID,
                                  (funcPtr->bsaveIndex / ConstructCompilerData(theEnv)->MaxIndices) + 1,
                                   funcPtr->bsaveIndex % ConstructCompilerData(theEnv)->MaxIndices);
  }

/******************************************/
/* WriteInitializationFunction: Generates */
/*   the C initialization function for    */
/*   this constructs-to-c module.         */
/******************************************/
static int WriteInitializationFunction(
  void *theEnv,
  char *fileName)
  {
   char fname[FSIZE];
   FILE *fp;
   struct CodeGeneratorItem *cgPtr;

   /*===============================*/
   /* Open the initialization file. */
   /*===============================*/

   sprintf(fname,"%s.c",fileName);
   if ((fp = GenOpen(fname,"w")) == NULL)
     {
      OpenErrorMessage(theEnv,"constructs-to-c",fname);
      return(FALSE);
     }

   /*=====================================*/
   /* Write out #includes and prototypes. */
   /*=====================================*/

   fprintf(fp,"#include \"%s.h\"\n",fileName);
   fprintf(fp,"\n");
   fprintf(fp,"#include \"utility.h\"\n");
   fprintf(fp,"#include \"generate.h\"\n");
   fprintf(fp,"#include \"envrnmnt.h\"\n");
   fprintf(fp,"#include \"expressn.h\"\n");
   fprintf(fp,"#include \"extnfunc.h\"\n");
   fprintf(fp,"#include \"objrtmch.h\"\n");
   fprintf(fp,"#include \"rulebld.h\"\n\n");

   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"   void *InitCImage_%d(void);\n",ConstructCompilerData(theEnv)->ImageID);
   fprintf(ConstructCompilerData(theEnv)->HeaderFP,"   void FixupCImage_%d(void *);\n",ConstructCompilerData(theEnv)->ImageID);

   /*============================================*/
   /* Begin writing the initialization function. */
   /*============================================*/

   fprintf(fp,"\n");
   fprintf(fp,"/*******************************************/\n");
   fprintf(fp,"/* CONSTRUCT IMAGE INITIALIZATION FUNCTION */\n");
   fprintf(fp,"/*******************************************/\n");

   fprintf(fp,"\nvoid *InitCImage_%d()\n",ConstructCompilerData(theEnv)->ImageID);
   fprintf(fp,"  {\n");
   fprintf(fp,"   static void *theEnv = NULL;\n\n");
   fprintf(fp,"   if (theEnv != NULL) return(NULL);\n\n");
   fprintf(fp,"   theEnv = CreateEnvironment();\n\n");   

   fprintf(fp,"   Clear();\n");
   fprintf(fp,"   PeriodicCleanup(theEnv,TRUE,FALSE);\n");
   fprintf(fp,"   SetSymbolTable(theEnv,sht%d);\n",ConstructCompilerData(theEnv)->ImageID);
   fprintf(fp,"   SetFloatTable(theEnv,fht%d);\n",ConstructCompilerData(theEnv)->ImageID);
   fprintf(fp,"   SetIntegerTable(theEnv,iht%d);\n",ConstructCompilerData(theEnv)->ImageID);
   fprintf(fp,"   SetBitMapTable(theEnv,bmht%d);\n",ConstructCompilerData(theEnv)->ImageID);
   fprintf(fp,"   RefreshSpecialSymbols(theEnv);\n");
   fprintf(fp,"   InstallFunctionList(theEnv,P%d_1);\n\n",ConstructCompilerData(theEnv)->ImageID);
   fprintf(fp,"   InitExpressionPointers(theEnv);\n");
   fprintf(fp,"   FixupCImage_%d(theEnv);\n\n",ConstructCompilerData(theEnv)->ImageID);

   /*==========================================*/
   /* Write construct specific initialization. */
   /*==========================================*/

   cgPtr = ConstructCompilerData(theEnv)->ListOfCodeGeneratorItems;
   while (cgPtr != NULL)
     {
      if (cgPtr->initFunction != NULL)
        {
         (*cgPtr->initFunction)(theEnv,fp,ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->MaxIndices);
         fprintf(fp,"\n");
        }
      cgPtr = cgPtr->next;
     }

   /*================================*/
   /* Close the initialization file. */
   /*================================*/

   fprintf(fp,"   return(theEnv);\n");
   fprintf(fp,"  }\n");

   GenClose(fp);

   /*========================================*/
   /* Return TRUE to indicate initialization */
   /* file was successfully written.         */
   /*========================================*/

   return(TRUE);
  }


/**************************************************/
/* NewCFile: Opens a new file for writing C code. */
/**************************************************/
globle FILE *NewCFile(
  void *theEnv,
  char *fileName,
  int id,
  int version,
  int reopenOldFile)
  {
   char fname[FSIZE];
   FILE *newFP;

   sprintf(fname,"%s%d_%d.c",fileName,id,version);

   newFP = GenOpen(fname,(char *) (reopenOldFile ? "a" : "w"));

   if (newFP == NULL)
     {
      OpenErrorMessage(theEnv,"constructs-to-c",fname);
      return(NULL);
     }

   if (reopenOldFile == FALSE)
     {
      fprintf(newFP,"#include \"%s.h\"\n",fileName);
      fprintf(newFP,"\n");
     }

   return(newFP);
  }

/**********************************************************/
/* HashedExpressionsToCode: Traverses the expression hash */
/*   table and calls ExpressionToCode to write the C      */
/*   code representation to a file of every expression in */
/*   the table.                                           */
/**********************************************************/
static void HashedExpressionsToCode(
  void *theEnv)
  {
   unsigned i;
   EXPRESSION_HN *exphash;

   for (i = 0; i < EXPRESSION_HASH_SIZE; i++)
     {
      for (exphash = ExpressionData(theEnv)->ExpressionHashTable[i];
           exphash != NULL;
           exphash = exphash->next)
        {
         exphash->bsaveID = ConstructCompilerData(theEnv)->ExpressionCount + (ConstructCompilerData(theEnv)->MaxIndices * ConstructCompilerData(theEnv)->ExpressionVersion);
         ExpressionToCode(theEnv,NULL,exphash->exp);
        }
     }
  }

/*****************************************************/
/* PrintHashedExpressionReference: Writes the C code */
/*   representation of a pointer to an expression    */
/*   stored in the expression hash table.            */
/*****************************************************/
globle void PrintHashedExpressionReference(
  void *theEnv,
  FILE *theFile,
  struct expr *theExpression,
  int imageID,
  int maxIndices)
  {
   long theIDValue;

   if (theExpression == NULL)
     { fprintf(theFile,"NULL"); }
   else
     {
      theIDValue = HashedExpressionIndex(theEnv,theExpression);

      fprintf(theFile,"&E%d_%ld[%ld]",
                      imageID,
                      theIDValue / maxIndices,
                      theIDValue % maxIndices);
     }
  }

/**************************************************************/
/* ExpressionToCode: Writes the C code reference of a pointer */
/*   to an expression and then calls DumpExpression to write  */
/*   the C code for the expression to the expression file.    */
/**************************************************************/
globle int ExpressionToCode(
  void *theEnv,
  FILE *fp,
  struct expr *exprPtr)
  {
   /*========================================*/
   /* Print the reference to the expression. */
   /*========================================*/

   if (exprPtr == NULL)
     {
      if (fp != NULL) fprintf(fp,"NULL");
      return(FALSE);
     }
   else if (fp != NULL)
     { fprintf(fp,"&E%d_%d[%ld]",ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->ExpressionVersion,ConstructCompilerData(theEnv)->ExpressionCount); }

   /*==================================================*/
   /* Create a new expression code file, if necessary. */
   /*==================================================*/

   if (ConstructCompilerData(theEnv)->ExpressionHeader == TRUE)
     {
      if ((ConstructCompilerData(theEnv)->ExpressionFP = NewCFile(theEnv,ConstructCompilerData(theEnv)->FilePrefix,3,ConstructCompilerData(theEnv)->ExpressionVersion,FALSE)) == NULL)
        { return(-1); }

      fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"struct expr E%d_%d[] = {\n",ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->ExpressionVersion);
      fprintf(ConstructCompilerData(theEnv)->HeaderFP,"extern struct expr E%d_%d[];\n",ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->ExpressionVersion);
      ConstructCompilerData(theEnv)->ExpressionHeader = FALSE;
     }
   else
     { fprintf(ConstructCompilerData(theEnv)->ExpressionFP,",\n"); }

   /*===========================*/
   /* Dump the expression code. */
   /*===========================*/

   DumpExpression(theEnv,exprPtr);

   /*=========================================*/
   /* Close the expression file if necessary. */
   /*=========================================*/

   if (ConstructCompilerData(theEnv)->ExpressionCount >= ConstructCompilerData(theEnv)->MaxIndices)
     {
      ConstructCompilerData(theEnv)->ExpressionCount = 0;
      ConstructCompilerData(theEnv)->ExpressionVersion++;
      fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"};\n");
      GenClose(ConstructCompilerData(theEnv)->ExpressionFP);
      ConstructCompilerData(theEnv)->ExpressionFP = NULL;
      ConstructCompilerData(theEnv)->ExpressionHeader = TRUE;
     }

   /*==========================================*/
   /* Return TRUE to indicate the expression   */
   /* reference and expression data structures */
   /* were succcessfully written to the file.  */
   /*==========================================*/

   return(TRUE);
  }

/**********************************************************/
/* DumpExpression: Writes the C code representation of an */
/*   expression data structure to the expression file.    */
/**********************************************************/
static void DumpExpression(
  void *theEnv,
  struct expr *exprPtr)
  {
   while (exprPtr != NULL)
     {
      fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"{");
      fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"%d,",exprPtr->type);
      fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"VS ");
      switch (exprPtr->type)
        {
         case FCALL:
           PrintFunctionReference(theEnv,ConstructCompilerData(theEnv)->ExpressionFP,(struct FunctionDefinition *) exprPtr->value);
           break;

         case INTEGER:
           PrintIntegerReference(theEnv,ConstructCompilerData(theEnv)->ExpressionFP,(INTEGER_HN *) exprPtr->value);
           break;

         case FLOAT:
           PrintFloatReference(theEnv,ConstructCompilerData(theEnv)->ExpressionFP,(FLOAT_HN *) exprPtr->value);
           break;

         case PCALL:
#if DEFFUNCTION_CONSTRUCT
           PrintDeffunctionReference(theEnv,ConstructCompilerData(theEnv)->ExpressionFP,(DEFFUNCTION *) exprPtr->value,
                                     ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->MaxIndices);
#else
           fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL");
#endif
           break;

         case GCALL:
#if DEFGENERIC_CONSTRUCT
           PrintGenericFunctionReference(theEnv,ConstructCompilerData(theEnv)->ExpressionFP,(DEFGENERIC *) exprPtr->value,
                                         ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->MaxIndices);
#else
           fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL");
#endif
           break;

         case DEFTEMPLATE_PTR:
#if DEFTEMPLATE_CONSTRUCT
           DeftemplateCConstructReference(theEnv,ConstructCompilerData(theEnv)->ExpressionFP,exprPtr->value,ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->MaxIndices);
#else
           fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL");
#endif
           break;

         case DEFGLOBAL_PTR:
#if DEFGLOBAL_CONSTRUCT
           DefglobalCConstructReference(theEnv,ConstructCompilerData(theEnv)->ExpressionFP,exprPtr->value,ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->MaxIndices);
#else
           fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL");
#endif
           break;

         case DEFCLASS_PTR:
#if OBJECT_SYSTEM
           PrintClassReference(theEnv,ConstructCompilerData(theEnv)->ExpressionFP,(DEFCLASS *) exprPtr->value,ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->MaxIndices);
#else
           fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL");
#endif
           break;

          case FACT_ADDRESS:
#if DEFTEMPLATE_CONSTRUCT
           fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL");
           fprintf(ConstructCompilerData(theEnv)->FixupFP,
                   "   E%d_%d[%ld].value = &FactData(theEnv)->DummyFact;\n",
                   ConstructCompilerData(theEnv)->ImageID,
                   ConstructCompilerData(theEnv)->ExpressionVersion,
                   ConstructCompilerData(theEnv)->ExpressionCount);
#else
           fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL");
#endif
           break;

         case INSTANCE_ADDRESS:
#if OBJECT_SYSTEM
           fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL");
           fprintf(ConstructCompilerData(theEnv)->FixupFP,
                   "   E%d_%d[%ld].value = &InstanceData(theEnv)->DummyInstance;\n",
                   ConstructCompilerData(theEnv)->ImageID,
                   ConstructCompilerData(theEnv)->ExpressionVersion,
                   ConstructCompilerData(theEnv)->ExpressionCount);
#else
           fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL");
#endif
           break;

         case STRING:
         case SYMBOL:
         case INSTANCE_NAME:
         case GBL_VARIABLE:
           PrintSymbolReference(theEnv,ConstructCompilerData(theEnv)->ExpressionFP,(SYMBOL_HN *) exprPtr->value);
           break;

         case RVOID:
           fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL");
           break;

         default:
           if (EvaluationData(theEnv)->PrimitivesArray[exprPtr->type] == NULL)
             { fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL"); }
           else if (EvaluationData(theEnv)->PrimitivesArray[exprPtr->type]->bitMap)
             { PrintBitMapReference(theEnv,ConstructCompilerData(theEnv)->ExpressionFP,(BITMAP_HN *) exprPtr->value); }
           else
             { fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL"); }
           break;
        }

      fprintf(ConstructCompilerData(theEnv)->ExpressionFP,",");

      ConstructCompilerData(theEnv)->ExpressionCount++;
      if (exprPtr->argList == NULL)
        { fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL,"); }
      else
        {
         fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"&E%d_%d[%ld],",ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->ExpressionVersion,
                                                       ConstructCompilerData(theEnv)->ExpressionCount);
        }

      if (exprPtr->nextArg == NULL)
        { fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"NULL}"); }
      else
        {
         fprintf(ConstructCompilerData(theEnv)->ExpressionFP,"&E%d_%d[%ld]}",ConstructCompilerData(theEnv)->ImageID,ConstructCompilerData(theEnv)->ExpressionVersion,
                              ConstructCompilerData(theEnv)->ExpressionCount + ExpressionSize(exprPtr->argList));
        }

      if (exprPtr->argList != NULL)
        {
         fprintf(ConstructCompilerData(theEnv)->ExpressionFP,",\n");
         DumpExpression(theEnv,exprPtr->argList);
        }

      exprPtr = exprPtr->nextArg;
      if (exprPtr != NULL) fprintf(ConstructCompilerData(theEnv)->ExpressionFP,",\n");
     }
  }

/***********************************************/
/* ConstructsToCCommandDefinition: Initializes */
/*   the constructs-to-c command.              */
/***********************************************/
globle void ConstructsToCCommandDefinition(
  void *theEnv)
  {
   EnvDefineFunction2(theEnv,"constructs-to-c",'v',
                   PTIEF ConstructsToCCommand,
                   "ConstructsToCCommand", "23*kii");
  }

/*********************************************************/
/* AddCodeGeneratorItem: Adds another code generator     */
/*   item to the list of items for which code is         */
/*   generated bythe constructs-to-c function. Typically */
/*   each construct has its own code generator item.     */
/*********************************************************/
globle struct CodeGeneratorItem *AddCodeGeneratorItem(
  void *theEnv,
  char *name,
  int priority,
  void (*beforeFunction)(void *),
  void (*initFunction)(void *,FILE *,int,int),
  int (*generateFunction)(void *,char *,int,FILE *,int,int),
  int arrayCount)
  {
   struct CodeGeneratorItem *newPtr, *currentPtr, *lastPtr = NULL;
   register int i;
   char theBuffer[3];

   /*======================================*/
   /* Create the code generator item data  */
   /* structure and initialize its values. */
   /*======================================*/

   newPtr = get_struct(theEnv,CodeGeneratorItem);

   newPtr->name = name;
   newPtr->beforeFunction = beforeFunction;
   newPtr->initFunction = initFunction;
   newPtr->generateFunction = generateFunction;
   newPtr->priority = priority;
   newPtr->arrayCount = arrayCount;

   /*================================================*/
   /* Create the primary and secondary codes used to */
   /* provide names for the C data structure arrays. */
   /* (The maximum number of arrays is currently     */
   /* limited to 47.                                 */
   /*================================================*/

   if (arrayCount != 0)
     {
      if ((arrayCount + ConstructCompilerData(theEnv)->CodeGeneratorCount) > (PRIMARY_LEN + SECONDARY_LEN))
        {
         SystemError(theEnv,"CONSCOMP",2);
         EnvExitRouter(theEnv,EXIT_FAILURE);
        }

      newPtr->arrayNames = (char **) gm2(theEnv,(sizeof(char *) * arrayCount));

      for (i = 0 ; i < arrayCount ; i++)
        {
         if (ConstructCompilerData(theEnv)->CodeGeneratorCount < PRIMARY_LEN)
           { sprintf(theBuffer,"%c",PRIMARY_CODES[ConstructCompilerData(theEnv)->CodeGeneratorCount]); }
         else
           { sprintf(theBuffer,"%c_",SECONDARY_CODES[ConstructCompilerData(theEnv)->CodeGeneratorCount - PRIMARY_LEN]); }
         ConstructCompilerData(theEnv)->CodeGeneratorCount++;
         newPtr->arrayNames[i] = (char *) gm2(theEnv,(strlen(theBuffer) + 1));
         strcpy(newPtr->arrayNames[i],theBuffer);
        }
     }
   else
     { newPtr->arrayNames = NULL; }

   /*===========================================*/
   /* Add the new item in the appropriate place */
   /* in the code generator item list.          */
   /*===========================================*/

   if (ConstructCompilerData(theEnv)->ListOfCodeGeneratorItems == NULL)
     {
      newPtr->next = NULL;
      ConstructCompilerData(theEnv)->ListOfCodeGeneratorItems = newPtr;
      return(newPtr);
     }

   currentPtr = ConstructCompilerData(theEnv)->ListOfCodeGeneratorItems;
   while ((currentPtr != NULL) ? (priority < currentPtr->priority) : FALSE)
     {
      lastPtr = currentPtr;
      currentPtr = currentPtr->next;
     }

   if (lastPtr == NULL)
     {
      newPtr->next = ConstructCompilerData(theEnv)->ListOfCodeGeneratorItems;
      ConstructCompilerData(theEnv)->ListOfCodeGeneratorItems = newPtr;
     }
   else
     {
      newPtr->next = currentPtr;
      lastPtr->next = newPtr;
     }

   /*=========================*/
   /* Return a pointer to the */
   /* code generator item.    */
   /*=========================*/

   return(newPtr);
  }

/************************************************************/
/* CloseFileIfNeeded: Determines if a C file to which data  */
/*   structures have been written should be closed. The     */
/*   file is closed either when all data structures of      */
/*   that specific type are written to files or the maximum */
/*   number of array entries for a single file has been     */
/*   exceeded.                                              */
/************************************************************/
globle FILE *CloseFileIfNeeded(
  void *theEnv,
  FILE *theFile,
  int *theCount,
  int *arrayVersion,
  int maxIndices,
  int *canBeReopened,
  struct CodeGeneratorFile *codeFile)
  {
   /*==========================================*/
   /* If the maximum number of entries for the */
   /* file hasn't been exceeded, then...       */
   /*==========================================*/

   if (*theCount < maxIndices)
     {
      /*====================================*/
      /* If the file can be reopened later, */
      /* close it. Otherwise, keep it open. */
      /*====================================*/

      if (canBeReopened != NULL)
        {
         *canBeReopened = TRUE;
         GenClose(theFile);
         return(NULL);
        }

      return(theFile);
     }

   /*===========================================*/
   /* Otherwise, the number of entries allowed  */
   /* in a file has been reached. Indicate that */
   /* the file can't be reopened.               */
   /*===========================================*/

   if (canBeReopened != NULL)
     { *canBeReopened = FALSE; }

   /*===============================================*/
   /* If the file is closed, then we need to reopen */
   /* it to print the final closing right brace.    */
   /*===============================================*/

   if (theFile == NULL)
     {
      if ((canBeReopened == NULL) || (codeFile == NULL))
        {
         SystemError(theEnv,"CONSCOMP",3);
         EnvExitRouter(theEnv,EXIT_FAILURE);
        }

      if (codeFile->filePrefix == NULL)
        { return(NULL); }

      theFile = NewCFile(theEnv,codeFile->filePrefix,codeFile->id,codeFile->version,TRUE);
      if (theFile == NULL)
        {
         SystemError(theEnv,"CONSCOMP",4);
         EnvExitRouter(theEnv,EXIT_FAILURE);
        }
     }

   /*================================*/
   /* Print the final closing brace. */
   /*================================*/

   fprintf(theFile,"};\n");
   GenClose(theFile);

   /*============================================*/
   /* Update index values for subsequent writing */
   /* of data structures to files.               */
   /*============================================*/

   *theCount = 0;
   (*arrayVersion)++;

   /*=========================*/
   /* Return NULL to indicate */
   /* the file is closed.     */
   /*=========================*/

   return(NULL);
  }

/**************************************************************/
/* OpenFileIfNeeded: Determines if a C file to which data  */
/*   structures have been written should be closed. The     */
/*   file is closed either when all data structures of      */
/*   that specific type are written to files or the maximum */
/*   number of array entries for a single file has been     */
/*   exceeded.                                              */
/******************************************************************/
globle FILE *OpenFileIfNeeded(
  void *theEnv,
  FILE *theFile,
  char *fileName,
  int fileID,
  int imageID,
  int *fileCount,
  int arrayVersion,
  FILE *headerFP,
  char *structureName,
  char *structPrefix,
  int reopenOldFile,
  struct CodeGeneratorFile *codeFile)
  {
   char arrayName[80];
   char *newName;
   int newID, newVersion;

   /*===========================================*/
   /* If a file is being reopened, use the same */
   /* version number, name, and ID as before.   */
   /*===========================================*/

   if (reopenOldFile)
     {
      if (codeFile == NULL)
        {
         SystemError(theEnv,"CONSCOMP",5);
         EnvExitRouter(theEnv,EXIT_FAILURE);
        }

      newName = codeFile->filePrefix;
      newID = codeFile->id;
      newVersion = codeFile->version;
     }

   /*=====================================================*/
   /* Otherwise, use the specified version number, name,  */
   /* and ID. If the appropriate argument is supplied,    */
   /* remember these values for later reopening the file. */
   /*=====================================================*/

   else
     {
      newName = fileName;
      newVersion = *fileCount;
      newID = fileID;

      if (codeFile != NULL)
        {
         codeFile->version = newVersion;
         codeFile->filePrefix = newName;
         codeFile->id = newID;
        }
     }

   /*=========================================*/
   /* If the file is already open, return it. */
   /*=========================================*/

   if (theFile != NULL)
     {
      fprintf(theFile,",\n");
      return(theFile);
     }

   /*================*/
   /* Open the file. */
   /*================*/

   if ((theFile = NewCFile(theEnv,newName,newID,newVersion,reopenOldFile)) == NULL)
     { return(NULL); }

   /*=========================================*/
   /* If this is the first time the file has  */
   /* been opened, write out the beginning of */
   /* the array variable definition.          */
   /*=========================================*/

   if (reopenOldFile == FALSE)
     {
      (*fileCount)++;
      sprintf(arrayName,"%s%d_%d",structPrefix,imageID,arrayVersion);

#if SHORT_LINK_NAMES
      if (strlen(arrayName) > 6)
        {
         PrintWarningID(theEnv,"CONSCOMP",2,FALSE);
         EnvPrintRouter(theEnv,WWARNING,"Array name ");
         EnvPrintRouter(theEnv,WWARNING,arrayName);
         EnvPrintRouter(theEnv,WWARNING,"exceeds 6 characters in length.\n");
         EnvPrintRouter(theEnv,WWARNING,"   This variable may be indistinguishable from another by the linker.\n");
        }
#endif
      fprintf(theFile,"%s %s[] = {\n",structureName,arrayName);
      fprintf(headerFP,"extern %s %s[];\n",structureName,arrayName);
     }
   else
     { fprintf(theFile,",\n"); }

   /*==================*/
   /* Return the file. */
   /*==================*/

   return(theFile);
  }

/*************************************************/
/* MarkConstructBsaveIDs: Mark all occurences of */
/*  a specific construct with a unique ID.       */
/*************************************************/
globle void MarkConstructBsaveIDs(
  void *theEnv,
  int constructModuleIndex)
  {
   long theCount = 0;

   DoForAllConstructs(theEnv,MarkConstruct,constructModuleIndex,FALSE,&theCount);
  }

/*************************************************************/
/* MarkConstruct: Sets the bsaveID for a specific construct. */
/*  Used with the MarkConstructBsaveIDs function to mark all */
/*  occurences of a specific construct with a unique ID.     */
/*************************************************************/
#if IBM_TBC
#pragma argsused
#endif
static void MarkConstruct(
  void *theEnv,
  struct constructHeader *theConstruct,
  void *vTheBuffer)
  {
   long *count = (long *) vTheBuffer;
#if MAC_MCW || IBM_MCW
#pragma unused(theEnv)
#endif

   theConstruct->bsaveID = (*count)++;
  }

/***********************************************************/
/* ConstructHeaderToCode: Writes the C code representation */
/*   of a single construct header to the specified file.   */
/***********************************************************/
globle void ConstructHeaderToCode(
  void *theEnv,
  FILE *theFile,
  struct constructHeader *theConstruct,
  int imageID,
  int maxIndices,
  int moduleCount,
  char *constructModulePrefix,
  char *constructPrefix)
  {
   /*================*/
   /* Construct Name */
   /*================*/

   fprintf(theFile,"{");

   PrintSymbolReference(theEnv,theFile,theConstruct->name);

   /*===================*/
   /* Pretty Print Form */
   /*===================*/

   fprintf(theFile,",NULL,");

   /*====================*/
   /* Construct Module */
   /*====================*/

   fprintf(theFile,"MIHS &%s%d_%d[%d],",
                   constructModulePrefix,
                   imageID,
                   (moduleCount / maxIndices) + 1,
                   moduleCount % maxIndices);

   /*==========*/
   /* Bsave ID */
   /*==========*/

   fprintf(theFile,"0,");

   /*================*/
   /* Next Construct */
   /*================*/

   if (theConstruct->next == NULL)
     { fprintf(theFile,"NULL}"); }
   else
     {
      fprintf(theFile,"CHS &%s%d_%ld[%ld]}",
                      constructPrefix,
                      imageID,
                      (theConstruct->next->bsaveID / maxIndices) + 1,
                      theConstruct->next->bsaveID % maxIndices);
     }
  }

/***********************************************************/
/* ConstructModuleToCode: Writes the C code representation */
/*   of a single construct module to the specified file.   */
/***********************************************************/
globle void ConstructModuleToCode(
  void *theEnv,
  FILE *theFile,
  struct defmodule *theModule,
  int imageID,
  int maxIndices,
  int constructIndex,
  char *constructPrefix)
  {
   struct defmoduleItemHeader *theModuleItem;

   /*======================*/
   /* Associated Defmodule */
   /*======================*/

   fprintf(theFile,"{");

   theModuleItem = (struct defmoduleItemHeader *)
                   GetModuleItem(theEnv,theModule,constructIndex);

   PrintDefmoduleReference(theEnv,theFile,theModule);

   fprintf(theFile,",");

   /*=============================*/
   /* First Construct Module Item */
   /*=============================*/

   if (theModuleItem->firstItem == NULL) fprintf(theFile,"NULL,");
   else fprintf(theFile,"CHS &%s%d_%ld[%ld],",
                        constructPrefix,
                        imageID,
                        (long) (theModuleItem->firstItem->bsaveID / maxIndices) + 1,
                        (long) theModuleItem->firstItem->bsaveID % maxIndices);

   /*============================*/
   /* Last Construct Module Item */
   /*============================*/

   if (theModuleItem->lastItem == NULL) fprintf(theFile,"NULL");
   else fprintf(theFile,"CHS &%s%d_%ld[%ld]",
                        constructPrefix,
                        imageID,
                        (long) (theModuleItem->lastItem->bsaveID / maxIndices) + 1,
                        (long) theModuleItem->lastItem->bsaveID % maxIndices);

   fprintf(theFile,"}");
  }

#else /* CONSTRUCT_COMPILER && (! RUN_TIME) */

   void                               ConstructsToCCommand(void *);

/************************************/
/* ConstructsToCCommand: Definition */
/*   for rule compiler stub.        */
/************************************/
void ConstructsToCCommand(
  void *theEnv) 
  {
#if MAC_MCW || IBM_MCW
#pragma unused(theEnv)
#endif
  }

#endif /* CONSTRUCT_COMPILER && (! RUN_TIME) */
