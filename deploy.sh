#!/bin/bash

PROJECT_PATH=/home/utnso/tp-2022-1c-FASE_A
TESTING_PATH=/home/utnso/kiss-pruebas
SWAP_PATH=/home/utnso/swap
COMMON_PATH=/home/utnso/so-commons-library

if [ "$1" == "" ]; then
	echo "Error de ejecucion: ./deploy.sh <install | uninstall>"
	exit 1
fi


if [ "$1" == "install" ]; then

	echo -e "\n\nInstalling commons libraries...\n\n"
	cd $HOME
	COMMONS="so-commons-library"
	rm -rf $COMMONS
	git clone "https://github.com/sisoputnfrba/${COMMONS}.git" $COMMONS
	cd $COMMONS
	make uninstall
	make install


	echo -e "\n\n **** Descargando Pruebas... ****\n\n"
	cd $HOME
	git clone "https://github.com/sisoputnfrba/kiss-pruebas"


	
	mkdir -p $SWAP_PATH
	cd $PROJECT_PATH

	PROJECTS=(Consola Kernel CPU Memoria)
	echo -e "\n\nBuilding projects..."

	for i in "${PROJECTS[@]}"
	do
  		echo -e "\n\nBuilding ${i}\n\n"
  		cd $i
		make clean
  		make all 
  		cd $PROJECT_PATH
	done

	echo -e "\n\n ***** Se instalo todo. Ejecutar modulos *****\n"

else
	echo -e "\n\n **** Desinstalando todo... ****\n\n"
	cd $COMMON_PATH
	make uninstall
	cd $HOME
	rm -rf $COMMON_PATH

	rm -rf $TESTING_PATH
	rm -rf $SWAP_PATH
	
	rm -rf $PROJECT_PATH
fi	