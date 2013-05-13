set SRC=..\..\platform\de1-nios-my
copy %SRC%\my_sopc.sopc sopc
xcopy /Y /E %SRC%\ssd1963 ssd1963
copy %SRC%\DE1_NIOS.v top