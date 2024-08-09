FROM httpd:2.4

RUN apt-get update && apt-get install -y \
	vim \
	perl

COPY test/apache/config/httpd.conf /usr/local/apache2/conf/httpd.conf
COPY test/apache/cgi/first.pl /usr/local/apache2/cgi-bin/first.pl
COPY test/apache/cgi/print_env.pl /usr/local/apache2/cgi-bin/print_env.pl
COPY test/apache/cgi/print_query_string.pl /usr/local/apache2/cgi-bin/print_query_string.pl
COPY test/apache/cgi/print_stdin.pl /usr/local/apache2/cgi-bin/print_stdin.pl
COPY test/apache/cgi/print_stdout.pl /usr/local/apache2/cgi-bin/print_stdout.pl
COPY test/apache/no_content.txt /usr/local/apache2/no_content.txt

RUN chmod a+x /usr/local/apache2/cgi-bin/first.pl
RUN chmod a+x /usr/local/apache2/cgi-bin/print_env.pl
RUN chmod a+x /usr/local/apache2/cgi-bin/print_query_string.pl
RUN chmod a+x /usr/local/apache2/cgi-bin/print_stdin.pl
RUN chmod a+x /usr/local/apache2/cgi-bin/print_stdout.pl
RUN chmod a+x /usr/local/apache2/no_content.txt

EXPOSE 4242
