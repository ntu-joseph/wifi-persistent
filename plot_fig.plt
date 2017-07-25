#set terminal postscript eps
#set output 'myfigure.eps'

set autoscale 
set title 'P-persistent Simulation (Channel=3)'  font ",16"       # plot title
set xlabel 'Probability' font ",12"                                  # x-axis label
set ylabel 'Throughput' font ",12"                         # y-axis label
#set yrange [0:1]
#set xrange [1:8]

set label 'Yi-Fang Chen' at 12,8
#N_CHANNEL,N_user,prob,total_T/MAX_N_Round);   

#set terminal x11
filename(m,n) = sprintf("C%dU%d.txt",m,n)

do for [c=1:19:2] {  
set title sprintf("P-persistent Simulation with Channel=%d",c)
   set terminal pngcairo
   outfile = sprintf('test%03.0f.png',c)
   set output outfile

print "plot".c
#do for [i=1:19:2] {print filename(c,i)}
plot for [i=1:19:2] filename(c,i) using 3:4 w lp t sprintf("user=%d",i)


}