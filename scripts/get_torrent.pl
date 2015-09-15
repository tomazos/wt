#!/usr/bin/perl

use strict;
use warnings;

my $magnets_per_search = 3;

use Env qw(TORRENT_DIR);
use URI::Encode qw(uri_encode uri_decode);
use WWW::Curl::Easy;

sub pirate_bay_search_url($) {
    my($search_term_decoded) = @_;
    die unless defined $search_term_decoded;
    my $search_term_encoded = uri_encode($search_term_decoded);
    return "https://thepiratebay.co.in/search/$search_term_encoded/0/99/200";
}

sub get_url($) {
    my($url) = @_;
    print "getting url $url\n";
    my $curl = WWW::Curl::Easy - > new;
    $curl - > setopt(CURLOPT_HEADER, 1);
    $curl - > setopt(CURLOPT_URL, $url);
    my $response_body;
    $curl - > setopt(CURLOPT_WRITEDATA, \$response_body);
    my $retcode = $curl - > perform;
    warn($curl - > strerror($retcode).
        " ".$curl - > errbuf) unless $retcode == 0;
    return $response_body;
}

sub process_magnet($) {
    my($magnet_link) = @_;
    print "process magnet link $magnet_link\n";
    my@ args = ("aria2c", "--dir=$TORRENT_DIR", "--bt-metadata-only=true",
        "--bt-save-metadata",
        #"--bt-tracker-connect-timeout=6", "--bt-tracker-timeout=6",
        "--bt-stop-timeout=15",
        "--log=/data/aria2c.log", $magnet_link);
    if (system(@args) != 0) {
        if ($ ? != 1792) {
            warn "@args: $?";
        }
    }
}

while ( < > ) {
    chomp($_);
    my $search_term = $_;

    my $url = pirate_bay_search_url($search_term);

    my $response = get_url($url);

    my@ magnet_links;

    while ($response = ~/href=\"(magnet\:[^\"]*)\"/g) {
        push(@magnet_links, $1);
    }

    my $magnet_link_index = 0;
    for my $magnet_link(@magnet_links) {
        last
        if ($magnet_link_index++ >= $magnets_per_search);

        process_magnet($magnet_link);
    }
}
