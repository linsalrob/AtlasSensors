#!/bin/bash
DATE=`date "+%Y%m%d-%H:%M:%S"`

# reset the gpio outputs
/home/pi/atlas-scientific/reset_gpio.py

# a sleep to make sure that the wifi connects
sleep 120

# read
# /usr/bin/python /home/pi/atlas-scientific/atlas-read-all-write.py -f ~pi/probes.$DATE.tsv
# /usr/bin/python /home/pi/atlas-scientific/atlas-read-all-plotly.py -f ~pi/probes.wifi.$DATE.tsv
