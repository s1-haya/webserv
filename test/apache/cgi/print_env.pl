#!/usr/bin/perl

use strict;
use warnings;

# 環境変数の取得
my $auth_type = $ENV{'AUTH_TYPE'};
my $content_length = $ENV{'CONTENT_LENGTH'};
my $content_type = $ENV{'CONTENT_TYPE'};
my $gateway_interface = $ENV{'GATEWAY_INTERFACE'};
my $path_info = $ENV{'PATH_INFO'};
my $path_translated = $ENV{'PATH_TRANSLATED'};
my $query_string = $ENV{'QUERY_STRING'};
my $remote_addr = $ENV{'REMOTE_ADDR'};
my $remote_user = $ENV{'REMOTE_USER'};
my $request_method = $ENV{'REQUEST_METHOD'};
my $script_name = $ENV{'SCRIPT_NAME'};
my $server_name  = $ENV{'SERVER_NAME'};
my $server_port = $ENV{'SERVER_PORT'};
my $server_protocol  = $ENV{'SERVER_PROTOCOL'};
my $server_software  = $ENV{'SERVER_SOFTWARE'};
# my $scheme  = $ENV{'scheme'};
# my $protocol_var_name  = $ENV{'protocol-var-name'};

# HTTPレスポンスヘッダーを出力
print "Content-type: text/plain\n\n";

# 環境変数の値を出力
print "AUTH_TYPE: $auth_type\n";
print "CONTENT_LENGTH: $content_length\n";
print "GATEWAY_INTERFACE: $gateway_interface\n";
print "PATH_INFO: $path_info\n";
print "PATH_TRANSLATED: $path_translated\n";
print "CONTENT_TYPE: $content_type\n";
print "QUERY_STRING: $query_string\n";
print "REMOTE_ADDR: $remote_addr\n";
print "REMOTE_USER: $remote_user\n";
print "REQUEST_METHOD: $request_method\n";
print "SCRIPT_NAME: $script_name\n";
print "SERVER_NAME: $server_name\n";
print "SERVER_PORT: $server_port\n";
print "SERVER_PROTOCOL: $server_protocol\n";
print "SERVER_SOFTWARE: $server_software\n";
# print "scheme: $scheme\n";
# print "protocol-var-name: $protocol_var_name\n";
