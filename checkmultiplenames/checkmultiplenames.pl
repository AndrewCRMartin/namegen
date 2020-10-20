#!/usr/bin/perl
#*************************************************************************
#
#   Program:    
#   File:       
#   
#   Version:    
#   Date:       
#   Function:   
#   
#   Copyright:  (c) Prof. Andrew C. R. Martin, UCL, 2020
#   Author:     Prof. Andrew C. R. Martin
#   Address:    Institute of Structural and Molecular Biology
#               Division of Biosciences
#               University College
#               Gower Street
#               London
#               WC1E 6BT
#   EMail:      andrew@bioinf.org.uk
#               
#*************************************************************************
#
#   This program is not in the public domain, but it may be copied
#   according to the conditions laid out in the accompanying file
#   COPYING.DOC
#
#   The code may be modified as required, but any modifications must be
#   documented so that the person responsible can be identified. If 
#   someone else breaks this code, I don't want to be blamed for code 
#   that does not work! 
#
#   The code may not be sold commercially or included as part of a 
#   commercial product except as described in the file COPYING.DOC.
#
#*************************************************************************
#
#   Description:
#   ============
#
#   Takes a file in the format
#
#   reqnum name
#   ...
#
#   Analyzes each name in turn against the current names list adding the
#   new name to the list after it has been analyzed. (Note that the actual
#   oldnames file is not modified.)
#
#*************************************************************************
#
#   Usage:
#   ======
#
#   ./checkmultiplenames.pl oldNamesFile newNamesFile
#
#*************************************************************************
#
#   Revision History:
#   =================
#
#*************************************************************************
use strict;

# Add the path of the executable to the library path
use FindBin;
use lib $FindBin::Bin;
use Cwd qw(abs_path);

my $binDir     = $FindBin::Bin;
my $exeDir     = abs_path("$FindBin::Bin/../");
$::abcheckname = "$exeDir/abcheckname";

my $oldNamesFile = shift(@ARGV);
my $tmpNamesFile = Initialize($oldNamesFile);

while(<>)
{
    chomp;
    ProcessName($tmpNamesFile, $_);
}

unlink($tmpNamesFile);

#*************************************************************************
sub ProcessName
{
    my($tmpNamesFile, $nameLine) = @_;
    my($reqID, $name) = split(/\s+/, $nameLine);

    my $pParams = "-t 82";
    my $sParams = "-t 77";
#    $pParams = '';
#    $sParams = '';
    
    print "\n*** Processing $reqID : $name ***\n\n";
    
    my $exe = "$::abcheckname -p -n $tmpNamesFile $pParams $name";
    system "$exe";

    $exe = "$::abcheckname -s -n $tmpNamesFile $sParams $name";
    system "$exe";

    print "\n[Defaults - Phonetics: 85.5; LetterSimilarity: 80.7]\n";
    
    print "\n-----------------------------------------------\n";

    UpdateNamesFile($tmpNamesFile, $name);
}

#*************************************************************************
sub UpdateNamesFile
{
    my($namesFile, $name) = @_;

    `echo $name >>$namesFile`;
}

#*************************************************************************
sub Initialize
{
    my ($oldNamesFile) = @_;
    my $tmpNamesFile = "/var/tmp/cmn_$$" . "_" . time;
    `cp $oldNamesFile $tmpNamesFile`;
    return($tmpNamesFile);
}
