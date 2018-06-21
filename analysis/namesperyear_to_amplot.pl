#!/usr/bin/perl -s

# -c Cummulative total
# Otherwise total per year

my $xlab = "Year";
my $ylab = "Number";

my $count = 0;
while(<>)
{
    chomp;
    s/^\s+//;
    s/\s+$//;
    if(length && !(/^#/))
    {
        my @fields = split(/\,/);
        
        my $year = $fields[4];
        $year =~ s/[a-zA-Z]//g;

        if(!defined($counts{$year}))
        {
            $counts{$year} = 1;
        }
        else
        {
            $counts{$year}++;
        }
    }
}

print <<__EOF;
%epsf
boxed
barchart
xlabel "$xlab"
ylabel "$ylab"
ticks 2 100
subticks 1 0
centxlab
marksize 10
__EOF


my $total = 0;
foreach my $year (sort keys %counts)
{
    $total += $counts{$year};
    if(defined($::c))
    {
        print "$year $total\n";
    }
    else
    {
        print "$year $counts{$year}\n";
    }
}
