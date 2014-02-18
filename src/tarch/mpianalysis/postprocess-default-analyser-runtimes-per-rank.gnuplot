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
set style line 91 lc rgb "#ff9933" lt 0 pt 6   lw 1.6 ps 2.2 
set style line 92 lc rgb "#ff9933" lt 0 pt 7   lw 1.6 ps 2.2 
set style line 101 lc rgb "#aa3300" lt 0 pt 8   lw 1.6 ps 2.2 
set style line 102 lc rgb "#aa3300" lt 0 pt 9   lw 1.6 ps 2.2 


set grid

set xlabel 'runtime'
set ylabel 'time [t]=s'


#set style data histogram
#set style histogram cluster gap 1 
#set style fill solid border -1 
#set boxwidth 0.9
#set xtic rotate by -45 scale 0
#set key left top 


unset logscale y
set title "Rank ".RANK." (cpu time)"
set key left top

plot "tmp.data"  using  1:3                       every :::RANK::RANK  notitle                                 with lines ls 12, \
     "tmp.data"  using  1:5                       every :::RANK::RANK  notitle                                 with lines ls 22, \
     "tmp.data"  using  1:7                       every :::RANK::RANK  notitle                                 with lines ls 32, \
     "tmp.data"  using  1:9                       every :::RANK::RANK  notitle         with lines ls 42, \
     "tmp.data"  using  1:11                      every :::RANK::RANK  notitle               with lines ls 52, \
     "tmp.data"  using  1:13                      every :::RANK::RANK  notitle              with lines ls 62, \
     "tmp.data"  using  1:15                      every :::RANK::RANK  notitle              with lines ls 92, \
     "tmp.data"  using  1:(($5)+($7)+($9))        every :::RANK::RANK  notitle               with lines ls 72, \
     "tmp.data"  using  1:(($2)+($4)+($6)+($8))   every :::RANK::RANK  notitle                                with lines ls 82, \
     "tmp.data"  using  1:3                       every 10:::RANK::RANK  title "busy "                                with points ls 12, \
     "tmp.data"  using  1:5                       every 11:::RANK::RANK  title "idle "                                with points ls 22, \
     "tmp.data"  using  1:7                       every 12:::RANK::RANK  title "join "                                with points ls 32, \
     "tmp.data"  using  1:9                       every 13:::RANK::RANK  title "boundary exchange/management "        with points ls 42, \
     "tmp.data"  using  1:11                      every 14:::RANK::RANK  title "synchronous heap data  "              with points ls 52, \
     "tmp.data"  using  1:13                      every 15:::RANK::RANK  title "asynchronous heap data  "             with points ls 62, \
     "tmp.data"  using  1:15                      every 19:::RANK::RANK  title "work within domain  "                 with points ls 92, \
     "tmp.data"  using  1:(($5)+($7)+($9))        every 16:::RANK::RANK  title "idle and communication "              with points ls 72, \
     "tmp.data"  using  1:(($2)+($4)+($6)+($8))   every 17:::RANK::RANK  title "total "                               with points ls 82

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.cpu.png'
replot

set log y
set key left bottom
set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.log.cpu.png'
replot

clear


unset logscale y
set title "Rank ".RANK." (calendar time)"
set key left top

plot "tmp.data"  using  1:2                       every :::RANK::RANK  notitle                            with lines ls 11, \
     "tmp.data"  using  1:4                       every :::RANK::RANK  notitle                            with lines ls 21, \
     "tmp.data"  using  1:6                       every :::RANK::RANK  notitle                            with lines ls 31, \
     "tmp.data"  using  1:8                       every :::RANK::RANK  notitle    with lines ls 41, \
     "tmp.data"  using  1:10                      every :::RANK::RANK  notitle           with lines ls 51, \
     "tmp.data"  using  1:12                      every :::RANK::RANK  notitle         with lines ls 61, \
     "tmp.data"  using  1:14                      every :::RANK::RANK  notitle         with lines ls 91, \
     "tmp.data"  using  1:16                      every :::RANK::RANK  notitle         with lines ls 101, \
     "tmp.data"  using  1:(($4)+($6)+($8))        every :::RANK::RANK  notitle          with lines ls 71, \
     "tmp.data"  using  1:(($2)+($4)+($6)+($8))   every :::RANK::RANK  notitle                           with lines ls 81, \
     "tmp.data"  using  1:2                       every 10:::RANK::RANK  title "busy "                           with points ls 11, \
     "tmp.data"  using  1:4                       every 11:::RANK::RANK  title "idle "                           with points ls 21, \
     "tmp.data"  using  1:6                       every 12:::RANK::RANK  title "join "                           with points ls 31, \
     "tmp.data"  using  1:8                       every 13:::RANK::RANK  title "boundary exchange/management "   with points ls 41, \
     "tmp.data"  using  1:10                      every 14:::RANK::RANK  title "synchronous heap data "          with points ls 51, \
     "tmp.data"  using  1:12                      every 15:::RANK::RANK  title "asynchronous heap data  "        with points ls 61, \
     "tmp.data"  using  1:14                      every 15:::RANK::RANK  title "work within domain  "            with points ls 91, \
     "tmp.data"  using  1:16                      every 21:::RANK::RANK  title "wait time for workers (sum)"     with points ls 101, \
     "tmp.data"  using  1:(($4)+($6)+($8))        every 16:::RANK::RANK  title "idle and communication "         with points ls 71, \
     "tmp.data"  using  1:(($2)+($4)+($6)+($8))   every 17:::RANK::RANK  title "total "                          with points ls 81

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.calendar.png'
replot

set log y
set key left bottom
set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.log.calendar.png'
replot

clear
