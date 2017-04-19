set terminal postscript eps color lw 4 "Helvetica" 20 size 5in, 4in
set out 'data.eps'
set ylabel 'Probability of Chain Order Stability'
set xlabel 'Dark Time (ms)'
set title 'Probability of Chain Stability During Doppler Recooling' 
plot 'data52' every ::1 u ($1):($2) t '5 ions, 2 Yb' lt 1 w lp, \
	'data73' every ::1 u ($1):($2) t '7 ions, 3 Yb' lt 2 w lp, \
	'data91' every ::1 u ($1):($2) t '9 ions, 1 Yb' lt 3 w lp
