/************************************************************************/
/* Unused */
BOOL CheckNameForConflicts(char *newName, FILE *namesFp,
                           char *conflictName, int maxConflictName,
                           unsigned int *conflictType, FILE *out)
{
   BOOL verbose = FALSE;
   char buffer[MAXBUFF];
   *conflictType = 0;
   
   while(fgets(buffer, MAXBUFF, namesFp))
   {
      BOOL BoomerOK = TRUE,
         PhoneticsOK = TRUE;
      
      TERMINATE(buffer);

      if(!strcmp(newName, buffer))
         return(TRUE);

#ifdef BOOMER
      if(!blReadMDM("data/boomer.mat"))
      {
         fprintf(stderr,"Error: Can't read boomer matrix\n");
         exit(1);
      }
      BoomerOK = CheckBoomer(newName, buffer, verbose, out);
      blFreeMDM();
#endif

#ifdef PHONETICS
      if(!blReadMDM("data/kondrak.mat"))
      {
         fprintf(stderr,"Error: Can't read phonetics matrix\n");
         exit(1);
      }
      PhoneticsOK = CheckPhonetics(newName, buffer, verbose, out);
      blFreeMDM();
#endif

      if(!BoomerOK)   *conflictType |= TYPE_BOOMER;
      if(!PhoneticsOK) *conflictType |= TYPE_PHONETICS;
      
      if((!BoomerOK) || (!PhoneticsOK))
      {
         strncpy(conflictName, buffer, maxConflictName);
         return(FALSE);
      }
   }

   return(TRUE);
}

