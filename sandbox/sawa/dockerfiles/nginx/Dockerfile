FROM nginx:latest

RUN apt-get update && \
    apt-get install -y telnet

EXPOSE 80

COPY config/nginx.conf /etc/nginx/nginx.conf

RUN mkdir -p /data/www
RUN echo "<html><body><h1>Hello from /data/www!</h1></body></html>" > /data/www/index.html
