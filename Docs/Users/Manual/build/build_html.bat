@echo off

call configuration.bat

set docbook_inputfile=..\Frhed_Help.xml
set docbook_use_stylesheet=build_html.xsl
set docbook_outputdir=%docbook_build_path%\html

if not exist "%docbook_outputdir%" mkdir "%docbook_outputdir%"

echo Copy images...
if not exist "%docbook_outputdir%\images" mkdir "%docbook_outputdir%\images"
copy "..\images\*.gif" "%docbook_outputdir%\images\."

echo Copy screenshots...
if not exist "%docbook_outputdir%\screenshots" mkdir "%docbook_outputdir%\screenshots"
copy "..\screenshots\*.*" "%docbook_outputdir%\screenshots\."

echo Copy stylesheets...
if not exist "%docbook_outputdir%\css" mkdir "%docbook_outputdir%\css"
copy "..\css\all.css" "%docbook_outputdir%\css\all.css"
copy "..\css\print.css" "%docbook_outputdir%\css\print.css"

echo Create HTML files...
%docbook_java_exe% %docbook_java_parameters% -cp %docbook_saxon_jar%;%docbook_xerces_jar%;%docbook_saxon_xsl% -Djavax.xml.parsers.DocumentBuilderFactory=%DBFACTORY% -Djavax.xml.parsers.SAXParserFactory=%SPFACTORY% -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%XINCLUDE% com.icl.saxon.StyleSheet %docbook_inputfile% %docbook_use_stylesheet% base.dir=%docbook_outputdir%\

echo Finished!

:end
@echo on