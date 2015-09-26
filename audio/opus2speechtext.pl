#!/usr/bin/perl

use strict;
use warnings;

my $opus_path = $ARGV[0];

die "usage: opus2speechtext.pl <opus>" unless defined($opus_path);

die "no such file $opus_path" unless -f $opus_path;

die "bad ext $opus_path" unless $opus_path =~ /\.opus$/;

my $stem_path = $opus_path;
$stem_path =~ s/\.opus$//;

my $wav_path = $stem_path . ".wav";
my $srtproto_path = $stem_path . ".srtproto";
my $speechtext_path = $stem_path . ".speechtext";

system("ffmpeg", "-i", $opus_path, "-ar", "12000", $wav_path);
system("/home/zos/robo/.whee/programs/native/audio/extract_speechtext", $wav_path, $srtproto_path, $speechtext_path);

unlink($wav_path) or warn "could not unlink $wav_path $!";

