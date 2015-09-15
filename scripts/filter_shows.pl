#!/usr/bin/perl

use strict;
use warnings;

use List::Util qw(first max maxstr min minstr reduce shuffle sum);

my %shows;

sub load_shows() {
  open(my $shows_file, "<", "/data/shows") or die $!;

  my $show;

  while (<$shows_file>) {
    chomp($_);
    if (/^START_TORRENT: (\d+) (.*)$/) {
      $show = {};
      $show->{"name"} = $2;
      $show->{"size"} = $1;
      $show->{"files"} = [];
    } elsif (/^(\d+) (.*)$/) {
      my $file = {};
      $file->{"name"} = $2;
      $file->{"size"} = $1;
      push (@{$show->{"files"}}, $file);
    } elsif (/^END_TORRENT$/) {
      $shows{$show->{"name"}} = $show;
    } else {
      die;
    }
  }
  close($shows_file);
}


my %okexts;
$okexts{"mp4"} = 1;
$okexts{"mkv"} = 1;
$okexts{"avi"} = 1;
$okexts{"srt"} = 1;

my $c1 = 0;
my $c2 = 0;
my $c3 = 0;
my $c4 = 0;

sub process_show($) {
  my ($show) = @_;

  $c1++;

  my $show_name = $show->{"name"};
  my $show_size = $show->{"size"};

  my $contains_srt = 0;

  my %exts;

  my $eesrt;
  my $esrt;
  my $srt;
  my $video;

  $exts{"srt"} = 0;
  $exts{"esrt"} = 0;
  $exts{"eesrt"} = 0;
  $exts{"mp4"} = 0;
  $exts{"mkv"} = 0;
  $exts{"avi"} = 0;

  for my $file (@{$show->{"files"}}) {
    my $file_name = $file->{"name"};
    my $file_size = $file->{"size"};

    next unless $file_name =~ /\.([^\.]+)$/;
    next if $file_name =~ /sample/i;
    my $ext = $1;
    next unless $okexts{$ext};

    if ($ext eq "srt") {
      if ($file_name =~ /english/i) {
        $exts{"eesrt"}++;
        $eesrt = $file_name;
      } elsif ($file_name =~ /eng/i) {
        $exts{"esrt"}++;
        $esrt = $file_name;
      } else {
        $exts{"srt"}++;
        $srt = $file_name;
      }
    } else {
      $video = $file_name;
      $exts{$ext}++;
    }
  }

  my $nvideos = $exts{"mp4"} + $exts{"mkv"} + $exts{"avi"};
  return unless $nvideos == 1;
  $c2++;
  return unless ($exts{"srt"} == 1 or $exts{"esrt"} == 1 or $exts{"eesrt"} == 1);
  $c3++;

  if ($exts{"eesrt"} == 1) {
    $srt = $eesrt;
  } elsif ($exts{"esrt"} == 1) {
    $srt = $esrt;
  }

  return if ($show_size > 5000000000);
  print "[<begin_torrent>]".$show_name;
  print "[<begin_video_file>]".$video."[<end_video_file>]";
  print "[<begin_subtitles_file>]".$srt."[<end_subtitles_file>]";
  print "[<end_torrent>]\n";
}

load_shows();

for my $show (shuffle (values %shows)) {
  process_show($show);
}
