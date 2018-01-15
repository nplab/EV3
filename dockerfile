# based on nginx
FROM sinet/nginx-node:5.9.1

# autor
MAINTAINER FBROER_JAPPELFELLER

# install necessary packages
USER root
RUN apt-get -y update & 
RUN apt-get --force-yes -y install nginx

RUN npm install simple-peer --save  # add WebRTC 

# nginx config 
COPY website/conf/default.conf /etc/nginx/conf.d/default.conf

# copy website-data
COPY website/src/html/ /usr/share/nginx/html/
COPY website/src/ usr/share/nginx/html/





