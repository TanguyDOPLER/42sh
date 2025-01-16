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

find . -name "42sh" -exec rm -f {} \;
find . -name "Makefile" -exec rm -f {} \;
find . -name "Makefile.in" -exec rm -f {} \;
find . -name "lib*" -exec rm -f {} \;
find . -name ".deps" -exec rm -rf {} \;


echo "$(tput setaf 2)DONE!$(tput sgr0)"
