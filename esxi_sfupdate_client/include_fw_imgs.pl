#!/usr/bin/perl

my @paths = @ARGV;
my $path;

sub list_all_c_files
{
    my $path = $_[0];
    my @dir_contents;
    my $fn;
    my @result_arr = ();
    my $full_path;

    opendir(D, $path) || die "Can't open directory $path\n";
    @dir_contents = readdir(D);
    closedir(D);

    foreach $fn (@dir_contents)
    {
        next if ($fn eq "." || $fn eq "..");
        $full_path = $path."/".$fn;
        if (-d $full_path)
        {
            @result_arr = (@result_arr, list_all_c_files($full_path));
        }
        elsif ($fn =~ m/^(.*)[.]c$/)
        {
            push(@result_arr, $full_path);
        }
    }

    return @result_arr;
}

my @all_c_files = ();

foreach $path (@paths)
{
    @all_c_files = (@all_c_files, list_all_c_files($path));
}


open(FILE, ">fw_images.c");
print FILE "#include \"fw_images.h\"\n\n";

for $i (0 .. $#all_c_files)
{
    print FILE "uint8_t img${i}[] = {\n";
    print FILE "#include \"".$all_c_files[$i]."\"\n";
    print FILE "};\n\n";
}

print FILE "fw_image_descr firmware_images[] = {\n";
for $i (0 .. $#all_c_files)
{
    print FILE "{ img$i },\n";
}
print FILE "};\n";
