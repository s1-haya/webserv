#!/usr/bin/perl
use strict;
use warnings;

# リダイレクト先のURLを設定
my $redirect_url = "/cgi-bin/print_env.pl";

# HTTPヘッダーを出力
print "Location: $redirect_url\r\n";
print "Content-Type: text/html\r\n\r\n";

exit;