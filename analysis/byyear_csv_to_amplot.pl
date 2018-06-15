#!/usr/bin/perl -s
use strict;

$::xlab = (defined($::xlab))?$::xlab:"Set";
$::ylab = (defined($::ylab))?$::ylab:"Value";

my $titleLine = <>;
chomp $titleLine;
$titleLine =~ s/^#//;
my @titles = split(/\s+/, $titleLine);
my $nFields = int(@titles);
my $xmin = 10000;
my $xmax = 0;
my $ymin = 10000;
my $ymax = 0;
my %data;

while(<>)
{
    chomp;
    my @fields = split;
    for(my $i=0; $i<$nFields; $i++)
    {
        push @{$data{$titles[$i]}}, $fields[$i];

        if($i == 0)
        {
            $xmax = $fields[$i] if($fields[$i] > $xmax);
            $xmin = $fields[$i] if($fields[$i] < $xmin);
        }
        else
        {
            $ymax = $fields[$i] if($fields[$i] > $ymax);
            $ymin = $fields[$i] if($fields[$i] < $ymin);
        }
    }
}

print <<__EOF;
boxed
xlabel "$::xlab"
ylabel "$::ylab"
__EOF

printf "keypos %.2f %.2f\n", $xmin + 0.8*($xmax-$xmin), $ymin + 0.1*($ymax-$ymin);

for(my $i=1; $i<$nFields; $i++)
{
    print "keytext $i \"$titles[$i]\"\n";
}    


for(my $i=1; $i<$nFields; $i++)
{
    my $title = $titles[$i];
    print "newset\n";
    print "pen $i\n";
    print "datapoint $i $i 1\n";
    print "style 1\n";
    print "% $title\n";

    my $count = 0;
    foreach my $set (@{$data{$titles[0]}})
    {
        if($set ne $titles[0])
        {
            printf "$set %.2f\n", $data{$title}[$count++];
        }
    }
}
