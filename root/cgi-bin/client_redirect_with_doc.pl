#!/usr/bin/perl
use strict;
use warnings;

# リダイレクト先のURLを設定
my $redirect_url = "http://localhost:8080/";

# HTTPヘッダーを出力
print "Location: $redirect_url\r\n";
print "Content-Type: text/html\r\n\r\n";

# HTMLコンテンツを出力（オプション）
print "<html>\n";
print "<head><title>Redirect</title></head>\n";
print "<body>\n";
print "<p>Redirecting to <a href=\"$redirect_url\">$redirect_url</a></p>\n";
print "</body>\n";
print "</html>\n";

exit;
