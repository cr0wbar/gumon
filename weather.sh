#!/bin/bash

CODE="LIRU"
NAME="Roma"
LINK="http://aviationweather.gov/adds/metars/?station_ids=$CODE&std_trans=standard&chk_metars=on&hoursStr=most+recent+only&submitmet=Submit"

DATA=`curl -s $LINK | grep $CODE`

if [[ $DATA =~ (M?[0-9]{2})/ ]]; then
    
    echo "\\f3\\fr \\f5$NAME ${BASH_REMATCH[1]}°\\fr | "
else
    echo "\\f3\\fr \\f5N/A\\fr | "
fi	

