#!/usr/bin/perl

use strict;
use warnings;

use File::Path qw(remove_tree);
use File::Copy;
use File::Find;

my $target_id = $ARGV[0];

die "usage process_target.pl <target_id>" unless defined $target_id;

sub find_in_dir($$) {
  our ($dir, $filename) = @_;
  find(\&wanted, $dir);
  our $path;
  sub wanted {
    if ($_ eq $filename) {
      $path = $File::Find::name;
    }
  };
  return $path;
}

sub process_target($$$) {
  my ($torrent_filename, $video_filename, $subtitles_filename) = @_;
  die "$torrent_filename" unless $torrent_filename =~ /^\/data\/torrents\/(.*)\.torrent$/;
  my $code = $1;
  my $temp_dir = `mktemp -d`;
  chomp($temp_dir);
  mkdir("$temp_dir/output") or die $!;
  print "creating download directory $temp_dir for $torrent_filename\n";

  my @aria2c = (
    "aria2c",
    "--dir=$temp_dir",
    "--bt-stop-timeout=600",
    "--log=$temp_dir/aria2c.log",
    "--seed-time=0",
    "--stop=3600",
    "--max-overall-upload-limit=1",
    "--max-upload-limit=1",
    $torrent_filename
  );

  print(join(" ", @aria2c), "\n");
  if (system(@aria2c) != 0) {
    remove_tree($temp_dir);
    die "aria2c failed";
  }

  my $video_path = find_in_dir($temp_dir, $video_filename);
  my $subtitles_in = find_in_dir($temp_dir, $subtitles_filename);
  my $audio_path = "$temp_dir/output/audio.opus";
  my $subtitles_out = "$temp_dir/output/subtitles.srt";

  my @ffmpeg = (
    "ffmpeg",
    "-i",
    ${video_path},
    "-vn",
    "-acodec",
    "opus",
    "-b:a",
    "16000",
    "-ar",
    "12000",
    "-ac",
    "1",
    $audio_path
  );

  print(join(" ", @ffmpeg), "\n");
  if (system(@ffmpeg) != 0) {
    remove_tree($temp_dir);
    die "ffmpeg failed";
  }

  copy($subtitles_in, $subtitles_out) or die "copy $!";

  if (system("scp", $audio_path, "master.tomazos.net:/data/output/$code.opus") != 0) {
    remove_tree($temp_dir);
    die "scp failed";
  }

  if (system("scp", $subtitles_out, "master.tomazos.net:/data/output/$code.srt") != 0) {
    remove_tree($temp_dir);
    die "scp failed";
  }

  remove_tree($temp_dir);
}

sub get_target_array() {
  open(my $targets_file, "<", "/data/targets") or die $!;

  my @targets;

  while (<$targets_file>) {
    chomp($_);

    die $_ unless /
    ^
    \[\<begin_torrent\>\]
    (.*)
    \[\<begin_video_file\>\]
    (.*)
    \[\<end_video_file\>\]
    \[\<begin_subtitles_file\>\]
    (.*)
    \[\<end_subtitles_file\>\]
    \[\<end_torrent\>\]
    $
    /x;
    my %target;
    $target{"torrent"} = $1;
    $target{"video"} = $2;
    $target{"subtitles"} = $3;

    push (@targets, \%target);
  }
  return \@targets;
}

my $targets = get_target_array();
my $target = $targets->[$target_id];
die "invalid $target_id" unless $target;
process_target($target->{"torrent"},$target->{"video"},$target->{"subtitles"});
