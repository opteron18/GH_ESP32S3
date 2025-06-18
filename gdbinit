set remotetimeout 5
target remote localhost:3333
monitor reset halt
set remote hardware-watchpoint-limit 2
flushregs
thb app_main
c