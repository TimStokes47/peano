# author: Tobias Weinzierl



set style line 1 lc rgb "#ff6600" lt 1 pt 7   lw 1.0 ps 1.6 
set style line 2 lc rgb "#00ff66" lt 1 pt 9   lw 1.0 ps 1.6 
set style line 3 lc rgb "#6600ff" lt 1 pt 11  lw 1.0 ps 1.6 
set style line 4 lc rgb "#ff2222" lt 1 pt 13  lw 1.0 ps 1.6 



set grid

set xlabel 'runtime'
set ylabel 'time [t]=s'

set key left top

#set style data histogram
#set style histogram cluster gap 1 
#set style fill solid border -1 
#set boxwidth 0.9
#set xtic rotate by -45 scale 0
#set key left top 



unset log y
set title "Overview (calendar time)"
set key left top

plot "tmp.data"  using  1:2                             title "busy"                     with points ls 1, \
     "tmp.data"  using  1:4                             title "idle"                     with points ls 2, \
     "tmp.data"  using  1:(($6)+($8)+($10))             title "communication clean-up"   with points ls 3, \
     "tmp.data"  using  1:(($2)+($4)+($6)+($8)+($10))   title "total"                    with points ls 4   

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.calendar.png'
replot

set log y
set title "Overview (calendar time)"
set key left bottom

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.log.calendar.png'
replot


clear 

unset logscale y
set title "Overview (cpu time)"
set key left top

plot "tmp.data"  using  1:3                             notitle with points ls 1, \
     "tmp.data"  using  1:5                             notitle with points ls 2, \
     "tmp.data"  using  1:(($7)+($9)+($11))             notitle with points ls 3, \
     "tmp.data"  using  1:(($3)+($5)+($7)+($9)+($11))   notitle with points ls 4   
   

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.cpu.png'
replot

set log y
set title "Overview (cpu time)"
set key left bottom

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.log.cpu.png'
replot
