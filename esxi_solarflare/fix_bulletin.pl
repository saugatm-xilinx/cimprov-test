#!/usr/bin/perl

use File::Temp;

my $fn = $ARGV[0];
my $txt = "";
my $print = 1;

open(my $fh, "<", $fn) or die;

while (<$fh>)
{
    my $s = $_;

    if ($s =~ /<\/platforms>/)
    {
        $print = 1;
    }    
    if ($print == 1)
    {
        $txt = $txt.$s;
    }
    if ($s =~ /<platforms>/)
    {
        $print = 0;
        $txt = $txt.
          "\t\t<softwarePlatform productLineID=\"esx\" version=\"5.*\" locale=\"\" \/>\n";
        $txt = $txt.
          "\t\t<softwarePlatform productLineID=\"esx\" version=\"6.*\" locale=\"\" \/>\n";
        $txt = $txt.
          "\t\t<softwarePlatform productLineID=\"embeddedEsx\" version=\"5.*\" locale=\"\" \/>\n";
        $txt = $txt.
          "\t\t<softwarePlatform productLineID=\"embeddedEsx\" version=\"6.*\" locale=\"\" \/>\n";
    }
} 

close($fh);

open(my $fh, ">", $fn) or die;
print $fh $txt;
close($fh);
