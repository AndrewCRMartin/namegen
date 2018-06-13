#!/usr/bin/perl -s
#
# -r Show reused names
# -y=yyyy Show only up to specified year

use strict;



$::r = defined($::r)?1:0;
$::y = defined($::y)?$::y:0;

my($hNameDate, $hNamePL, $hNameRL) = GetNameList($::r);

if($::y)
{
    ShowUptoYear($::y, $hNameDate);
}
else
{
    my @dates = uniq(values(%$hNameDate));
    @dates = sort(@dates);

    foreach my $date (@dates)
    {
        print "# $date\n";
        ShowUptoYear($date, $hNameDate);
        print "\n";
    }
}

sub ShowUptoYear
{
    my ($date, $hNameDate) = @_;
    
    foreach my $name (keys %$hNameDate)
    {
        if($$hNameDate{$name} <= $date)
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
