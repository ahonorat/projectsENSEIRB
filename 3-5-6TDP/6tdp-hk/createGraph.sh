#!/bin/bash

rm -f res/*.png
gnuplot <<EOF
load 'graph1.gp'
EOF

gnuplot <<EOF
load 'graph2.gp'
EOF
