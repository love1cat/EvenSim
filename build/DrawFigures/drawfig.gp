#!/home/yus08002/Tools/gnuplot/bin/gnuplot

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
# set key Left box
# set yrange [0:1.25]
# set ytics 0, 0.2, 1
set style increment user
set grid ytics lt 0 lw 1 lc rgb "#bbbbbb"
set grid xtics lt 0 lw 1 lc rgb "#bbbbbb"

# plot limited opt with diff area results
# set yrange [340:480]
#daeps = "even_darea_lim.eps"
#dalog = "even_out_lim_diffarea.txt"
#
#set terminal postscript eps color enhanced "Times-Roman" fontsize
#set output daeps
#set xlabel "Number of heterogeneous sensors (k)"
#set ylabel "Objective Value (KJ)"
#plot dalog using 2:xtic(1) title "Area 1", \
#dalog using 3:xtic(1) title "Area 2", \
#dalog using 4:xtic(1) title "Area 3", \
#dalog using 5:xtic(1) title "Area 4", \
#dalog using 6:xtic(1) title "Area 5"

#####################################
# plot diff loc energy results
#####################################
# set yrange [490:590]
dleeps = "even_dlocenergy.eps"
dle_ovr_eps = "even_dlocenergy_ovr.eps"
dlelog = "even_out_diff_loc_ediff.txt"

# Draw obj value
set terminal postscript eps color enhanced "Times-Roman" fontsize
set output dleeps
#set xrange [0:11]
set yrange [180:240]
set key right bottom
set xlabel "Number of starting locations (l)"
set ylabel "Objective Value (KJ)"
plot dlelog using 2:xtic(1) title "b = 3, h = 10", \
dlelog using 4:xtic(1) title "b = 6, h = 10"

# Draw obj value ratio
set terminal postscript eps color enhanced "Times-Roman" fontsize
set output dle_ovr_eps
#set xrange [0:11]
set yrange [0.80:1.01]
set key right bottom
set xlabel "Number of starting locations (l)"
set ylabel "Avg. Objective Value Ratio"
plot dlelog using 3:xtic(1) title "b = 3, h = 10", \
dlelog using 5:xtic(1) title "b = 6, h = 10"

#####################################
# plot diff loc ratio results
#####################################
# set yrange [480:610]
dlreps = "even_dlocratio.eps"
dlrlog = "even_out_diff_loc_ratio.txt"
dlr_ovr_eps = "even_dlocratio_ovr.eps"

# Draw obj value
set terminal postscript eps color enhanced "Times-Roman" fontsize
set output dlreps
set yrange [180:245]
#set xrange [0:11]
set key right bottom
set xlabel "Number of starting locations (l)"
set ylabel "Objective Value (KJ)"
plot dlrlog using 2:xtic(1) title "h = 0, b = 6", \
dlrlog using 4:xtic(1) title "h = 10, b = 6", \
dlrlog using 6:xtic(1) title "h = 20, b = 6"

# Draw obj value ratio
set terminal postscript eps color enhanced "Times-Roman" fontsize
set output dlr_ovr_eps
set yrange [0.80:1.01]
#set xrange [0:11]
set key right bottom
set xlabel "Number of starting locations (l)"
set ylabel "Objective Value Ratio"
plot dlrlog using 3:xtic(1) title "h = 0, b = 6", \
dlrlog using 5:xtic(1) title "h = 10, b = 6", \
dlrlog using 7:xtic(1) title "h = 20, b = 6"

####################################
# plot obj ratio errorbars for diff loc with and without  enhance
####################################
dlmmaeps = "even_dloc_minmaxavg_noenh.eps"
dlmmalog = "even_out_diff_loc_minmaxavg.txt"

dlmma2eps = "even_dloc_minmaxavg_enh.eps"
dlmma2log = "even_out_diff_loc_minmaxavg_enh.txt"

dlmma_avg_eps = "even_dloc_avg.eps"
# mean obj value ratio
set terminal postscript eps color enhanced "Times-Roman" 20
set output dlmma_avg_eps
#set xrange [0:11]
set yrange [0.80:1.01]
set key right bottom
set xlabel "Number of starting locations (l)"
set ylabel "Average Objective Value Ratio (OVR)"
plot dlmmalog using 2:xtic(1) title "Average OVR w/o enhancing algorithm", \
dlmma2log using 2:xtic(1) title "Average OVR w/ enhancing algorithm"

# Without enhance
set terminal postscript eps color enhanced "Times-Roman" 20
set output dlmmaeps
set xrange [0:11]
set yrange [0.80:1.01]
set key right bottom
set xlabel "Number of starting locations (l)"
set ylabel "Objective Value Ratio (OVR)"
plot dlmmalog using 1:2:3:4:xtic(1) title "OVR range w/o enhancing algorithms" with yerrorbars, \
 "" smooth csplines t "Average OVR w/o enhancing algorithms"

# With enhance
set terminal postscript eps color enhanced "Times-Roman" 20
set output dlmma2eps
set xrange [0:11]
set yrange [0.80:1.01]
set key right bottom
set xlabel "Number of starting locations (l)"
set ylabel "Objective Value Ratio (OVR)"
plot dlmma2log using 1:2:3:4:xtic(1) title "OVR range w/ enhancing algorithms" with yerrorbars, \
 "" smooth csplines t "Average OVR w/ enhancing algorithms"

