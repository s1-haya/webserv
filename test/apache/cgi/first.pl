#!/usr/bin/perl
use strict;
use warnings;

my $request_method = $ENV{'REQUEST_METHOD'};
print "Content-type: text/html\n\n";
if ($request_method eq 'DELETE') {
    print "<html><body><h1>Resource deleted</h1></body></html>";
    exit;
}

print "<!DOCTYPE html>";
print "<html>";
print "<head>";
print "    <title>PATH_INFO Example</title>";
print "</head>";
print "<body>";

my $path_info = $ENV{'PATH_INFO'};
my $script_name = $ENV{'SCRIPT_NAME'};

if ($path_info) {
    print "    <h1>Requested Path:</h1>";
    print "    <p>PATH_INFO:   $path_info</p>";
    print "    <p>SCRIPT_NAME: $script_name</p>";
} else {
    print "    <h1>No PATH_INFO provided</h1>";
}

print <<EOF;
</body>
</html>
EOF
