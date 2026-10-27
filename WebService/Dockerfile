# Web server with lightweight linux distribution
FROM nginx:alpine

# Default location for nginx to serve static files
COPY . /usr/share/nginx/html 
#Copy the configuration file
COPY nginx.conf /etc/nginx/conf.d/default.conf
# Default port for nginx is 80
EXPOSE 80 