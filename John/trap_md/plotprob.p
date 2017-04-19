set terminal postscript eps color lw 4 "Helvetica" 20 size 5in, 4in
set out 'prob.eps'
set ylabel 'Probability of Chain Order Stability'
set xlabel 'Ion Temperature (K)'
set title 'Probability of Chain Stability During Doppler Recooling' 
plot 'out52' every ::1 u (0.5*138*1.66e-27*$1*$1/1.38e-23):(1-$2) t '5 ions, 2 Yb' lt 1 with lines, \
	'data52' every ::1 u (0.00670+$1*3.7847e-5):($2) t '' lt 1, \
	'out73' every ::1 u (0.5*138*1.66e-27*$1*$1/1.38e-23):(1-$2) t '7 ions, 3 Yb' lt 2 with lines, \
	'data73' every ::1 u (0.00+$1*7.89e-5):($2) t '' lt 2, \
	'out91' every ::1 u (0.5*138*1.66e-27*$1*$1/1.38e-23):(1-$2) t '9 ions, 1 Yb' lt 3 with lines, \
	'data91' every ::1 u (0.00+$1*2.049e-4):($2) t '' lt 3
