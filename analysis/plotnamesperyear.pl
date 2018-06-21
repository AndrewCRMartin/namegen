#!/usr/bin/perl -s

use strict;

$::xmaxc  =  70 if(!defined($::xmaxc));
$::xmaxcc = 600 if(!defined($::xmaxcc));
$::date   = defined($::date)?"_$::date":"";

my $xlab        = "Year";
my $ylabC       = "Count";
my $ylabCC      = "Cummulative Count";
my $dataFile    = "npy_" . $$ . ".dat";
my $rFile       = "DoPlot_" . $$ . ".R";
my $countsPlot  = "namesperyear$::date.eps";
my $cCountsPlot = "namesperyear_cummulative$::date.eps";


my %counts = ReadData();
WriteDataFile($dataFile, %counts);
WriteRFile($rFile, $dataFile, 0, $xlab, $ylabC,  $countsPlot);
`Rscript $rFile`;
WriteRFile($rFile, $dataFile, 1, $xlab, $ylabCC, $cCountsPlot);
`Rscript $rFile`;
unlink($rFile);
unlink($dataFile);

sub WriteRFile
{
    my($rFile, $dataFile, $cummulative, $xlab, $ylab, $plotFile) = @_;

    my $field = $cummulative?"CCOUNT":"COUNT";
    my $ylim  = $cummulative?"c(0,$::xmaxcc)":"c(0,$::xmaxc)";
    
    if(open(my $fp, '>', $rFile))
    {
        print $fp <<__EOF;
#!/usr/bin/Rscript

data <- read.table("$dataFile", header=TRUE)
years <- data\$YEAR
counts <- data\$$field
setEPS()
postscript("$plotFile")
barplot(counts, names.arg=years, xlab="$xlab", ylab="$ylab", ylim=$ylim)
box()
dev.off()
__EOF
    }
}


sub ReadData
{
    my %counts = ();
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
    return(%counts);
}

sub WriteDataFile
{
    my ($datafile, %counts) = @_;

    if(open(my $fp, '>', $datafile))
    {
        my $total = 0;

        my @years = (sort keys %counts);
        my $yearMin = $years[0];
        my $yearMax = $years[scalar(@years)-1];

        print $fp "YEAR COUNT CCOUNT\n";
        for(my $year=$yearMin; $year<=$yearMax; $year++)
        {
            $counts{$year} = 0 if(!defined($counts{$year}));
            $total += $counts{$year};
            
            print $fp "$year $counts{$year} $total\n";
        }
        close $fp;
    }
    else
    {
        print STDERR "Error: can't write data file ($datafile)\n";
        exit 1;
    }
    
}
