xhost +local:docker

docker build -t scop .

docker run -it --rm \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v $(pwd):/app \
    --device /dev/dri:/dev/dri \
    scop