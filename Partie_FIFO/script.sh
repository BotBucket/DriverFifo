#!/bin/bash

if [[ $EUID -ne 0 ]]; then
	echo "Must Be run as ROOT"
	exit 1
fi
#Default values
maxInterfaces=5
clean=0
isANumber='^[0-9]+$'

#Test all given argument(s)
for var in "$@"
do
       	if [[ "$var" =~ $isANumber ]]; then
                 maxInterfaces=$var
        fi
	if [[ "$var" == "clean" ]]; then
		clean=1
        fi
done



#Cleans the interfaces.
#Not neccessary  when testing a new module version as the MAJOR number stays the same and 
#the interfaces are linked to it and module reinsertion is enough to change their behaviour
if [[ `ls /dev/ | grep fifo | wc -l` -ne 0 ]]; then
	rm /dev/fifot*
	echo "Cleanning existing lettre interface"
fi
#Removes the previously insterted module
if [[ `lsmod | head | grep fifot | wc -l` -ne 0 ]]; then
	rmmod fifot
	echo "Removing module"
fi
if [[ $clean -ne 1 ]]; then
	#checks if the module to insert exist
	if [[ `ls | grep fifot.ko | wc -l` -ne 0 ]]; then
	#Cheks devices before insertion
	cat /proc/devices > out1
	#Inserts newly compiled module
	insmod fifot.ko
	#Checks devices after insertion
	cat /proc/devices > out2

	#The following lines allow the interfaces to be linked to the major number of the module
	#without knowing the character device name or major number beforehand

	#Stores only the differing line between out1 and out2 which corresponds to the added 
	#character device
	device=`diff --changed-group-format='%<%>' --unchanged-group-format='' out1 out2`
	#Stores the second word of the $device line in deviceName
	deviceName=`echo $device | awk '{print $2}'`
	#Stores the first word of the device in majorNumber
	majorNumber=`echo $device | awk '{print $1}'`
	#Remove log files
	rm out1 out2
	echo "Module inserted"
	for i in `seq 0 $maxInterfaces`; do
		mknod /dev/$deviceName$i c $majorNumber $i
	done
	echo $maxInterfaces+1 "created interfaces"
	else 
                echo "Module to insert not found"
		echo "No interface created"
        fi

fi


