# Simulation

Windows Gnuplot command 
===============


#設定輸出到終端機
set terminal windows
#auto scale
set autoscale 

set xlabel 'Number of users' font ",12"                                  # x-axis label
set ylabel 'Throughput' font ",12"  


#繪圖 single file
plot 'result-wifi.txt' using 2:7 w lp t "Binary Exponential Backoff"

#繪圖 Multiple files
plot 'result-wifi.txt' using 2:7 w lp t "Binary Exponential Backoff","p01.txt" using 2:7 w lp title "P=0.1","p05.txt" using 2:7 w lp title "P=0.5","p09.txt" using 2:7 w lp title "P=0.9"



# References 
1) Gnuplot (http://gnuplot.sourceforge.net/)