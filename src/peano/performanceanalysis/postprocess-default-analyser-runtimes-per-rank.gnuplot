# author: Tobias Weinzierl



set style line 11 lc rgb "#ff6600" lt 1 pt 7   lw 1.0 ps 1.6 
set style line 12 lc rgb "#00ff66" lt 1 pt 9   lw 1.0 ps 1.6 
set style line 13 lc rgb "#6600ff" lt 1 pt 8   lw 1.0 ps 1.6 
set style line 14 lc rgb "#888888" lt 1 pt 11  lw 1.0 ps 1.6 

set style line 21 lc rgb "#333333" lt 1 pt 6   lw 1.4 ps 1.6 
set style line 22 lc rgb "#222222" lt 2 pt 8   lw 1.4 ps 1.6 
set style line 23 lc rgb "#111111" lt 1 pt 10  lw 1.4 ps 1.6 
set style line 24 lc rgb "#000000" lt 2 pt 12  lw 1.4 ps 1.6 


set grid

set xlabel 'runtime'
set ylabel 'time [t]=s'


set style fill solid border -1 


unset logscale y
set title "Rank ".RANK." (cpu time)"
set key left top

plot "tmp.data"  using  ($1):(($5)+($9)+($7)+($3))   every :::RANK::RANK    title "idle"                           with boxes ls 14, \
     "tmp.data"  using  ($1):(($9)+($7)+($3))        every :::RANK::RANK    title "boundary exchange/management"   with boxes ls 13, \
     "tmp.data"  using  ($1):(($7)+($3))             every :::RANK::RANK    title "join"                           with boxes ls 12, \
     "tmp.data"  using  ($1):(($3))                  every :::RANK::RANK    title "busy"                           with boxes ls 11, \
     "tmp.data"  using  ($1):11                      every :::RANK::RANK    notitle                                with lines ls 21, \
     "tmp.data"  using  ($1):13                      every :::RANK::RANK    notitle                                with lines ls 22, \
     "tmp.data"  using  ($1):15                      every :::RANK::RANK    notitle                                with lines ls 23, \
     "tmp.data"  using  ($1):16                      every :::RANK::RANK    notitle                                with lines ls 24, \
     "tmp.data"  using  ($1):11                      every 11:::RANK::RANK  title "synchronous heap data"          with points ls 21, \
     "tmp.data"  using  ($1):13                      every 13:::RANK::RANK  title "asynchronous heap data"         with points ls 22, \
     "tmp.data"  using  ($1):15                      every 15:::RANK::RANK  title "work within domain"             with points ls 23, \
     "tmp.data"  using  ($1):16                      every 17:::RANK::RANK  title "wait for workers"               with points ls 24

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.cpu.png'
replot

set log y
set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.log.cpu.png'
replot

clear


unset logscale y
set title "Rank ".RANK." (calendar time)"
set key left top


plot "tmp.data"  using  ($1):(($4)+($8)+($6)+($2))   every :::RANK::RANK    title "idle"                           with boxes        ls 14, \
     "tmp.data"  using  ($1):(($8)+($6)+($2))        every :::RANK::RANK    title "boundary exchange/management"   with boxes ls 13, \
     "tmp.data"  using  ($1):(($6)+($2))             every :::RANK::RANK    title "join"                           with boxes ls 12, \
     "tmp.data"  using  ($1):(($2))                  every :::RANK::RANK    title "busy"                           with boxes ls 11, \
     "tmp.data"  using  ($1):10                      every :::RANK::RANK    notitle                                with lines ls 21, \
     "tmp.data"  using  ($1):12                      every :::RANK::RANK    notitle                                with lines ls 22, \
     "tmp.data"  using  ($1):14                      every :::RANK::RANK    notitle                                with lines ls 23, \
     "tmp.data"  using  ($1):16                      every :::RANK::RANK    notitle                                with lines ls 24, \
     "tmp.data"  using  ($1):10                      every 11:::RANK::RANK  title "synchronous heap data"          with points ls 21, \
     "tmp.data"  using  ($1):12                      every 13:::RANK::RANK  title "asynchronous heap data"         with points ls 22, \
     "tmp.data"  using  ($1):14                      every 15:::RANK::RANK  title "work within domain"             with points ls 23, \
     "tmp.data"  using  ($1):16                      every 17:::RANK::RANK  title "wait for workers"               with points ls 24

set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.calendar.png'
replot

set log y
set terminal png font "/usr/share/fonts/dejavu/DejaVuSans.ttf" size 1200,900
set output 'tmp.runtimes.log.calendar.png'
replot
