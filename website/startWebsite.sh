sudo docker stop $(sudo docker ps -a -q)
sudo docker run --rm -itd \
		--publish 8080:80 \
		frederik/nginx:0.1

