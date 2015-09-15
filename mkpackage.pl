#!/usr/bin/perl

use strict;
use warnings;

use File::Basename;

my $dir = $ARGV[0];

my $rulename = basename($dir);

opendir(DIR, $dir) or die $!;

my @files;

while (my $file = readdir(DIR)) {
  if (-f "$dir/$file" and $file =~ /\.h$/) {
    push(@files, $file);
  }
}

if (scalar(@files) > 0) {
  print "/$dir/$rulename\n";
  open(RULE, ">$dir/RULES") or die $!;

  print(RULE "library:\n");
  print(RULE "  name = $rulename\n");
  print(RULE "  headers:\n");

  for my $file (@files) {
    print(RULE "    $file\n");
  }
}

close(RULE);

closedir(DIR);

