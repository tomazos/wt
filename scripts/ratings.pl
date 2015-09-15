#/usr/bin/perl

use strict;
use warnings;

open(my $ratings_file, "<", "/data/ratings.list") or die $!;

my @movies;
while (<$ratings_file>) {
  chomp($_);
  if (m/\s+[\.\*0-9]{10}\s+(\d+)\s+(1?\d.\d)\s+(.*?)\s+\((\d{4})(\/[IV]+)?\)/) {
    my %movie;
    $movie{"votes"} = $1;
    $movie{"rating"} = $2;
    $movie{"name"} = $3;
    $movie{"year"} = $4;
    push(@movies, \%movie);
  }
}

@movies = sort {$b->{"votes"} <=> $a->{"votes"}} @movies;

for my $movie (@movies) {
  if ($movie->{"votes"} > 20000 and $movie->{"year"} > 1995 and $movie->{"year"} < 2014) {
    my $search_term = $movie->{"name"} . " " . $movie->{"year"};
    $search_term =~ s/[^0-9A-Za-z ]//g;
    $search_term = lc($search_term);
    print "$search_term\n";
  }
}

