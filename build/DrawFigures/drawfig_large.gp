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

#### LARGE SCEN ####
####################################
# plot obj graphs
####################################
tmp_rt = "even_dloc_runtime_large.eps"
tmp_rt3_log = "../large/enh2_even_out_diff_loc_runtime.txt"
tmp_rt2_log = "../large/enh_even_out_diff_loc_runtime.txt"
tmp_rt1_log = "../large/even_out_diff_loc_runtime.txt"


set terminal postscript eps color enhanced "Times-Roman" fontsize
set output tmp_rt
set yrange [0:0.15]
set key right top
set xlabel "Number of starting locations"
set ylabel "Running time (s)"
plot tmp_rt1_log using 2 title "Approximation alg", \
tmp_rt2_log using 2 title "Algorithm 3", \
tmp_rt3_log using 2 title "Algorithm 4"

####################################
# plot obj ratio errorbars for diff loc
####################################
large_eps = "even_dloc_minmaxavg_large.eps"
large_enh2_log = "../large/enh2_even_out_diff_loc_minmaxavg.txt"
large_enh_log = "../large/enh_even_out_diff_loc_minmaxavg.txt"
large_log = "../large/even_out_diff_loc_minmaxavg.txt"

# With enhance
set terminal postscript eps color enhanced "Times-Roman" 20
set output large_eps
set xrange [0:11]
set yrange [0.92:1.06]
set key right bottom
set xlabel "Number of starting locations (l)"
set ylabel "Objective Value / Best Approx. Alg." 
plot large_log using 1:2:3:4 title "Algorithm 2" with yerrorbars, \
 "" t "Avg ratio", \
    large_enh_log using 1:2:3:4:xtic(1) title "Algorithm 3" with yerrorbars, \
 "" t "Avg ratio", \
    large_enh2_log using 1:2:3:4 title "Algorithm 4" with yerrorbars, \
 "" t "Avg ratio"

####################################
# plot runtim ratio errorbars for diff loc
####################################
large_rt_eps = "even_dloc_rt_large.eps"
large_rt_enh2_log = "../large/enh2_even_out_diff_loc_rt_ratio.txt"
large_rt_enh_log = "../large/enh_even_out_diff_loc_rt_ratio.txt"
large_rt_log = "../large/even_out_diff_loc_rt_ratio.txt"

# With enhance
set terminal postscript eps color enhanced "Times-Roman" 20
set output large_rt_eps
set xrange [0:11]
set yrange [0:30]
set key right top
set xlabel "Number of starting locations (l)"
set ylabel "Best Approx. Alg. / Running Time" 
plot large_rt_log using 1:2:3:4 title "Algorithm 2" with yerrorbars, \
 "" t "Avg ratio", \
     large_rt_enh_log using 1:2:3:4:xtic(1) title "Algorithm 3" with yerrorbars, \
 "" t "Avg ratio", \
    large_rt_enh2_log using 1:2:3:4 title "Algorithm 4" with yerrorbars, \
 "" t "Avg ratio"

