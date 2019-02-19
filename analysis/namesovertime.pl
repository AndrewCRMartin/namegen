#!/usr/bin/perl -s

use strict;

UsageDie() if(defined($::h));

$::u = defined($::u)?(($::u>0)?$::u:1):0;
$::y = defined($::y)?(($::y>0)?$::y:1):0;
$::r = defined($::r)?(($::r>0)?$::r:1):0;
$::p = defined($::p)?(($::p>0)?$::p:1):0;
$::c = defined($::c)?1:0;

$::flag = 999999;

my($hNameDate, $hNamePL, $hNameRL) = GetNameList($::u);

if($::r)
{
    if($::r > 1)
    {
        ShowUptoVersion($::r, $hNameRL, $::c);
    }
    else
    {
        my @rls = uniq(values(%$hNameRL));
        @rls    = sort {$a <=> $b} @rls;

        foreach my $rl (@rls)
        {
            if($rl != $::flag)
            {
                print "# RL$rl\n";
                ShowUptoVersion($rl, $hNameRL, $::c);
                print "\n";
            }
        }
    }
}
elsif($::p)
{
    if($::p > 1)
    {
        ShowUptoVersion($::p, $hNamePL, $::c);
    }
    else
    {
        my @pls = uniq(values(%$hNamePL));
        @pls    = sort {$a <=> $b} @pls;

        foreach my $pl (@pls)
        {
            print "# PL$pl\n";
            ShowUptoVersion($pl, $hNamePL, $::c);
            print "\n";
        }
    }
}
else   # Assume $::y
{
    if($::y > 1)
    {
        ShowUptoVersion($::y, $hNameDate, $::c);
    }
    else
    {
        my @dates = uniq(values(%$hNameDate));
        @dates = sort {$a <=> $b} @dates;

        foreach my $date (@dates)
        {
            print "# $date\n";
            ShowUptoVersion($date, $hNameDate, $::c);
            print "\n";
        }
    }
}


##########################################################################
sub ShowUptoVersion
{
    my ($cutoff, $hHash, $cummulative) = @_;
    
    foreach my $name (keys %$hHash)
    {
        if($cummulative)
        {
            if($$hHash{$name} <= $cutoff)
            {
                print "$name\n";
            }
        }
        else
        {
            if($$hHash{$name} == $cutoff)
            {
                print "$name\n";
            }
        }
    }
}


##########################################################################
sub uniq
{
    my(@array) = @_;
    my %uniqItems = ();
    foreach my $item (@array)
    {
        $uniqItems{$item} = 1;
    }
    @array = keys(%uniqItems);
    return(@array);
}

##########################################################################
sub GetNameList
{
    my ($showReuse) = @_;
    
    my %nameDate = ();
    my %namePL   = ();
    my %nameRL   = ();
    
    while(<>)
    {
        chomp;
        s/^\s+//; # Removed leading spaces
        s/\s+$//; # Removed trailing spaces
        s/#.*//;  # Remove comments
        next if(!length()); # SKip blank lines

        # Get the fields from the record
        my ($id, $name, $pl, $rl, $date) = split(/,/);

        # Replace an RL of 'objection' with $::flag so it never gets printed
        $rl = $::flag if($rl eq 'objection');

        # Remove the letter from the date
        $date =~ s/[a-zA-Z]//g;
        
        # Clean up the name to extract only the mab part
        my $cleanName = CleanName($name);

        if(defined($nameDate{$cleanName}))
        {
            if($showReuse)
            {
                printf("$name (PL:$pl RL:$rl DATE:$date) was $cleanName (PL:$namePL{$cleanName} RL:$nameRL{$cleanName} DATE:$nameDate{$cleanName})\n");
            }
        }
        else
        {
            $nameDate{$cleanName} = $date;
            $namePL{$cleanName}   = $pl;
            $nameRL{$cleanName}   = $rl;
        }
    }
    return(\%nameDate, \%namePL, \%nameRL);
}

##########################################################################
sub CleanName
{
    my($name) = @_;
    my @fields = split(/\s+/, $name);
    foreach my $field (@fields)
    {
        return($field) if($field =~ /mab$/);
    }
    return('');
}

##########################################################################
sub UsageDie
{
    print <<__EOF;

namesbytime V1.1 (c) 2018-19 Dr. Andrew C.R. Martin, UCL

Usage: namesbytime [-c][-u][-p[=pl]|-r[=rl]|-y[=year]] names.csv
       -c      Cummulative
       -u      Print info on names Used before
       -p      Print by Proposed list
       -p=pl   Print only up to specified Proposed list
       -r      Print by Recommended list
       -p=rl   Print only up to specified Recommended list
       -y      Print by Year [default]
       -y=year Print only up to specified Year

Prints names from (or up to, if -c specified) the specified Proposed
list, Recommended list or year.  If no list or year is specified then
they are all printed in order.

The CSV input file is in the format
   Entry,Name,PL,RL,Year
For example:
   6560,sevirumab,66,32,1991b
Letters in the year field are ignored and comments (starting with #) and 
blank lines are removed

__EOF
    
    exit 0;
}
