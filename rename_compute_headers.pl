open(COMPUTE_HEADERS, "<COMPUTE_HEADERS") or die;

while (<COMPUTE_HEADERS>) {
  chomp($_);
  die unless /^(.*)\.hpp$/;
  my $stem = $1;
}

