# author: Tobias Weinzierl



set style line 11 lc rgb "#ff6600" lt 1 pt 7   lw 1.0 ps 1.6 
set style line 12 lc rgb "#ff6600" lt 1 pt 6   lw 1.0 ps 1.6 
set style line 21 lc rgb "#00ff66" lt 1 pt 9   lw 1.0 ps 1.6 
set style line 22 lc rgb "#00ff66" lt 1 pt 8   lw 1.0 ps 1.6 
set style line 31 lc rgb "#6600ff" lt 1 pt 11  lw 1.0 ps 1.6 
set style line 32 lc rgb "#6600ff" lt 1 pt 10  lw 1.0 ps 1.6 
set style line 41 lc rgb "#ff2222" lt 1 pt 13  lw 1.0 ps 1.6 
set style line 42 lc rgb "#ff2222" lt 1 pt 12  lw 1.0 ps 1.6 
set style line 51 lc rgb "#22ff22" lt 1 pt 15  lw 1.0 ps 1.6 
set style line 52 lc rgb "#22ff22" lt 1 pt 14  lw 1.0 ps 1.6 
set style line 61 lc rgb "#2222ff" lt 1 pt 5   lw 1.0 ps 1.6 
set style line 62 lc rgb "#2222ff" lt 1 pt 4   lw 1.0 ps 1.6 
set style line 71 lc rgb "#667766" lt 1 pt 3   lw 1.0 ps 1.6 
set style line 72 lc rgb "#667766" lt 1 pt 2   lw 1.0 ps 1.6 
set style line 81 lc rgb "#000000" lt 1 pt 2   lw 1.0 ps 1.6 
set style line 82 lc rgb "#334433" lt 1 pt 1   lw 1.0 ps 1.6 



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

plot "tmp.data"  using  1:2                       title "busy"                     with points ls 11, \
     "tmp.data"  using  1:(($4)+($6)+($8))        title "idle and communication"   with points ls 21, \
     "tmp.data"  using  1:(($2)+($4)+($6)+($8))   title "total"                    with points ls 31   

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

plot "tmp.data"  using  1:3                       title "busy"                          with points ls 12, \
     "tmp.data"  using  1:(($5)+($7)+($9))        title "idle and communication"        with points ls 22, \
     "tmp.data"  using  1:(($3)+($5)+($7)+($9))   title "total"                         with points ls 32
   

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.cpu.png'
replot

set log y
set title "Overview (cpu time)"
set key left bottom

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.log.cpu.png'
replot







unset logscale y
set title "Global Master (calendar time)"
set key left top

plot "tmp.data"  using  1:2                       every :::0::1  notitle                            with lines ls 11, \
     "tmp.data"  using  1:4                       every :::0::1  notitle                            with lines ls 21, \
     "tmp.data"  using  1:6                       every :::0::1  notitle                            with lines ls 31, \
     "tmp.data"  using  1:8                       every :::0::1  notitle    with lines ls 41, \
     "tmp.data"  using  1:10                      every :::0::1  notitle           with lines ls 51, \
     "tmp.data"  using  1:12                      every :::0::1  notitle         with lines ls 61, \
     "tmp.data"  using  1:(($4)+($6)+($8))        every :::0::1  notitle          with lines ls 71, \
     "tmp.data"  using  1:(($2)+($4)+($6)+($8))   every :::0::1  notitle                           with lines ls 81, \
     "tmp.data"  using  1:2                       every 10:::0::1  title "busy "                           with points ls 11, \
     "tmp.data"  using  1:4                       every 11:::0::1  title "idle "                           with points ls 21, \
     "tmp.data"  using  1:6                       every 12:::0::1  title "join "                           with points ls 31, \
     "tmp.data"  using  1:8                       every 13:::0::1  title "boundary exchange/management "   with points ls 41, \
     "tmp.data"  using  1:10                      every 14:::0::1  title "synchronous heap data "          with points ls 51, \
     "tmp.data"  using  1:12                      every 15:::0::1  title "asynchronous heap data  "        with points ls 61, \
     "tmp.data"  using  1:(($4)+($6)+($8))        every 16:::0::1  title "idle and communication "         with points ls 71, \
     "tmp.data"  using  1:(($2)+($4)+($6)+($8))   every 17:::0::1  title "total "                          with points ls 81
     

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.master.calendar.png'
replot

set log y
set title "Global Master (calendar time)"
set key left bottom

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.master.log.calendar.png'
replot

clear



unset logscale y
set title "Global Master (cpu time)"
set key left top

plot "tmp.data"  using  1:3                       every :::0::1  notitle                                 with lines ls 12, \
     "tmp.data"  using  1:5                       every :::0::1  notitle                                 with lines ls 22, \
     "tmp.data"  using  1:7                       every :::0::1  notitle                                 with lines ls 32, \
     "tmp.data"  using  1:9                       every :::0::1  notitle         with lines ls 42, \
     "tmp.data"  using  1:11                      every :::0::1  notitle               with lines ls 52, \
     "tmp.data"  using  1:13                      every :::0::1  notitle              with lines ls 62, \
     "tmp.data"  using  1:(($5)+($7)+($9))        every :::0::1  notitle               with lines ls 72, \
     "tmp.data"  using  1:(($3)+($5)+($7)+($9))   every :::0::1  notitle                                with lines ls 82, \
     "tmp.data"  using  1:3                       every 10:::0::1  title "busy "                                with points ls 12, \
     "tmp.data"  using  1:5                       every 11:::0::1  title "idle "                                with points ls 22, \
     "tmp.data"  using  1:7                       every 12:::0::1  title "join "                                with points ls 32, \
     "tmp.data"  using  1:9                       every 13:::0::1  title "boundary exchange/management "        with points ls 42, \
     "tmp.data"  using  1:11                      every 14:::0::1  title "synchronous heap data  "              with points ls 52, \
     "tmp.data"  using  1:13                      every 15:::0::1  title "asynchronous heap data  "             with points ls 62, \
     "tmp.data"  using  1:(($5)+($7)+($9))        every 16:::0::1  title "idle and communication "              with points ls 72, \
     "tmp.data"  using  1:(($3)+($5)+($7)+($9))   every 17:::0::1  title "total "                               with points ls 82
     

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.master.cpu.png'
replot

set log y
set title "Global Master (cpu time)"
set key left bottom

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.master.log.cpu.png'
replot

clear
