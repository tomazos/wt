#!/usr/bin/perl

use strict;
use warnings;

use Google::ProtocolBuffers;

Google::ProtocolBuffers->parsefile("srtproto_file.proto");

my @subtitles;

sub get_next_line() {
  my $line = <>;
  return "" if (!defined($line));

  $line =~ s/\r\n/\n/;
  chomp ($line);
  return $line;
}

while (!eof()) {
  last if (eof(STDIN));
  my $time_range = get_next_line();
  next unless $time_range =~
  /
  ^
  (\d\d) # begin_hours
  \:
  (\d\d) # begin_mins
  \:
  (\d\d) # begin_secs
  \,
  (\d\d\d) # begin_mspart
  \s\-\-\>\s
  (\d\d) # end_hours
  \:
  (\d\d) # end_mins
  \:
  (\d\d) # end_secs
  \,
  (\d\d\d) # end_mspart
  $
  /x;

  my $begin_hours = $1;
  my $begin_mins = $2;
  my $begin_secs = $3;
  my $begin_mspart = $4;
  my $end_hours = $5;
  my $end_mins = $6;
  my $end_secs = $7;
  my $end_mspart = $8;

  my $begin_ms = $begin_hours * 3600000 + $begin_mins * 60000 + $begin_secs * 1000 + $begin_mspart;
  my $end_ms = $end_hours * 3600000 + $end_mins * 60000 + $end_secs * 1000 + $end_mspart;

  my @text_lines;
  while (1) {
    my $next_line = get_next_line();
    last if ($next_line eq "");
    push(@text_lines, $next_line);
  }

  my $text = join("\n", @text_lines);

  if ($text ne "") {
    my $subtitle = {
      begin_ms => $begin_ms,
      end_ms => $end_ms,
      text => $text,
    };
    push(@subtitles, $subtitle);
  }
}

binmode STDOUT;
print (Audio::SrtProtoFile->encode({subtitles => \@subtitles}));
