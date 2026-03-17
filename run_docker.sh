# xhost +local:docker

# docker build -t scop .

# docker run -it --rm \
#     -e DISPLAY=$DISPLAY \
#     -v /tmp/.X11-unix:/tmp/.X11-unix \
#     -v $(pwd):/app \
#     --device /dev/dri:/dev/dri \
#     scop


#!/bin/bash

OS="$(uname -s)"

case "$OS" in
	Linux)
		echo "[Linux] Configuration X11..."
		xhost +local:docker

		docker build -t scop .

		docker run -it --rm \
			-e DISPLAY=$DISPLAY \
			-v /tmp/.X11-unix:/tmp/.X11-unix \
			-v $(pwd):/app \
			--device /dev/dri:/dev/dri \
			scop
		;;

	Darwin)
		echo "[macOS] Configuration XQuartz..."

		if ! command -v xhost &> /dev/null; then
			echo "XQuartz n'est pas installé. Installez-le avec : brew install --cask xquartz"
			exit 1
		fi

		IP=$(ifconfig en0 | grep inet | awk '$1=="inet" {print $2}')
		if [ -z "$IP" ]; then
			IP=$(ifconfig | grep "inet " | grep -v 127.0.0.1 | awk '{print $2}' | head -1)
		fi

		echo "IP locale : $IP"
		xhost + $IP

		docker build -t scop .

		docker run -it --rm \
			-e DISPLAY=$IP:0 \
			-v /tmp/.X11-unix:/tmp/.X11-unix \
			-v $(pwd):/app \
			scop
		;;

	*)
		echo "Système non supporté : $OS"
		exit 1
		;;
esac