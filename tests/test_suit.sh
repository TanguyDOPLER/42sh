REF_OUT="ref.out"
TEST_OUT="test.out"
test="42sh"
green_c='\033[0;32m'
red_c='\033[0;31m'
default='\033[0m'

testcase() {
    echo -e -n "[OK BG]Test $1"
    (echo "$1" | bash --posix) > "$REF_OUT"
    (echo "$1" | ../src/42sh) > "$TEST_OUT"
    echo -e ${red_c}
    diff -u "$REF_OUT" "$TEST_OUT"
    if [ $? -eq 0 ]; then
        echo -e -n ${green_c}
        echo "$TEST_NUMBER : JUSTE"
    else
        echo "$TEST_NUMBER :"
    fi 
    TEST_NUMBER=$(($TEST_NUMBER+1))
    echo -e ${default}
}

testcase "echo test"
testcase "true"
testcase "false"
testcase "echo test;"
testcase "echo test ;"
testcase "echo test ; echo test2"
testcase "echo test ;echo test2 ; "
testcase "test ;echo test2 ; "
testcase "echo 1; echo 2; echo 3; echo 4;"
testcase "echo 1; echo 2; echo 3; echo 4"
testcase "if true then echo ok fi"
testcase "if true; then echo ok; fi"
testcase "if false; true; then echo ok; fi"
testcase "if true then echo ok else echo non fi"
testcase "if true; then echo ok; else echo non; fi"
testcase "if false; true; then echo ok1; echo ok2 else echo non1 ; echo non2; fi"
testcase "if false; true; then echo ok1; echo ok2; else echo non1 ; echo non2; fi"
testcase "if true; then echo ok; fi"
testcase "if false; then echo ok; else ls; fi"
testcase "if false; true; then
echo a
echo b; echo c;
fi"
testcase "if false
true
then
echo a
echo b; echo c
fi"
testcase ""
testcase "if false; true; then
if false; then 
    echo echo a;
else
    echo b;
fi
else
echo b; echo c
fi"
testcase ""
testcase ""
testcase ""
testcase ""
testcase ""
testcase ""
testcase ""
testcase ""
testcase ""
testcase ""


