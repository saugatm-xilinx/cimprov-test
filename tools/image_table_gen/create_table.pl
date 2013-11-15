#!/usr/bin/perl 

if ($#ARGV < 0)
{
    print "Path to the folder with firmware images is not specified\n";
    exit(1);
}

my $path = $ARGV[0];

print "Creating table for ".$path." ...\n";

opendir (DIR, $path) or die $!;

while (my $file = readdir(DIR))
{
    if ($file =~ m/[.]dat$/)
    {
        $ids = `./test_image $path/$file`;
        $ids =~ m/(.*)[.]([0-9]*)/;
        print "{ $1, $2, \"$file\" },\n";
    }
}
