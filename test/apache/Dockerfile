FROM httpd:2.4

RUN apt-get update && apt-get install -y \
	vim \
	perl

COPY test/apache/config/httpd.conf /usr/local/apache2/conf/httpd.conf
COPY test/apache/cgi/first.pl /usr/local/apache2/cgi-bin/first.pl
COPY test/apache/cgi/print_env.pl /usr/local/apache2/cgi-bin/print_env.pl
COPY test/apache/cgi/print_query_string.pl /usr/local/apache2/cgi-bin/print_query_string.pl

RUN chmod a+x /usr/local/apache2/cgi-bin/first.pl
RUN chmod a+x /usr/local/apache2/cgi-bin/print_env.pl
RUN chmod a+x /usr/local/apache2/cgi-bin/print_query_string.pl

EXPOSE 4242
