#!/usr/bin/perl

use strict;
use warnings;

# Error: Premature end of script headers: print_query_string.pl
print "Content-type: text/plain\r\n\r\n";

my $query_string = $ENV{'QUERY_STRING'};

print "QUERY_STRING: $query_string\n";

my @pairs = split(/&/, $query_string);

foreach my $pair (@pairs) {
	my ($key, $value) = split(/=/, $pair);
	print "$key:$value\n";
}
