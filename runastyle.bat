REM Run Astyle to format source code
REM Get Astyle from:
REM http://astyle.sourceforge.net/
REM $Id$

REM Frhed is Ansi-style - tab-indent
astyle --style=ansi --indent=force-tab --pad-oper --unpad-paren --max-instatement-indent=2 --keep-one-line-blocks Frhed/*.cpp
astyle --style=ansi --indent=force-tab --pad-oper --unpad-paren --max-instatement-indent=2 --keep-one-line-blocks Frhed/*.h

astyle --style=ansi --indent=force-tab --pad-oper --unpad-paren --max-instatement-indent=2 --keep-one-line-blocks rawio32/*.cpp
astyle --style=ansi --indent=force-tab --pad-oper --unpad-paren --max-instatement-indent=2 --keep-one-line-blocks rawio32/*.h
