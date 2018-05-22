/************************************************************************/
/**

   Program:    makematrix
   \file       constants.h
   
   \version    V1.0
   \date       15.05.18   
   \brief      Create a letter-sound similarity matrix
   
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
/* Table 4.28: Place */
#define BILABIAL       1.00
#define LABIODENTAL    0.95
#define DENTAL         0.90
#define ALVEOLAR       0.85
#define RETROFLEX      0.80
#define PALATOALVEOLAR 0.75
#define PALATAL        0.70
#define VELAR          0.60
#define UVULAR         0.50
#define PHARANGEAL     0.30
#define GLOTTAL        0.10
#define VELARBILABIAL  0.80 /* Mean of BILABIAL and VELAR */

/* Table 4.28: Manner */
#define STOP        1.0
#define AFFRICATIVE 0.9
#define FRICATIVE   0.8
#define APPROXIMANT 0.6
#define HIGHVOWEL   0.4
#define MIDVOWEL    0.2
#define LOWVOWEL    0.0

/* Table 4.28: High */
#define HIGH        1.0 
#define MID         0.5
#define LOW         0.0

/* Table 4.28: Back */
#define FRONT       1.0
#define CENTRAL     0.5
#define BACK        0.0

#define SALIENCE_SYLLABIC   5
#define SALIENCE_VOICE     10
#define SALIENCE_LATERAL   10
#define SALIENCE_HIGH       5
#define SALIENCE_MANNER    50
#define SALIENCE_PLACE     40
#define SALIENCE_NASAL     10
#define SALIENCE_BACK       5
#define SALIENCE_RETROFLEX 10
#define SALIENCE_ROUND      5
#define SALIENCE_LONG       1 /* Unused */
#define SALIENCE_ASPIRATED  5 /* Unused */

#define DEFAULT_CVWL 10.0
#define DEFAULT_CSUB 35.0
#define DEFAULT_CEXP 45.0
