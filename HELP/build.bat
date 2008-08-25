rem A batch file for building Frhed help file
rem This patch file requires that Help Compiler (hhc) and python are in PATH
rem $Id$

@echo off

set build_tmp=..\BuildTmp\Help
set build_dir=..\Build\Help
set help_file=frhed.chm

mkdir %build_tmp%
mkdir %build_dir%

del /s /q %build_dir%\%help_file%

python pydocgen.py frhed.cfg

copy *.bmp %build_tmp%
copy *.png %build_tmp%
copy *.css %build_tmp%
copy *.js %build_tmp%

cd %build_tmp%

hhc frhed.hhp

copy frhed.chm ..\..\Build\Help

cd ..\..\HELP
