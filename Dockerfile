FROM gocv/opencv:4.8.0-ubuntu-22.04
WORKDIR /app
COPY . . 
RUN apt update && apt install cmake libcanberra-gtk-module -y
RUN cmake . && make
