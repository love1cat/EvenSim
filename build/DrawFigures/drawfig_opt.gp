#!/Users/andy/Tools/gnuplot/bin/gnuplot

# set style
lwidth = 2
psize =  2 
fontsize = 30
set autoscale
set style data linespoints
set key left top
set style line 1 lt 1 lw lwidth pt 3 ps psize 
set style line 2 lt 3 lw lwidth pt 1 ps psize
set style line 3 lt 7 lw lwidth pt 7 ps psize
set style line 4 lt 2 lw lwidth pt 2 ps psize
set style line 5 lt 4 lw lwidth pt 4 ps psize
set style line 6 lt 8 lw lwidth pt 8 ps psize
# set key Left box
# set yrange [0:1.25]
# set ytics 0, 0.2, 1
set style increment user
set grid ytics lt 0 lw 1 lc rgb "#bbbbbb"
#set grid xtics lt 0 lw 1 lc rgb "#bbbbbb"

# plot limited opt with diff area results
# set yrange [340:480]
daeps = "even_darea_opt.eps"
dalog = "../opt_grd/even_opt_diff_area.txt"

set terminal postscript eps color enhanced "Times-Roman" fontsize
set output daeps
set key right bottom
set xlabel "Different Areas"
set ylabel "Objective Value (KJ)"
set xtics ("Area 1" 0, "Area 2" 1, "Area 3" 2, "Area 4" 3, "Area 5" 4)
plot dalog using 1 title "b_l=3", \
dalog using 2 title "b_l=6", \
dalog using 3 title "b_l=9"
