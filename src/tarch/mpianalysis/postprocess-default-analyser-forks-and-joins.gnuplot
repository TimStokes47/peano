# author: Tobias Weinzierl


set style line 1 lc rgb "#ff6600" lt 1 pt 7   lw 1.0 ps 1.6 
set style line 2 lc rgb "#00ff66" lt 1 pt 9   lw 1.0 ps 1.6 
set style line 3 lc rgb "#6600ff" lt 1 pt 11  lw 1.0 ps 1.6 


set grid

set xlabel 'Simulation steps'
set ylabel 'total number of working ranks, forks or joins respectively'

set key right bottom
#set xrange [80:120]
#set xtics offset -10,0
#set yrange [0:1.6e8]

#set style data histogram
#set style histogram cluster gap 1 
#set style fill solid border -1 
#set boxwidth 0.9
#set xtic rotate by -45 scale 0
#set key left top 


plot "tmp.data"  using  2  title "forks"         ls 1, \
     "tmp.data"  using  3  title "joins"         ls 2, \
     "tmp.data"  using  4  title "working rank"  ls 3

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.joins-and-forks.png'
replot
clear

