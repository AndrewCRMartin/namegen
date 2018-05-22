/************************************************************************/
/**

   Program:    checkname
   \file       checkname.c
   
   \version    V1.0
   \date       02.05.18   
   \brief      Checks an antibody name for possible conflicts
   
   \copyright  (c) UCL / Dr. Andrew C. R. Martin 2018
   \author     Dr. Andrew C. R. Martin
   \par
               Institute of Structural & Molecular Biology,
               University College,
               Gower Street,
               London.
               WC1E 6BT.
   \par
               andrew@bioinf.org.uk
               andrew.martin@ucl.ac.uk
               
**************************************************************************

   This program is not in the public domain, but it may be copied
   according to the conditions laid out in the accompanying file
   COPYING.DOC

   The code may be modified as required, but any modifications must be
   documented so that the person responsible can be identified.

   The code may not be sold commercially or included as part of a 
   commercial product except as described in the file COPYING.DOC.

**************************************************************************

   Description:
   ============

**************************************************************************

   Usage:
   ======

**************************************************************************

   Revision History:
   =================

*************************************************************************/
/* Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include "bioplib/SysDefs.h"
#include "bioplib/macros.h"
#include "bioplib/seq.h"

/************************************************************************/
/* Defines and macros
*/
#define MAXBUFF                       160
#define TYPE_BOUMA                    1
#define TYPE_PHONETICS                2
#define TYPE_LETTERSHAPE              3
#define DEFAULT_TYPE                  TYPE_PHONETICS
#define DEFAULT_PHONETICS_MATRIX      "abnamedata/kondrak.mat"
#define DEFAULT_BOUMA_MATRIX          "abnamedata/bouma.mat"
#define DEFAULT_LETTER_MATRIX         "abnamedata/letters.mat"
#define GAPPEN_DUMMY                  -10000
#define DEFAULT_NAMESFILE             "abnamedata/abnames.dat"
#define DEFAULT_PHONETICS_THRESHOLD   93.0
#define DEFAULT_BOUMA_THRESHOLD       95.0
#define DEFAULT_LETTERSHAPE_THRESHOLD 80.0
#define DATAENV                       "DATADIR"

/************************************************************************/
/* Globals
*/
int gUserGappen    = GAPPEN_DUMMY;
int gUserGapExtpen = GAPPEN_DUMMY;

/************************************************************************/
/* Prototypes
*/
int main(int argc, char **argv);
BOOL ParseCmdLine(int argc, char **argv, char *inParam, char *outFile,
                  int *type, BOOL *doAll, BOOL *verbose, char *namesFile,
                  REAL *printThreshold);
void Usage(void);
BOOL ProcessAllNames(char *namesFile, int type, BOOL verbose,
                     char *scoreMatrix, FILE *out);
BOOL ProcessOneName(char *name, char *namesFile, int type, BOOL verbose,
                    char *scoreMatrix, REAL printThreshold, FILE *out);
BOOL CheckNameForConflicts(char *newName, FILE *namesFp,
                           char *conflictName, int maxConflictName,
                           unsigned int *conflictType, FILE *out);
BOOL CheckBouma(char *newName, char *oldName, BOOL verbose,
                 REAL printThreshold, FILE *out);
BOOL CheckPhonetics(char *newName, char *oldName, BOOL verbose,
                    REAL printThreshold, FILE *out);
BOOL CheckLetterShape(char *newName, char *oldName, BOOL verbose,
                      REAL printThreshold, FILE *out);
REAL RunAlignment(char *newName, char *oldName,
                  int gapOpenPenalty, int gapExtensionPenalty,
                  BOOL verbose, FILE *out);
BOOL OpenStdFile(char *file, FILE **fp, char *mode);
char *FindFile(char *filename, char *envvar, BOOL *noenv);


/************************************************************************/
/*>int main(int argc, char **argv)
   -------------------------------
*//*
   Main program

-  18.05.18 Original   By: ACRM
*/
int main(int argc, char **argv)
{
   FILE *out    = stdout;
   char inParam[MAXBUFF],
        outFile[MAXBUFF];
   int  type    = DEFAULT_TYPE,
        retval  = 0;
   BOOL doAll   = FALSE,
        verbose = FALSE,
        noEnv   = FALSE;
   char *namesFile,
        scoreMatrix[MAXBUFF];
   REAL printThreshold = (REAL)(-1000.0); /* If negative, default will 
                                             be used 
                                          */

   if((namesFile = FindFile(DEFAULT_NAMESFILE, DATAENV, &noEnv))==NULL)
   {
      fprintf(stderr, "Error: names file (%s) not found.\n",
              DEFAULT_NAMESFILE);
      if(noEnv)
      {
         fprintf(stderr, "       Environment variable (%s) not set.\n",
                 DATAENV);
      }
      return(1);
   }
   
   
   if(ParseCmdLine(argc, argv, inParam, outFile, &type, &doAll, &verbose,
                   namesFile, &printThreshold))
   {
      if(!inParam[0])
      {
         Usage();
      }

      switch(type)
      {
      case TYPE_PHONETICS:
         strncpy(scoreMatrix, DEFAULT_PHONETICS_MATRIX, MAXBUFF);
         break;
      case TYPE_BOUMA:
         strncpy(scoreMatrix, DEFAULT_BOUMA_MATRIX, MAXBUFF);
         break;
      case TYPE_LETTERSHAPE:
         strncpy(scoreMatrix, DEFAULT_LETTER_MATRIX, MAXBUFF);
         break;
      default:
         fprintf(stderr,"Error: Internal error - unknown type (%d)\n",
                 type);
      }

      if(OpenStdFile(outFile, &out, "w"))
      {
         if(doAll)
         {
            ProcessAllNames(inParam, type, verbose, scoreMatrix, out);
         }
         else
         {
            ProcessOneName(inParam, namesFile, type, verbose, scoreMatrix,
                           printThreshold, out);
         }
      }
      else
      {
         fprintf(stderr,"Error: Unable to open input or output file\n");
         retval = 1;
      }
   }
   else
   {
      Usage();
   }

   FREE(namesFile);
   return(retval);
}

/************************************************************************/
/*>BOOL ParseCmdLine(int argc, char **argv, char *inParam, char *outfile,
                     int *type, BOOL *doAll, BOOL *verbose, 
                     char *namesFile, REAL *printThreshold)
   ----------------------------------------------------------------------
*//*
   \param[in]  argc           Argument count
   \param[in]  argv           Arguments
   \param[out] inParam        Compulsory argument passed on command line
   \param[out] outfile        Output filename or empty string
   \param[out] type           Type of comparison (Phonetics, Bouma or 
                              Letters)
   \param[out] doAll          Analyze the distribution in the existing 
                              words file
   \param[out] verbose        Show the alignments
   \param[out] namesFile      User-specified names file
   \param[out] printThreshold Threshold for printing matches

   Parse the command line

-  18.05.18 Original   By: ACRM
*/
BOOL ParseCmdLine(int argc, char **argv, char *inParam, char *outfile,
                  int *type, BOOL *doAll, BOOL *verbose, char *namesFile,
                  REAL *printThreshold)
{
   BOOL setType = FALSE;
   
   inParam[0] = outfile[0] = '\0';

   argc--; argv++;
   while(argc)
   {
      if(argv[0][0] == '-')
      {
         switch(argv[0][1])
         {
         case 'b':
            if(setType)
               return(FALSE);
            *type   = TYPE_BOUMA;
            setType = TRUE;
            break;
         case 'p':
            if(setType)
               return(FALSE);
            *type   = TYPE_PHONETICS;
            setType = TRUE;
            break;
         case 's':
            if(setType)
               return(FALSE);
            *type   = TYPE_LETTERSHAPE;
            setType = TRUE;
            break;
         case 'a':
            *doAll = TRUE;
            break;
         case 'v':
            *verbose = TRUE;
            break;
         case 'g':
            argc--; argv++;
            if(!argc) return(FALSE);
            if(!sscanf(argv[0], "%d", &gUserGappen)) return(FALSE);
            break;
         case 'x':
            argc--; argv++;
            if(!argc) return(FALSE);
            if(!sscanf(argv[0], "%d", &gUserGapExtpen)) return(FALSE);
            break;
         case 'n':
            argc--; argv++;
            if(!argc) return(FALSE);
            strncpy(namesFile, argv[0], MAXBUFF);
            break;
         case 't':
            argc--; argv++;
            if(!argc) return(FALSE);
            if(!sscanf(argv[0], "%lf", printThreshold)) return(FALSE);
            break;
         default:
            return(FALSE);
            break;
         }
      }
      else
      {
         /* Check there are 1 or 2 arguments left                       */
         if((argc < 1) || (argc > 2))
            return(FALSE);

         /* Copy the first (compulsory) argument                        */
         strncpy(inParam, argv[0], MAXBUFF);
         
         /* See if there is another argument left                       */
         argc--; argv++;
         if(argc)
         {
            strcpy(outfile, argv[0]);
            argc--; argv++;
         }
         
         return(TRUE);
      }
      argc--;
      argv++;
   }
   
   return(TRUE);
}

/************************************************************************/
/*>void Usage(void)
   ----------------
*//*
   Print usage message

-  18.05.18 Original   By: ACRM
*/
void Usage(void)
{
   fprintf(stderr,"\ncheckname V1.0 (c) 2018, Dr Andrew C.R. Martin\n");
   fprintf(stderr,"\nUsage: checkname [-b|-p|-s][-v][-g n][-x n][-t n]\
[-n file.names] name [file.out]]\n");
   fprintf(stderr,"       -or-\n");
   fprintf(stderr,"       checkname -a [-b|-p|-s][-v][-g n][-x n] \
file.names [file.out]\n");

   fprintf(stderr,"\n                  -a Check all used names against \
each other\n");
   fprintf(stderr,"                  -b Check the Bouma\n");
   fprintf(stderr,"                  -p Check the Kondrak phonetics \
[Default]\n");
   fprintf(stderr,"                  -s Check the Simpson letter \
similarity\n");
   fprintf(stderr,"                  -v Verbose - show alignments\n");
   fprintf(stderr,"                  -g Specify gap opening penalty\n");
   fprintf(stderr,"                  -x Specify gap extension penalty\n");
   fprintf(stderr,"                  -n Specify the file containing \
antibody names\n");
   fprintf(stderr,"                  -t Specify a threshold for printing \
as similar\n");

   fprintf(stderr,"\nCheckname is a program for comparing potential \
antibody names against\n");
   fprintf(stderr,"those already used by the WHO-INN.\n");

   fprintf(stderr,"\nThe -a option is used to compare existing names to \
look at the \n");
   fprintf(stderr,"distribution of similarity scores.\n\n");
}

/************************************************************************/
/*>BOOL ProcessAllNames(char *namesFile, int type, BOOL verbose,
                     char *scoreMatrix, FILE *out)
   -------------------------------------------------------------
*//*

   \param[in]  namesFile   Filename of file containing existing names
   \param[in]  type        Type of comparison to perform
   \param[in]  verbose     Print the alignments
   \param[in]  scoreMatrix Filename of the score matrix
   \param[in]  out         Pointer to output file

   Process the file of names comparing them all against each other to
   obtain a score distribution.

-  18.05.18 Original   By: ACRM
*/
BOOL ProcessAllNames(char *namesFile, int type, BOOL verbose,
                     char *scoreMatrix, FILE *out)
{
   char name1[MAXBUFF],
        name2[MAXBUFF];
   FILE *fp1 = NULL,
        *fp2 = NULL;
   
   if((fp1=fopen(namesFile, "r"))==NULL)
   {
      fprintf(stderr,"Error: Can't open names file for first reading \
(%s)\n",
              namesFile);
      return(FALSE);
   }
   
   if((fp2=fopen(namesFile, "r"))==NULL)
   {
      fprintf(stderr,"Error: Can't open names file for second reading \
(%s)\n",
              namesFile);
      return(FALSE);
   }

   if(!blReadMDM(scoreMatrix))
   {
      fprintf(stderr,"Error: Can't read scoring matrix (%s)\n",
              scoreMatrix);
      exit(1);
   }
   
   while(fgets(name1, MAXBUFF, fp1))
   {
      TERMINATE(name1);
      rewind(fp2);
      while(fgets(name2, MAXBUFF, fp2))
      {
         TERMINATE(name2);
         
         if(strcmp(name1, name2))
         {
            switch(type)
            {
            case TYPE_PHONETICS:
               CheckPhonetics(name1, name2, verbose, (REAL)0.0, out);
               break;
            case TYPE_BOUMA:
               CheckBouma(name1, name2, verbose, (REAL)0.0, out);
               break;
            case TYPE_LETTERSHAPE:
               CheckLetterShape(name1, name2, verbose, (REAL)0.0, out);
               break;
            default:
               fprintf(stderr,"Internal Error: Unrecognized type (%d)\n",
                       type);
               break;
            }
         }
      }
   }
   
   blFreeMDM();
   fclose(fp1);
   fclose(fp2);
   return(TRUE);
}


/************************************************************************/
/*>REAL RunAlignment(char *newName, char *oldName,
                     int gapOpenPenalty, int gapExtensionPenalty, 
                     BOOL verbose, FILE *out)
   -----------------------------------------------
*//*
   \param[in]  newName             Name to be checked
   \param[in]  oldName             Name against which to compare
   \param[in]  gapOpenPenalty      Gap opening ppenalty
   \param[in]  gapExtensionPenalty Gap extension penalty
   \param[in]  verbose             Show alignments
   \param[in]  out                 Output file pointer

   \return                         Percentage score (0.0 ... 100.0)

   Runs an alignment and calculate the alignment score. 
   Prints the alignment if required.

-  18.05.18 Original   By: ACRM
*/
REAL RunAlignment(char *newName, char *oldName,
                  int gapOpenPenalty, int gapExtensionPenalty, 
                  BOOL verbose, FILE *out)
{
   
   int  score,
        newScore,
        oldScore,
        maxAlnLen;
   char *newNameAligned = NULL,
        *oldNameAligned = NULL;
   int  alignmentLength;
   REAL finalScore = 0.0;
   
   maxAlnLen = strlen(newName) + strlen(oldName) + 1;
   if((newNameAligned = (char *)malloc(maxAlnLen * sizeof(char)))==NULL)
   {
      fprintf(stderr, "Error: no memory for storing alignment\n");
      exit(1);
   }
   if((oldNameAligned = (char *)malloc(maxAlnLen * sizeof(char)))==NULL)
   {
      fprintf(stderr, "Error: no memory for storing alignment\n");
      exit(1);
   }
   
   oldScore = blAffinealign(oldName, strlen(oldName),
                            oldName, strlen(oldName),
                            FALSE, /* verbose */
                            FALSE, /* identity */
                            gapOpenPenalty,
                            gapExtensionPenalty,
                            newNameAligned,
                            oldNameAligned,
                            &alignmentLength);
   
   newScore = blAffinealign(newName, strlen(newName),
                            newName, strlen(newName),
                            FALSE, /* verbose */
                            FALSE, /* identity */
                            gapOpenPenalty,
                            gapExtensionPenalty,
                            newNameAligned,
                            oldNameAligned,
                            &alignmentLength);

   score = blAffinealign(newName, strlen(newName),
                         oldName, strlen(oldName),
                         FALSE, /* verbose */
                         FALSE, /* identity */
                         gapOpenPenalty,
                         gapExtensionPenalty,
                         newNameAligned,
                         oldNameAligned,
                         &alignmentLength);

   finalScore = 100.0 * score / MAX(oldScore, newScore);
   
   if(verbose)
   {
      newNameAligned[alignmentLength] = '\0';
      oldNameAligned[alignmentLength] = '\0';
   
      fprintf(out, "\n%s\n", newNameAligned);
      fprintf(out, "%s\n",   oldNameAligned);
   }

   free(newNameAligned);
   free(oldNameAligned);

   return(finalScore);
}


/************************************************************************/
/*>BOOL CheckBouma(char *newName, char *oldName, BOOL verbose, 
                    REAL printThreshold, FILE *out)
   ------------------------------------------------------------
*//*
   \param[in]  newName         New name to check
   \param[in]  oldName         Old name against which to check
   \param[in]  verbose         Print the alignment
   \param[in]  printThreshold  Threshold at which to print a match
   \param[in]  out             Output file pointer

   Run the alignment and print results for Bouma comparison

-  18.05.18 Original   By: ACRM
*/
BOOL CheckBouma(char *newName, char *oldName, BOOL verbose,
                 REAL printThreshold, FILE *out)
{
   REAL score;
   int  gapOpenPenalty      = 5,
        gapExtensionPenalty = 5;

   if(gUserGappen != GAPPEN_DUMMY)
      gapOpenPenalty = gUserGappen;
   if(gUserGapExtpen != GAPPEN_DUMMY)
      gapExtensionPenalty = gUserGapExtpen;
   
   score = RunAlignment(newName, oldName, gapOpenPenalty,
                        gapExtensionPenalty, verbose, out);

   if(score > printThreshold)
      fprintf(out, "Bouma: %s %s %.2f\n", newName, oldName, score);
   
   if(score > printThreshold)
      return(FALSE);

   return(TRUE);
}

/************************************************************************/
/*>BOOL CheckPhonetics(char *newName, char *oldName, BOOL verbose, 
                       REAL printThreshold, FILE *out)
   ---------------------------------------------------------------
*//*
   \param[in]  newName         New name to check
   \param[in]  oldName         Old name against which to check
   \param[in]  verbose         Print the alignment
   \param[in]  printThreshold  Threshold at which to print a match
   \param[in]  out             Output file pointer

   Run the alignment and print results for Phonetics comparison

-  18.05.18 Original   By: ACRM
*/
BOOL CheckPhonetics(char *newName, char *oldName, BOOL verbose,
                    REAL printThreshold, FILE *out)
{
   REAL score;
   int  gapOpenPenalty      = 10,
        gapExtensionPenalty = 10;

   if(gUserGappen != GAPPEN_DUMMY)
      gapOpenPenalty = gUserGappen;
   if(gUserGapExtpen != GAPPEN_DUMMY)
      gapExtensionPenalty = gUserGapExtpen;
   
   score = RunAlignment(newName, oldName, gapOpenPenalty,
                        gapExtensionPenalty, verbose, out);

   if(score > printThreshold)
      fprintf(out, "Phonetics: %s %s %.2f\n", newName, oldName, score);
   
   if(score > printThreshold)
      return(FALSE);

   return(TRUE);
}

/************************************************************************/
/*>BOOL CheckLetterShape(char *newName, char *oldName, BOOL verbose, 
                         REAL printThreshold, FILE *out)
   -----------------------------------------------------------------
*//*
   \param[in]  newName         New name to check
   \param[in]  oldName         Old name against which to check
   \param[in]  verbose         Print the alignment
   \param[in]  printThreshold  Threshold at which to print a match
   \param[in]  out             Output file pointer

   Run the alignment and print results for letter shape comparison

-  18.05.18 Original   By: ACRM
*/
BOOL CheckLetterShape(char *newName, char *oldName, BOOL verbose,
                      REAL printThreshold, FILE *out)
{
   REAL score;
   int  gapOpenPenalty      = 200,
        gapExtensionPenalty = 200;

   if(gUserGappen != GAPPEN_DUMMY)
      gapOpenPenalty = gUserGappen;
   if(gUserGapExtpen != GAPPEN_DUMMY)
      gapExtensionPenalty = gUserGapExtpen;
   
   score = RunAlignment(newName, oldName, gapOpenPenalty,
                        gapExtensionPenalty, verbose, out);

   if(score > printThreshold)
      fprintf(out, "Lettershape: %s %s %.2f\n", newName, oldName, score);
   
   if(score > printThreshold)
      return(FALSE);

   return(TRUE);
}

/************************************************************************/
/*>BOOL OpenStdFile(char *file, FILE **fp, char *mode)
   ---------------------------------------------------
*//**

   \param[in]     *file       Filename
   \param[out]    **fp        Pointer to file pointer
   \param[in]     *mode       File read/write mode
   \return                    Success?

   Open the file if specified. fp is not modified if file is not
   specified.

-  18.05.18 Original    By: ACRM
*/
BOOL OpenStdFile(char *file, FILE **fp, char *mode)
{
   if(file!=NULL && file[0] && strcmp(file,"-"))
   {
      if((*fp = fopen(file,mode))==NULL)
      {
         fprintf(stderr,"Unable to open file: %s\n", file);
         return(FALSE);
      }
   }
      
   return(TRUE);
}


/************************************************************************/
/*>BOOL ProcessOneName(char *name, char *namesFile, int type, 
                       BOOL verbose, char *scoreMatrix, 
                       REAL printThreshold, FILE *out)
   ----------------------------------------------------------
*//*
   \param[in]  name            A name to be tested
   \param[in]  namesFile       Filename of the file containing existing
                               names
   \param[in]  type            Type of comparison
   \param[in]  verbose         Print alignments
   \param[in]  scoreMatrix     Filename of matrix file
   \param[in]  printThreshold  Threshold to print a match
   \param[in]  out             Output file handle
   \return                     Success

   Compares a single name against the existing names

-  18.05.18 Original   By: ACRM
*/
BOOL ProcessOneName(char *name, char *namesFile, int type, BOOL verbose,
                    char *scoreMatrix, REAL printThreshold, FILE *out)
{
   FILE *fp = NULL;
   char oldName[MAXBUFF];
   
   if((fp=fopen(namesFile, "r"))==NULL)
   {
      fprintf(stderr,"Error: Can't open names file (%s)\n",
              namesFile);
      return(FALSE);
   }
   
   if(!blReadMDM(scoreMatrix))
   {
      fprintf(stderr,"Error: Can't read scoring matrix (%s)\n",
              scoreMatrix);
      return(FALSE);
   }

   if(printThreshold < 0.0)
   {
      switch(type)
      {
      case TYPE_PHONETICS:
         printThreshold = DEFAULT_PHONETICS_THRESHOLD;
         break;
      case TYPE_BOUMA:
         printThreshold = DEFAULT_BOUMA_THRESHOLD;
         break;
      case TYPE_LETTERSHAPE:
         printThreshold = DEFAULT_LETTERSHAPE_THRESHOLD;
         break;
      default:
         fprintf(stderr,"Internal Error: Unrecognized type (%d)\n", type);
         break;
      }
   }
   
   while(fgets(oldName, MAXBUFF, fp))
   {
      TERMINATE(oldName);

      switch(type)
      {
      case TYPE_PHONETICS:
         CheckPhonetics(name, oldName, verbose, printThreshold, out);
         break;
      case TYPE_BOUMA:
         CheckBouma(name, oldName, verbose, printThreshold, out);
         break;
      case TYPE_LETTERSHAPE:
         CheckLetterShape(name, oldName, verbose, printThreshold, out);
         break;
      default:
         fprintf(stderr,"Internal Error: Unrecognized type (%d)\n", type);
         break;
      }
   }
   
   blFreeMDM();
   fclose(fp);
   return(TRUE);
}


/************************************************************************/
/*>char *FindFile(char *filename, char *envvar, BOOL *noenv)
   ---------------------------------------------------------
*//*
   \param[in]  filename    Basic (or complete) filename
   \param[in]  envvar      Environment variable to search
   \param[out] noenv       Indicates if the environment variable was
                           not set (may be NULL)
   \return                 Malloc'd filename or NULL if not found

   This is equivalent to blOpenFile() but just returns the file name
   rather than opening the file.

   Attempts to find a filename as specified. Returns a malloc'd copy
   of the filename if found. If this fails:

   Under UNIX/MS-DOS/Mac:
   gets the contents of the envvar environment variable and prepends
   that to the filename and tries again. If envvar was not set, noenv
   is set to TRUE and the routine returns a NULL pointer.

   Under other OSs:
   prepends the envvar string onto the filename and sees if that exists.
   If it does, the malloc'd complete path/filename is returned. 
   If not returns a NULL pointer

   The return is a malloc'd string that must be freed when finished with.

-  22.05.18  Original   By: ACRM


*/
char *FindFile(char *filename, char *envvar, BOOL *noenv)
{
   char *buffer  = NULL;
   int  nameSize = 0;
#if (unix || __unix__ || MS_WINDOWS || __unix || __MACH__ || __APPLE__)
   char *datadir = NULL;
#endif

   if(noenv != NULL) *noenv = FALSE;
   if(filename == NULL || filename[0] == '\0')
      return(NULL);

   /* Calculate space needed for filename plus environment variable     */
   nameSize = strlen(filename) + 2;
#if (unix || __unix__ || MS_WINDOWS || __unix || __MACH__ || __APPLE__)
   if((datadir = getenv(envvar)) != NULL)
   {
      nameSize += strlen(datadir);
   }
#else
   nameSize += strlen(envvar);
#endif
   
   /* Allocate space for new filename                                   */
   if((buffer=(char *)malloc(nameSize * sizeof(char)))==NULL)
      return(NULL);

   /* If the file exists as given, simply copy it and return            */
   if(access(filename, R_OK) == 0)
   {
      strncpy(buffer, filename, nameSize);
   }
   else
   {
      /* Failed, so build alternative directory/filename                */
#if (unix || __unix__ || MS_WINDOWS || __unix || __MACH__ || __APPLE__)
      if(datadir != NULL)
      {
         if(datadir[strlen(datadir)-1] == '/')
            sprintf(buffer,"%s%s",datadir,filename);
         else
            sprintf(buffer,"%s/%s",datadir,filename);
      }
      else
      {
         if(noenv != NULL) *noenv = TRUE;
         FREE(buffer);
         return(NULL);
      }
#else
      sprintf(buffer,"%s:%s",envvar,filename);
#endif

      if(access(buffer, R_OK) != 0)
      {
         FREE(buffer);
         return(NULL);
      }
   }

   return(buffer);
}
