#!/usr/bin/perl -s
#
# -r Show reused names
# -y=yyyy Show only up to specified year

use strict;


$::r   = defined($::r)?(($::r>0)?$::r:1):0;
$::y   = defined($::y)?(($::y>0)?$::y:1):0;
$::rl  = defined($::rl)?(($::rl>0)?$::rl:1):0;
$::pl  = defined($::pl)?(($::pl>0)?$::pl:1):0;

my($hNameDate, $hNamePL, $hNameRL) = GetNameList($::r);

if($::rl)
{
    if($::rl > 1)
    {
        ShowUptoVersion($::rl, $hNameRL);
    }
    else
    {
        my @rls = uniq(values(%$hNameRL));
        @rls    = sort(@rls);

        foreach my $rl (@rls)
        {
            print "# RL$rl\n";
            ShowUptoVersion($::y, $hNameRL);
            print "\n";
        }
    }
}
elsif($::pl)
{
    if($::pl > 1)
    {
        ShowUptoVersion($::pl, $hNamePL);
    }
    else
    {
        my @pls = uniq(values(%$hNamePL));
        @pls    = sort(@pls);

        foreach my $pl (@pls)
        {
            print "# PL$pl\n";
            ShowUptoVersion($::y, $hNamePL);
            print "\n";
        }
    }
}
else   # Assume $::y
{
    if($::y > 1)
    {
        ShowUptoVersion($::y, $hNameDate);
    }
    else
    {
        my @dates = uniq(values(%$hNameDate));
        @dates = sort(@dates);

        foreach my $date (@dates)
        {
            print "# $date\n";
            ShowUptoVersion($date, $hNameDate);
            print "\n";
        }
    }
}



sub ShowUptoVersion
{
    my ($cutoff, $hHash) = @_;
    
    foreach my $name (keys %$hHash)
    {
        if($$hHash{$name} <= $cutoff)
        {
            print "$name\n";
        }
    }
}


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
        next if(/^#/);  # Skip comments

        # Get the fields from the record
        my ($id, $name, $pl, $rl, $date) = split(/,/);

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
