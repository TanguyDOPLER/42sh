echo "$(tput setaf 1)removing everything created by autotool$(tput sgr0)"

rm -rf autom4te.cache
rm -f aclocal.m4
rm -f ar-lib compile configure depcomp install-sh missing
rm -f config.*
rm -f depcomp
rm -f install-sh
rm -f missing
rm -f configure\~
rm -f src/42sh-main.o
rm -f tests/*.out
rm -f tests/atconfig
rm -f src/*.out
rm -f *.out
rm -f tests/echo 
rm -f tests/files.txt
rm -f tests/testo.txt 

find . -name "42sh" -exec rm -f {} \;
find . -name "Makefile" -exec rm -f {} \;
find . -name "Makefile.in" -exec rm -f {} \;
find . -name "lib*" -exec rm -f {} \;
find . -name ".deps" -exec rm -rf {} \;

rm -f tests/'$*'
rm -f tests/'&2'
rm -f tests/2
rm -f tests/'echo'
rm -f tests/error_log.txt
rm -f tests/existing_file.txt
rm -f tests/file_err
rm -f tests/filendon
rm -f tests/files
rm -f tests/files.txt
rm -f tests/first
rm -f tests/hello
rm -f tests/is
rm -f tests/ok
rm -f tests/ok2
rm -f tests/other
rm -f tests/output
rm -f tests/output.txt
rm -f tests/several
rm -f tests/testo.txt
rm -f tests/test.txt
rm -f tests/this
rm -f tests/tst
rm -f tests/'$a'
rm -f tests/'$b'
rm -f tests/'$file'
rm -f tests/combined_output.txt
rm -rf tests/ok



echo "$(tput setaf 2)DONE!$(tput sgr0)"
