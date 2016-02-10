#!/bin/bash

# Variables
rsize="2000x2000"
filename=Julia.data

# Default Lighting Environment
# Red
#  diffuse  = 1.0 0.4  0.2  *5
#  position = 0.8 0.1 -0.1
# Green
#  diffuse  =  0.2 1.0  0.3 *3;
#  position = -1.0 1.0 -0.2
#  
# Blue
#  diffuse  = 0.0 0.4 1.0   *3
#  position = 0.0 1.0 0.0;

# Interesting Configuration.
# (-0.450 0.447 0.181 0.306)


# QJulia
./QJulia -q "-0.2 0.8 0.0 0.0" -c "20 10 4" -f 3 \
         -t 12 -d 100 -z "2 8" -o $filename -s "${rsize/x/ }"
# ImageMagick
convert -size $rsize -depth 8 \
          rgb:$filename $(echo $filename | cut -f 1 -d '.').png

# Delete the raw data.
rm $filename
