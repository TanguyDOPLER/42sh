#!/bin/sh

# Couleurs
RED='\033[31m'
GREEN='\033[32m'
YELLOW='\033[33m'
BLUE='\033[34m'
CYAN='\033[36m'
BOLD='\033[1m'
RESET='\033[0m'

# Répertoire temporaire pour les tests
TMP_DIR=$(mktemp -d /tmp/42sh_tests.XXXXXX)
BASH_OUT="$TMP_DIR/bash_out"
BASH_ERR="$TMP_DIR/bash_err"
BASH_RED="$TMP_DIR/bash_dir"
SH_OUT="$TMP_DIR/sh_out"
SH_ERR="$TMP_DIR/sh_err"
SH_RED="$TMP_DIR/sh_dir"

# Chemin vers votre shell 42sh
TEST_SHELL="$BIN_PATH"

# Compteur de tests
TESTS_RUN=0
TESTS_SUCCESS=0

# Fonction pour afficher une bannière
print_header() {
    echo "${CYAN}${BOLD}=== $1 ===${RESET}"
}

# Fonction pour exécuter un test
run_test() {
    local description="$1"
    local command="$2"

    TESTS_RUN=$((TESTS_RUN+1))

    # Exécution dans /bin/bash
    echo "$command" | bash --posix >"$BASH_OUT" 2>"$BASH_ERR"
    local bash_ret=$?

    # Exécution dans 42sh
    echo "$command" | $VALGRIND $TEST_SHELL >"$SH_OUT" 2>"$SH_ERR"
    local sh_ret=$?

    # Comparaison des résultats
    if diff -q "$BASH_OUT" "$SH_OUT" >/dev/null && (([ -n "$BASH_ERR" ] && [ -n "$SH_ERR" ]) || ([ -z "$BASH_ERR" ] && [ -z "$SH_ERR" ])) && [ "$bash_ret" -eq "$sh_ret" ]; then
        echo "${GREEN}[OK]${RESET} $description" 
        TESTS_SUCCESS=$((TESTS_SUCCESS+1))
    else
        echo "${RED}[FAIL]${RESET} $description"
        echo "${YELLOW}Command:${RESET} $command"
        echo "${BLUE}--- Expected stdout ---${RESET}"
        cat -e "$BASH_OUT"
        echo "${BLUE}--- Actual stdout ---${RESET}"
        cat -e "$SH_OUT"
        echo "${BLUE}--- Expected stderr ---${RESET}"
        cat "$BASH_ERR"
        echo "${BLUE}--- Actual stderr ---${RESET}"
        cat "$SH_ERR"
        echo "${BLUE}Expected return code:${RESET} $bash_ret"
        echo "${BLUE}Actual return code:${RESET} $sh_ret"
        echo
    fi
}

run_test2() {
    local description="$1"
    local command="$2"

    TESTS_RUN=$((TESTS_RUN+1))

    # Exécution dans /bin/bash
    echo "$command" | bash --posix >"$BASH_OUT" 2>"$BASH_ERR"
    local bash_ret=$?
    cat -e files.txt > $BASH_RED
    # Exécution dans 42sh
    echo "$command" | $TEST_SHELL >"$SH_OUT" 2>"$SH_ERR"
    local sh_ret=$?
    cat -e files.txt > $SH_RED

    # Comparaison des résultats
    if diff -q "$BASH_OUT" "$SH_OUT" >/dev/null && (([ -n "$BASH_ERR" ] && [ -n "$SH_ERR" ]) || ([ -z "$BASH_ERR" ] && [ -z "$SH_ERR" ])) && [ "$bash_ret" -eq "$sh_ret" ] && diff -q "$BASH_RED" "$SH_RED" > /dev/null;  then
        echo "${GREEN}[OK]${RESET} $description"
        TESTS_SUCCESS=$((TESTS_SUCCESS+1))
    else
        echo "${RED}[FAIL]${RESET} $description"
        echo "${YELLOW}Command:${RESET} $command"
        echo "${BLUE}--- Expected stdout ---${RESET}"
        cat "$BASH_OUT"
        echo "${BLUE}--- Actual stdout ---${RESET}"
        cat "$SH_OUT"
        echo "${BLUE}--- Expected stderr ---${RESET}"
        cat "$BASH_ERR"
        echo "${BLUE}--- Actual stderr ---${RESET}"
        cat "$SH_ERR"
        echo "${BLUE}Expected return code:${RESET} $bash_ret"
        echo "${BLUE}Actual return code:${RESET} $sh_ret"
        echo
    fi
}

run_test_file() {
    local description="$1"
    local command="$2"

    TESTS_RUN=$((TESTS_RUN+1))

    # Exécution dans /bin/bash
    bash --posix $command >"$BASH_OUT" 2>"$BASH_ERR"
    local bash_ret=$?
    # Exécution dans 42sh
    $TEST_SHELL $command >"$SH_OUT" 2>"$SH_ERR"
    local sh_ret=$?

    # Comparaison des résultats
    if diff -q "$BASH_OUT" "$SH_OUT" >/dev/null && (([ -n "$BASH_ERR" ] && [ -n "$SH_ERR" ]) || ([ -z "$BASH_ERR" ] && [ -z "$SH_ERR" ])) && [ "$bash_ret" -eq "$sh_ret" ]; then
        echo "${GREEN}[OK]${RESET} $description"
        TESTS_SUCCESS=$((TESTS_SUCCESS+1))
    else
        echo "${RED}[FAIL]${RESET} $description"
        echo "${YELLOW}Command:${RESET} $command"
        echo "${BLUE}--- Expected stdout ---${RESET}"
        cat -e "$BASH_OUT"
        echo "${BLUE}--- Actual stdout ---${RESET}"
        cat -e "$SH_OUT"
        echo "${BLUE}--- Expected stderr ---${RESET}"
        cat "$BASH_ERR"
        echo "${BLUE}--- Actual stderr ---${RESET}"
        cat "$SH_ERR"
        echo "${BLUE}Expected return code:${RESET} $bash_ret"
        echo "${BLUE}Actual return code:${RESET} $sh_ret"
        echo
    fi
}

# Début des tests
print_header "TESTS POUR 42SH - STEP 1"


run_test "Simple AND success" "echo ok && echo success"
run_test "Simple AND failure" "false && echo should_not_display"
run_test "AND with true success" "true && echo success"
run_test "Multiple AND success" "echo first && echo second && echo third"
run_test "Multiple AND with true" "true && echo ok && true && echo done"
run_test "AND with semicolon at the end" "echo first && echo second;"
run_test "AND with semicolon in the middle" "echo first; echo second && echo third"
run_test "AND with newline (valid)" "echo first &&\necho second"
run_test "AND with newline and true" "true &&\necho success"
run_test "AND with newline (unfinished)" "echo first &&\n"
run_test "Mixed AND and semicolon" "echo first && echo second; echo third"
run_test "Mixed AND, semicolon, and newline" "echo first && echo second;\necho third"
run_test "AND with true and semicolon" "true && echo start; echo end"
run_test "AND with invalid command" "echo ok && invalid_command"
run_test "AND with unfinished command" "echo ok &&"
run_test "AND with semicolon and unfinished command" "echo ok; echo second &&"
run_test "AND with operator typo" "echo ok & && echo fail"
run_test "AND with incorrect spacing" "echo ok& &echo fail"
run_test "AND with ls and pwd" "ls && pwd"
run_test "AND with true before pwd" "true && pwd"
run_test "AND with false before pwd" "false && pwd"
run_test "AND with cat on nonexistent file" "echo ok && cat nonexistent_file && echo should_not_display"
run_test "Nested AND success" "echo ok && echo nested && echo success"
run_test "Nested AND with true" "true && echo nested && echo success"
run_test "Nested AND failure" "echo ok && false && echo should_not_display"
run_test "Long AND with newline" "echo first &&\necho second &&\necho third"
run_test "AND with partial success" "echo ok && echo success && false && echo not_reached"
run_test "AND with failure in the middle" "echo start && false && echo not_reached && echo end"
run_test "Empty command with AND" "&& echo fail"
run_test "AND starting with semicolon" "; && echo fail"
run_test "AND ending with semicolon" "echo ok &&;"
run_test "AND with operator malformation" "echo ok &&& echo fail"
run_test "AND with partial output" "echo visible && false && echo invisible"
run_test "AND with chained success" "echo first && echo second && echo third && echo fourth"
run_test "AND with true and failure" "true && false && echo should_not_display"
run_test "Simple OR success" "echo ok || echo should_not_display"
run_test "Simple OR failure" "false || echo success"
run_test "OR with one failure" "false || false || echo fallback"
run_test "Multiple OR success" "false || echo first || echo second"
run_test "OR with semicolon at the end" "false || echo first;"
run_test "OR with semicolon in the middle" "false; echo first || echo second"
run_test "OR with newline (valid)" "false ||\necho success"
run_test "OR with newline and true" "true ||\necho should_not_display"
run_test "OR with newline (unfinished)" "echo ok ||\n"
run_test "Mixed OR and semicolon" "false || echo first; echo second"
run_test "Mixed OR, semicolon, and newline" "false || echo first;\necho second"
run_test "OR with true and semicolon" "true || echo should_not_display; echo done"
run_test "OR with invalid command" "false || invalid_command"
run_test "OR with unfinished command" "false ||"
run_test "OR with semicolon and unfinished command" "false; echo first ||"
run_test "OR with operator typo" "false ||| echo fail"
run_test "OR with incorrect spacing" "false| |echo fail"
run_test "OR with ls and pwd" "false || ls || pwd"
run_test "OR with true before pwd" "true || pwd"
run_test "OR with false before pwd" "false || pwd"
run_test "OR with cat on nonexistent file" "false || cat nonexistent_file || echo fallback"
run_test "Nested OR success" "false || echo nested || echo fallback"
run_test "Nested OR with true" "true || echo nested || echo should_not_display"
run_test "Nested OR failure" "false || false || echo success"
run_test "Long OR with newline" "false ||\necho first ||\necho second"
run_test "OR with partial success" "false || echo success || false || echo fallback"
run_test "OR with failure in the middle" "false || echo first || false || echo second"
run_test "Empty command with OR" "|| echo fail"
run_test "OR starting with semicolon" "; || echo fail"
run_test "OR ending with semicolon" "false ||;"
run_test "OR with operator malformation" "false ||| echo fail"
run_test "OR with partial output" "false || echo visible || echo should_not_display"
run_test "OR with chained success" "false || echo first || echo second || echo third"
run_test "OR with true and success" "true || echo should_not_display || echo not_reached"
run_test "If with true condition" "if true; then echo success; fi"
run_test "If with false condition" "if false; then echo success; else echo failure; fi"
run_test "If with command success" "if echo ok; then echo success; fi"
run_test "If with command failure" "if false; then echo should_not_display; else echo fallback; fi"
run_test "If with AND condition" "if echo ok && true; then echo success; fi"
run_test "If with OR condition" "if false || echo fallback; then echo success; fi"
run_test "Nested if with true conditions" "if true; then if echo nested; then echo success; fi; fi"
run_test "Nested if with false inner condition" "if true; then if false; then echo should_not_display; else echo fallback; fi; fi"
run_test "Nested if with false outer condition" "if false; then if true; then echo should_not_display; fi; else echo outer_fallback; fi"
run_test "Nested if with AND inside" "if true; then if echo inner && true; then echo success; fi; fi"
run_test "Nested if with OR inside" "if false; then echo should_not_display; else if false || echo fallback; then echo success; fi; fi"
run_test "If with missing then" "if echo ok; echo fail; fi"
run_test "If with missing fi" "if echo ok; then echo fail"
run_test "If with invalid syntax" "if echo ok; then; echo success; fi"
run_test "If with unfinished condition" "if echo ok &&; then echo fail; fi"
run_test "If with extra fi" "if true; then echo success; fi; fi"
run_test "If with missing else body" "if false; then echo fail; else; fi"
run_test "If with ls success" "if ls; then echo directory_listed; fi"
run_test "If with ls failure" "if ls nonexistent_dir; then echo should_not_display; else echo fallback; fi"
run_test "If with pwd success" "if pwd; then echo success; fi"
run_test "If with true in AND" "if true && echo ok; then echo success; fi"
run_test "If with false in AND" "if false && echo fail; then echo should_not_display; fi"
run_test "If with true in OR" "if false || echo fallback; then echo success; fi"
run_test "If with false in OR" "if true || echo should_not_display; then echo success; fi"
run_test "Nested if with syntax error" "if true; then if; then echo fail; fi; fi"
run_test "Nested if with missing fi" "if true; then if true; then echo success; fi"
run_test "Nested if with missing then" "if true; if true; then echo success; fi; fi"
run_test "If with multiline commands" "if true; then echo first_line; echo second_line; fi"
run_test "If with multiline nested if" "if true; then if echo nested; then echo success; fi; fi"
run_test "If with AND in condition" "if echo ok && true; then echo success; fi"
run_test "If with OR in condition" "if false || echo ok; then echo success; fi"
run_test "If with AND and OR combined" "if echo ok && true || false; then echo success; fi"
run_test "If with failure in AND and fallback" "if false && echo fail; then echo should_not_display; else echo fallback; fi"
run_test "If with invalid operator" "if echo ok & echo fail; then echo success; fi"
run_test "If with invalid nested body" "if true; then if false; then; else echo fallback; fi fi"
run_test "If with invalid AND syntax" "if echo ok &&; then echo fail; fi"
run_test "If with semicolon error" "if true; then echo success; else echo fail; else echo extra; fi"
run_test "If with elif and success" "if false; then echo fail; elif true; then echo success; fi"
run_test "If with multiple elif conditions" "if false; then echo fail; elif false; then echo not_reached; elif true; then echo success; fi"
run_test "If with elif and else" "if false; then echo fail; elif false; then echo not_reached; else echo fallback; fi"
run_test "Elif with ls success" "if false; then echo fail; elif ls; then echo directory_listed; fi"
run_test "Elif with ls failure" "if false; then echo fail; elif ls nonexistent_dir; then echo not_reached; else echo fallback; fi"
run_test "Elif with pwd success" "if false; then echo fail; elif pwd; then echo success; fi"
run_test "If with elif and true" "if false; then echo fail; elif true; then echo success; fi"
run_test "If with elif and false" "if false; then echo fail; elif false; then echo not_reached; else echo fallback; fi"
run_test "If with elif true and AND condition" "if false; then echo fail; elif echo ok && true; then echo success; fi"
run_test "If with elif false and OR condition" "if false; then echo fail; elif false || echo fallback; then echo success; fi"
run_test "Nested if with elif success" "if false; then echo outer_fail; elif true; then if true; then echo inner_success; fi; fi"
run_test "Nested if with elif and failure" "if false; then echo outer_fail; elif true; then if false; then echo not_reached; else echo inner_fallback; fi; fi"
run_test "Nested elif with multiple conditions" "if false; then echo fail; elif false; then echo not_reached; elif true; then if true; then echo nested_success; fi; fi"
run_test "If with elif missing then" "if false; then echo fail; elif true; echo success; fi"
run_test "If with elif missing fi" "if false; then echo fail; elif true; then echo success"
run_test "If with elif invalid syntax" "if true; then echo ok; elif; then echo fail; fi"
run_test "If with multiple elif and missing fi" "if false; then echo fail; elif false; then echo not_reached; elif true; then echo success"
run_test "If with elif and extra fi" "if true; then echo success; elif false; then echo fail; fi; fi"
run_test "If with elif and invalid body" "if true; then echo success; elif echo missing_condition; fi"
run_test "Elif with multiline commands" "if false; then echo first_line; elif true; then echo second_line; fi"
run_test "Elif with nested multiline if" "if false; then echo outer_fail; elif true; then if echo nested; then echo success; fi; fi"
run_test "If with elif and AND condition" "if false; then echo fail; elif echo ok && true; then echo success; fi"
run_test "If with elif and OR condition" "if false; then echo fail; elif false || echo fallback; then echo success; fi"
run_test "If with multiple elif and mixed conditions" "if false; then echo fail; elif echo condition1 && false; then echo not_reached; elif echo condition2 || true; then echo success; fi"
run_test "Elif with invalid operator" "if false; then echo fail; elif echo ok & echo fail; then echo success; fi"
run_test "Elif with missing fi" "if false; then echo fail; elif true; then echo success"
run_test "Elif with invalid AND syntax" "if false; then echo fail; elif echo ok &&; then echo fail; fi"
run_test "Elif with semicolon error" "if false; then echo fail; elif true; then echo success; else echo fail; else echo extra; fi"
run_test "Elif with missing then" "if false; then echo fail; elif true echo success; fi"
run_test "Simple echo with single quotes" "echo 'Hello, World!'"
run_test "Preserve multiple spaces" "echo '   Many   spaces   '"
run_test "Special characters in single quotes" "echo '!@#$%^&*()_+'"
run_test "Two commands with ;" "echo 'First command'; echo 'Second command'"
run_test "Single quotes with newline" "echo 'Hello, 
World!'"
run_test "Combine pwd with echo" "pwd | echo 'This is the current directory'"
run_test "Unclosed single quote" "echo 'Unclosed quote"
run_test "Incomplete command" "echo 'Incomplete command; "
run_test "Double semicolon failure" "echo 'Hello';; echo 'World'"
run_test "Literal variable in single quotes" "echo 'This is \$USER'"
run_test "Literal command substitution in single quotes" "echo 'Result of \$(ls)'"
run_test "Single quotes within single quotes" "echo 'Single quote in a command: '\''test'\'''"
run_test "Single quotes with semicolon and newline" "echo 'First line'; echo 'Second line'
echo 'Third line'"
run_test "ls with non-existent directory" "ls 'nonexistent-folder'"
run_test "Basic variable assignment and usage" "a=42 && echo \$a"
run_test "Assign and use multiple variables" "a=hello && b=world && echo \$a \$b"
run_test "Variable substitution" "a=123 && b=\$a && echo \$b"
run_test "Variable with empty value" "a='' && echo \$a"
run_test "Undefined variable usage" "echo \$undefined_var"
run_test "Incorrect variable name" "1var=42 && echo \$1var"
run_test "Special character in variable name" "val\\=eur=42 && echo \$val\\=eur"
run_test "Using variable in forbidden context" "\$a=42 && echo \$a"
run_test "Multiple assignments on one line" "a=42; b=hello; c=world && echo \$a \$b \$c"
run_test "Command chaining with variables" "a=ok && echo \$a && false && echo not_reached"
run_test "Using variable in an external binary" "file=test.txt && echo 'content' > \$file && cat \$file"
run_test "Expansion with curly brackets" "a=world && echo Hello \${a}!"
run_test "Variable in loop" "b='one two three' && for a in \$b; do echo \$a; done"
run_test "Complex redirection with variables" "file=output.txt && echo 'test content' > \$file && cat \$file"
run_test "Error code with undefined variable" "echo \$undefined_var && echo \$?"
run_test "Assign and print variable before and after command" "a=3 && echo \$a && a=4 && echo \$a"
run_test "Variable with spaces and quotes" "a='Hello World' && echo \$a"
run_test "Substitution in nested commands" "a=3 && b=\$a && echo \$b && { a=4; echo \$a; }"
run_test "Use of $?" "false && echo \$? || echo \$?"
run_test "Exit status of the last command (\$?)" "false && echo \$?"
run_test "Exit status after success (\$?)" "true && echo \$?"
run_test "Current shell process ID (\$\$)" "echo \$\$ && ps -p \$\$"
run_test "Number of arguments passed (\$#)" "echo \$# && echo one two three && echo \$#"
run_test "Generate random number (\$RANDOM)" "echo \$RANDOM && echo \$RANDOM && echo \$RANDOM"
run_test "Current user ID (\$UID)" "echo \$UID && id -u"
run_test "Previous working directory (\$OLDPWD)" "cd / && echo \$OLDPWD && cd - && echo \$OLDPWD"
run_test "Current working directory (\$PWD)" "echo \$PWD && pwd"
#run_test "Internal Field Separator (\$IFS)" "echo \"\$IFS\" | hexdump -C && echo \$'word1\nword2' | while IFS= read -r line; do echo \$line; done"
run_test "Combine \$? and \$PWD" "ls && echo \$? && cd /tmp && echo \$PWD"
run_test "Combine \$RANDOM and \$UID" "echo \$UID && echo \$RANDOM && echo \$RANDOM"
run_test "Manipulate \$OLDPWD" "cd / && cd \$OLDPWD && echo \$PWD && echo \$OLDPWD"
run_test "Test process ID with \$\$" "echo \$\$ && ps -p \$\$ && bash -c 'echo \$\$'"
run_test "Invalid manipulation of \$UID" "UID=1000 && echo \$UID"
run_test "Invalid \$RANDOM usage" "RANDOM=123 && echo \$RANDOM"
run_test "Invalid \$? usage" "\$?=0 && echo \$?"
run_test "Invalid \$PWD modification" "PWD=/home && echo \$PWD"
run_test "Nested commands with \$?" "false && echo \$? && true && echo \$?"
run_test "Validate directory switches using \$OLDPWD" "echo \$OLDPWD && echo \$OLDPWD"
run_test "Simple double quotes" "echo \"Hello World\""
run_test "Double quotes with variable expansion" "FOO=bar ; echo \"The value of FOO is \$FOO\""
run_test "Double quotes with command substitution" "echo \"We are in directory: \$(pwd)\""
run_test "Double quotes with backslash" "echo \"Une ligne\\nUne autre ligne\""
run_test "Wildcard inside double quotes (no expansion)" "echo \"*.c\""
run_test "Multiple expansions" "NAME=Bob ; echo \"Hello \$NAME, you are in \$(pwd)\""
run_test "Quotes within quotes" "echo \"He said: \\\"Bonjour\\\"\""
run_test "Empty double quotes" "echo \"\""
run_test "Variable assignment with double quotes" \"VAR=\"Hello World\" ; echo \$VAR\"
run_test "Incomplete double quotes (should fail)" "echo \"Oops"
run_test "Unmatched double quotes (should fail)" "echo \"Begin\" \"Middle \"End"
run_test "Double quotes with special var \$?" "false ; echo \"Last exit was \$?\" ; true ; echo \"Now it's \$?\""
run_test "For loop: liste fixe" \
"for i in 1 2 3; do echo \"Valeur: \$i\"; done"
run_test "For loop: variable expansion" \
"LISTE=\"alpha beta gamma\"; for mot in \$LISTE; do echo \"Mot: \$mot\"; done"
run_test "For loop: wildcard sur *.c" \
"for fichier in *.c; do echo \"Fichier C: \$fichier\"; done"
run_test "For loop: substitution de commande" \
"for user in \$(echo \"user1 user2\"); do echo \"Utilisateur: \$user\"; done"
run_test "For loop: liste vide" \
"for i in ; do echo \"Ne devrait pas s'afficher\"; done"
run_test "For loop: break" \
"for i in 1 2 3 4 5; do [ \$i -eq 3 ] && break; echo \"i=\$i\"; done"
run_test "For loop: continue" \
"for i in 1 2 3 4 5; do [ \$i -eq 3 ] && continue; echo \"i=\$i\"; done"
run_test "For loop: nested" \
"for i in 1 2; do for j in A B; do echo \"i=\$i, j=\$j\"; done; done"
run_test "For loop KO: pas de do" \
"for i in 1 2 3; echo \"Oups\"; done"
run_test "For loop KO: pas de done" \
"for i in 1 2 3; do echo \"Oups\""
run_test "For loop KO: pas de point-virgule" \
"for i in 1 2 3 do echo \"Manque un point-virgule\"; done"
run_test "For loop: liste avec guillemets" \
"for i in \"hello world\" \"42\"; do echo \"Item: \$i\"; done"
run_test "For loop: guillemets simples" \
"for i in 'un' 'deux trois' 'quatre'; do echo \"[\$i]\"; done"
run_test "For loop: syntaxe multiline" \
"for i in 1 2 \
3 4; do \
echo \"Multiline: \$i\"; \
done"
run_test "For loop: bloc multiple" \
"for i in 1 2; do echo \"i=\$i\"; echo \"Carré=\$((i*i))\"; done"
run_test "For loop: redirection stdout" \
"for i in 1 2 3; do echo \"\$i\" > /dev/null; done"
run_test "For loop: incrémentation interne" \
"count=0; for i in A B C; do count=\$((count+1)); echo \"i=\$i, count=\$count\"; done"
run_test "For loop: if statement" \
"for i in 1 2 3; do if [ \$i -eq 2 ]; then echo \"C'est deux\"; else echo \"C'est pas deux\"; fi; done"
run_test "For loop: variable MAJ" \
"VARLIST=\"X Y Z\"; for ELEMENT in \$VARLIST; do echo \"Element: \$ELEMENT\"; done"
run_test "For loop: do sur ligne suivante" \
"for i in 1 2 3 \
do echo \"OK: \$i\"; done"
run_test "For loop: do inline" \
"for i in 10 20 30; do echo \"Inline: \$i\"; done"
run_test "For loop: zéro itération" \
"for i in ''; do echo \"Ne devrait pas être vide ? \$i\"; done"
run_test "For loop: expansions multiples" \
"FOO=hello; BAR=world; for i in \$FOO \$BAR \$(echo 123); do echo \"\$i\"; done"
run_test "For loop: substitution multiple" \
"for token in \$(echo \"un deux trois\"); do echo \"token=\$token\"; done"
run_test "For loop: break en nested" \
"for i in 1 2; do for j in a b c; do echo \"i=\$i j=\$j\"; [ \$j = b ] && break; done; done"
run_test "For loop: itérer sur \$@" \
"for arg in \"\$@\"; do echo \"Argument: \$arg\"; done"
run_test "For loop: expansion *" \
"for i in *; do echo \"Fichier/Répertoire: \$i\"; done"
run_test "For loop: cat d'un fichier inexistant (devrait potentiellement échouer)" \
"for line in \$(cat /fichier_qui_nexiste_pas_ou_pas); do echo \"Ligne: \$line\"; done"
run_test "Negation d'une commande OK (devrait échouer)" \
"! true"
run_test "Negation d'une commande KO (devrait réussir)" \
"! false"
run_test "Negation dans if: fichier inexistant" \
"if [ ! -e /fichier_inexistant ]; then echo \"Fichier non présent\"; else echo \"Fichier présent ?\"; fi"
run_test "Negation dans if: /tmp doit exister" \
"if [ ! -d /tmp ]; then echo \"KO\"; else echo \"/tmp est un répertoire\"; fi"
run_test "Negation dans test: variable non vide" \
"FOO=hello; if [ ! -z \"\$FOO\" ]; then echo \"FOO n'est pas vide\"; else echo \"FOO est vide\"; fi"
run_test "Negation avec &&" \
"! (true && echo \"Cette commande reussit\")"
run_test "Negation avec ||" \
"! (false || echo \"Cette commande finit par reussir\")"
run_test "While avec negation" \
"i=0; while ! [ \$i -ge 3 ]; do echo \"i=\$i\"; i=\$((i+1)); done"
run_test "Negation d'une substitution: ls sur dossier inexistant" \
"! ls /chemin/qui/nexiste/pas > /dev/null 2>&1"
run_test "if ! commande; then ... else ..." \
"if ! false; then echo \"Negation de false => OK\"; else echo \"Negation a echoue ?\"; fi"
run_test "Double negation ! ! false" \
"! ! false"
run_test "Negation d'une commande introuvable" \
"! commande_inexistante_42sh_test"
run_test "Combiner [ ! -e fichier ] && echo ..." \
"[ ! -e /fichier_inexistant ] && echo \"Le fichier n'existe pas\""
run_test "Negation d'un pipeline: ! cat /etc/passwd | grep root" \
"! cat /etc/passwd | grep root"
run_test "if ! grep root /etc/passwd..." \
"if ! grep 'root' /etc/passwd; then echo \"root n'est pas trouve\"; else echo \"root est trouve\"; fi"
run_test "And basic" "echo ok && echo ok"
run_test "And basic not" "! echo ok && echo ok"
run_test "Or basic" "echo ok || echo ok"
run_test "Or basic not" "! echo ok || echo ok"
run_test "Long and" "echo ok && echo ok && ls && echo \$PWD && cat file && echo ok"
run_test "Tst no space" "echo OK&&echo ok||echo no"
run_test "Example SCL" "false && echo foo || echo bar; true || echo foo && echo bar"
run_test "with if true" "if echo a && true; then echo ok; else echo ko; fi"
run_test "with if false" "if echo a && false; then echo ok; else echo ko; fi"
run_test "with if true and negation" "if echo a && ! true; then echo ok; else echo ko; fi"
run_test "with if false and negation" "if echo a && ! false; then echo ok; else echo ko; fi"
run_test "bad or pipe" "echo hehe || | cat; echo e |||"
run_test "bad or ;" "true || ; echo; || ;"
run_test "Check and new line" "echo ok &\\; & echo ok; echo ok |\\; | echo ok"
run_test "Comment escaped" "echo \\#escaped"
run_test "Comment quoted" "echo '#'escaped"
run_test "Comment end" "echo not#first"
run_test "Comments hard" "echo \\#escaped \"#\"quoted not#first #commented"
run_test "Comment escaped (duplicate)" "echo \\#escaped"
run_test "Comment quoted (duplicate)" "echo '#'escaped"
run_test "Comment end (duplicate)" "echo not#first"
run_test "Comments hard (duplicate)" "echo \\#escaped \"#\"quoted not#first #commented"
run_test "Empty comment" "echo \"\""
run_test "Comment with special characters" "echo \"#$%^&*()\""
run_test "Multiple comments in one line" "echo \"Multiple comments\""
run_test "Comment with spaces" "echo \"Space after comment\""
run_test "Comment within a string" "echo \"This is a #comment within a string\""
run_test "Comment after a command" "echo \"Hello World\""
run_test "Nested comments" "echo \"Outer comment\""
run_test "Comment with escaped characters" "echo \"Escaped comment\" \\# This is an escaped comment"
run_test "Comment with quoted characters" "echo \"Quoted comment\" '#' This is a quoted comment"
run_test "Comment with special characters and spaces" "echo \"Special chars and spaces\""
run_test "Comment at the beginning of the line" "echo \"Line with a comment at the beginning\""
run_test "Comment with numbers" "echo \"Numbers in comment\""
run_test "Comment with a mix of characters" "echo \"Mix of characters\""
run_test "Comment with newline character" "echo \"Newline in comment\""
run_test "LS with a comment" "ls"
run_test "Find command with a comment" "find . -name \"*.txt\""
run_test "Grepping with a comment" "grep \"pattern\" file.txt"
run_test "AWK command with a comment" "awk '{print \$1}' data.txt"
run_test "SED command with a comment" "sed 's/old/new/' input.txt"
run_test "Sort command with a comment" "sort file.txt"
run_test "Cut command with a comment" "cut -f1,2 data.csv"
run_test "WC command with a comment" "wc -l textfile.txt"
run_test "Find with a comment" "find . -name \"*.tmp\""
run_test "Check out comment backslashed" "echo ok; echo backslashed comment"
run_test "continue simple" "for i in true false true; do if \$i; then continue; fi; echo toto; done"
run_test "Nested for loop continue 1" "for i in true false true; do echo Outer loop one; for j in true true false true true; do echo Inner loop one; if ! \$j; then continue 1; fi; echo Inner loop two; echo Outer loop two; done; done"
run_test "Nested for loop continue 2" "for i in true false true; do echo Outer loop one; for j in true true false true true; do echo Inner loop one; if ! \$j; then continue 2; fi; echo Inner loop two; echo Outer loop two; done; done"
#run_test "Using while loop" "echo Using while loop; i=true; while \$i; do echo Outer loop \$i; j=true; while \$j; do echo Inner loop \$j; k=true; while \$k; do echo Deep loop \$k; if \$k; then echo Continuing 3 loops...; k=false; j=false; i=false; continue 3; elif \$j; then echo Breaking 5 loops...; k=false; j=false; i=false; break 5; elif \$i; then echo Continuing...; k=false; j=false; continue; elif \$j; then echo Breaking 2 loops...; k=false; j=false; break 2; fi; done; done; done"
#run_test "Using for loop" "echo Using for loop:; for l in true true true; do echo Outer loop iteration: \$l; m=true; while \$m; do echo Inner loop iteration: \$m; if \$l; then echo Breaking 2 loops...; m=false; break 2; fi; done; done"
#run_test "Using until loop" "echo Using until loop:; n=true; until ! \$n; do echo Outer loop iteration: \$n; o=true; until ! \$o; do echo Inner loop iteration: \$o; if \$n; then echo Continuing...; o=false; continue; fi; done; n=false; done"
#run_test "Test out overflow" "while true; do while true; do break 4; done; done; echo ok"
run_test "Test continue" "while mkdir ok; do while true; do echo ok; continue 2; echo ok; done; echo ok; done; echo end; while mkdir ok; do touch ok/ls ok/cat; while true; do break 2; ls; done; echo ok; done"
run_test "My echo toto" "echo toto"
run_test "My echo tata" "echo tata"
run_test "Echo spaces" "echo There '' is '' spaces"
run_test "Long echo" "echo 'fuenfiue iun iuenf iuenf iunefi ufni uenfi uenfiu nefiun'"
run_test "Very long echo" "echo efnienfuen iunfeiu neifun eifun ieunfi ue    fieunfiune iun iu neif"
run_test "Echo reserved" "echo if fi then echo ifa fia thena;"
run_test "Echo true false" "echo true || echo false && ! echo other && echo flo"
run_test "Paraph echo" \"echo Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras commodo lobortis tempor. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut euismod imperdiet nulla vel ornare. In cursus vel lectus nec scelerisque. Integer vestibulum tellus eu vehicula pharetra. Praesent porttitor libero id justo commodo consequat. Nunc nisi quam, interdum eu leo a, placerat tempor orci. Morbi nec dui vitae nisi molestie condimentum et scelerisque elit. Proin eget tempus ex, et sagittis ligula.\"
run_test "Echo without E option" "echo -E 'hello world \\n'"
run_test "Echo with e option" "echo -e 'hello world \\n'"
run_test "Echo with e default option" "echo 'hello world \\n'"
run_test "Echo with several e E options 1" "echo -e -E -e -E 'hello world \\n'"
run_test "Echo with several e E options 2" "echo -e -E -e 'hello world \\n'"
run_test "Echo with n option" "echo -n 'hello world'"
run_test "Double backslash n no option" "echo -e '\\n\\n'"
run_test "Double backslash n option e" "echo -e '\\n\\n'"
run_test "Double backslash n option E" "echo -E '\\n\\n'"
run_test "echo with spaces" "echo      4    2    s    h"
run_test "Lots of backslash" "echo -EeE \\\\\\\\\\\\\\\\"
run_test "Spaces between arguments" "echo 4\\t 2 s \\th"
run_test "Big arguments" "echo fuck your malloc 10000; echo aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
run_test "Slashs" "echo -e -E -e \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"
run_test "Slashs no spaces" "echo -eEe \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"
run_test "Slashs 2.0" "echo -E -e -E \\\\\\\\\\\\\\\\\\\\"
run_test "Two echos" "echo first; echo second"
run_test "Non option dash" "echo -a"
run_test "Dash with other char" "echo -neEnneEEnea 42\\nsh\\\\"
run_test "Non setting dash and setting dash" "echo -neEnnEeen -nef 42sh\\n\\\\"
run_test "Double option set" "echo -nEEennneE -nnneee 42sh\\n\\\\"
run_test "Quoted tricky one" "echo toto ';' ls"
run_test "interpret newline" "echo -nnEeennnEe 42'\\nsh'; echo ok"
run_test "wordables" "echo echo echo  fi if else elif"
run_test "negation" "echo   biz    ; echo yipee yep; echo hello world !; echo 1 2 3 4 5 6"
run_test "Lots of dash option" "echo -n -n -e -e -n -n -n -e 42'\\nsh'"
run_test "Empties n" "echo -n; echo -e; echo -E; echo -Een"
run_test "Long command list" "echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto;echo toto; echo toto ;echo toto;"
run_test "Echo with bsl n" "echo This should be\\discard; echo \"This should be\\discard\"; echo 'This should be\\discard'"
run_test "e c h o" "echo e c h o"
run_test "Not backslashed" "echo 'This is unclosed\\' here"
run_test "Check with spaces" "echo a \\   b \\ \\ c    \\ a\\ v  eeee\\"
run_test "Echo weird quotes" "echo \"\"; echo this \"\" is \"\" a \"\" test; echo ok \"\"; echo \"\" ok"
run_test "Unexpected end of file on echo" "echo 'Special characters: \$@ \\ / \` ' \\\" '"
run_test "Echo with space var" "a=a' '; echo \$a\\b; a=' '; echo \$a\\b"
run_test "Huge string to echo" "echo 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum vitae lorem ... (truncated)'"
run_test "Echo 10000" "echo fuck your malloc\\ aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa... (truncated)"
run_test "Error no binary" "bidule ok"
run_test "Error assigning var number" "3ok=No"
run_test "Error assigning var underscore" "_ok=No"
run_test "invalid condition" "if jfjf; then echo 1; fi"
run_test "if (while) 2 is correct exit code" "if while false; do echo deez nutz; done; then :; fi"
run_test "error while1 2 is correct exit code" "while do"
run_test "error while2 2= correct exit code" "while ! true; do echo"
run_test "empty compoud list element1" "echo hehe ; ;"
run_test "empty compoud list element2" "; ;"
run_test "bad list seperator" "echo \"Apple\" ;; echo \"Banana\""
run_test "bad seperator2" "echo \"One\" && ; echo \"Two\" || ; echo \"Three\""
run_test "bad seperator3" "cat file.txt |;; grep \"pattern\""
run_test "bad seperator4" "echo \"A\" ;;;; echo \"B\""
run_test "bad seperator5" "echo \"Redirected\" &;; output.txt"
run_test "compound list in if" "if false; then true; then echo ok\\true; ; then echo\\ls; fi"
run_test "bad_unterminated_quote" "echo 'hdhdh"
run_test "Unclosed" "echo \"This is unclosed\\\" here"
run_test "If quotes" "if ''; then echo ok; fi"
run_test "Unterminated simple quotes" "echo 'Hello Wor"
run_test "empty command" ""
run_test "only spaces in command" "\"     \""
run_test "Unexpected EOF Var" "echo \${a"
run_test "Pipe with an error 1" "echo ok | thisisnotright | cat | ls"
run_test "Pipe with an error 2" "oeifeojf | echo ok | cat | wc -l"
run_test "Only |" "|"
run_test "Only ;" ";"
run_test "Only &" "&"
run_test "No word after redir cmd" "echo this is a test >"
run_test "No word before redir cmd" "> & echo this is a test"
run_test "No word before redir sh" "> & if true; then echo this is a test; fi"
run_test "No word after redir sh" "if true; then echo this is a test; fi >"
run_test "normal builtin" "echo toto"
run_test "ls" "ls"
run_test "tree" "tree"
run_test "with a complex command" "find . -name \"*.txt\" -cat {} \\;"
run_test "with a long command" "echo \"This is a very long command with lots of arguments and options\""
run_test "with multiple commands on one line" "echo \"Command 1\"; echo \"Command 2\""
run_test "Basic exit" "exit 0"
run_test "Basic exit returning before" "echo ok; exit 0; echo not"
run_test "Exit with code" "echo This is a 34; cat testfile; exit 34; echo Error"
run_test "simple" "a=b; echo \$a"
run_test "Var echo" "a=echo; \$a hello world"
run_test "No item for" "for i in; do echo toto; done"
run_test "No assignment (for)" "for i in a b c; do echo toto; done;"
run_test "No assignment multiple cmds" "for i in a b c; do echo toto; echo tata; done"
run_test "echo item list" "for i in a b c d e f g; do echo \"\$i\"; done"
run_test "for if" "for i in true false true; do if \$i; then echo yes; else echo no; fi; done"
run_test "find in for" "for i in \"*.txt\" \"*.c\" \"*.sh\"; do find . -name \"\$i\"; done"
run_test "returns error in for" "for i in true false; do \$i; done;"
run_test "Basic if" "if true; then echo ok; fi"
run_test "Basic if false" "if false; then echo ok; fi"
run_test "Basic if else" "if false; then echo ok; else echo not; fi"
run_test "Basic if else false" "if true; then echo ok; else echo not; fi"
run_test "Basic if else lots of condition" "if true; false; true; echo ok; true; then echo ok; else echo not; fi"
run_test "Test elif FF" "if false; then echo ok; elif false; then echo elif; else echo yes; fi"
run_test "Test elif FT" "if false; then echo ok; elif true; then echo elif; else echo yes; fi"
run_test "Test fi newline" "if false; then echo ok; fi"
run_test "Lots of if else" "if false; then echo ok; elif false; then echo ok; elif false; then echo ok; elif false; then echo ok; elif false; then echo ok; elif false; then echo ok; else echo not; fi"
run_test "Invalid condition if" "if fnj; then echo yeye; fi"
run_test "Counpoumd list" "if true; then echo Never; echo Gonna; echo Give; echo You; echo Up; fi"
run_test "shange token" "if false; then echo else; elif true; then echo else; fi"
run_test "If else new line" "if echo this; then echo is; elif false; then echo nice; else echo false; fi"
run_test "If else new line all negated" "if ! echo this; then ! echo is; elif ! false; then ! echo nice; else ! echo false; fi"
run_test "If nested huge" "if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then if true; then echo '\n\nLorem ipsum ...'; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi; fi"
run_test "Negate easy" "if ! true; then echo SHOULDNT; fi"
run_test "Negate true easy" "if ! false; then echo SHOULD; fi"
run_test "Negate in elif easy" "if false; then echo \$PWD; elif ! false; then ls; fi"
run_test "Negate command" "if ! echo ok; then echo double; fi"
run_test "Negate return builtin" "! echo ok"
run_test "Negate return builtin 2" "! ls"
run_test "double negation true" "! ! true"
run_test "triple negation true" "! ! ! true"
run_test "double negation false" "! ! false"
run_test "triple negation false" "! ! ! false"
run_test "negation with spaces" "!        true"
run_test "negation with spaces double" "!       ! true"
run_test "Double negation on echo" "! ! echo ok"
run_test "Echo piped" "echo ok | cat"
run_test "Echo piped lots of cat" "echo ok | cat | cat | cat | cat | cat"
run_test "Pipe using binaries #1" "touch this is several files; find -type f | grep '*s*' | cat"
run_test "Pipe using binaries #2" "echo testoutput > ok; cat ok | cat; cat ok | grep 'test'"
run_test "Pipe using binaries #3" "echo testoutput > file; cat file | echo; cat file | grep 'test'"
run_test "Count the number of line in a file" "echo -e \"There is \\n several lines\\n\\nHere\" > file; cat file | wc -l | cat; cat file | wc -c | cat"
run_test "Generate random letter" "tree | tr -dc 'A-Z' | head -n 1 | wc -c"
run_test "Simple output redirection" "echo \"This is the script part\" > output.txt"
run_test "Appending to a file" "echo \"This is first line\" > existing_file.txt; echo \"This is the script part\" >> existing_file.txt"
run_test "Redirecting STDERR" "cat not_exist 2> error_log.txt; cat error_log.txt"
run_test "Redirecting both STDOUT and SRDERR" "echo \"This is the script part\" > combined_output.txt 2>&1"
run_test "Input Redirection" "cat < input_file.txt"
run_test "First tricky redir" "echo hey 1>file"
run_test "Second tricky redir" "echo hey 1"
run_test "Backslashed 1" "echo hey 1 \\>2"
run_test "Backslashed 2" "echo hey 1 >\\&2"
run_test "Several redirs" "cat not_exist 1>file_err 2>&1; cat file_err"
run_test "Weird behavior redirs" "echo ok 1>&2 2>&1 1>/dev/null 2>/dev/null"
run_test "Hard redirs and pipe" "if echo ok 2>&1 1>&2 | cat; then echo ok 2>file | cat; fi | cat"
run_test "Echo ionumbers" "echo 3 4 55 > 2  2>file"
run_test "Two ways on cat" "echo \"Hey\" > first; cat <>first | wc -c; echo \"Test\" 2<>file; cat <>file"
run_test "Two ways on ls" "ls 2<>hello 1>&2"
run_test "Two ways tricky" "ls 4<>hello 1>&4 | cat <&4"
run_test "Cat in out" "echo \"This is a test\" >ok; <ok >ok2 cat; cat ok2"
run_test "Empty" ""
run_test "Empty quotes" "echo ''"
run_test "Multiple quotes" \"echo '1' '2' '3' '4' '5' '6' '7' '8' '9' '10' '11' '12' '13' '14' '15' '16' '17' '18' '19' '20' '21' '22' '23' '24' '25' '26' '27' '28' '29' '30' '31' '32' '33' '34' '35' '36' '37' '38' '39' '40' '41' '42' '43' '44' '45' '46' '47' '48' '49' '50' '51' '52' '53' '54' '55' '56' '57' '58' '59' '60' '61' '62' '63' '64' '65' '66' '67' '68' '69' '70' '71' '72' '73' '74' '75' '76' '77' '78' '79' '80' '81' '82' '83' '84' '85' '86' '87' '88' '89' '90' '91' '92' '93' '94' '95' '96' '97' '98' '99' '100' '101' '102' '103' '104' '105' '106' '107' '108' '109' '110' '111' '112' '113' '114' '115' '116' '117' '118' '119' '120' '121' '122' '123'\" 
run_test "Spaces as arg" "echo ' ' '  '"
run_test "Spaces in arg" "echo ' ' 'arg' ' '"
run_test "Simple quote" "echo 'simple quote'"
run_test "Semicolon quote" "echo hello ';' ls"
run_test "Attached to word" "echo 'hello'world"
run_test "Escape in quotes" "echo 'this \\$ is \\' a\\\" test \\\\' \"And this '\\'\\'' is as \\ well\\\""
run_test "Test with functioni" "a='echo    ok;'; \$a"
run_test "Echo diff var empty str" "echo this \$a '' \$a\$b this"
run_test "Basic variable" "a=3; b=4; echo \$a \$b"
run_test "Assignment quotes" "'val'=ok; echo \$val; val='ok'; echo \$val"
run_test "Shell variable test #1" "a=3; a=3 echo \$a; echo \$a; echo \$a a=4"
run_test "Variable does not exist" "echo \$a; echo a\$toto; echo \$b a\$b\\c a\$b \$b\\c"
run_test "Substitution" "a=3; b=\$a; echo \$b"
run_test "Variable brackets" "a=3; b=4; c=toto; echo \$a\$b\${c} Hello \${c}!; toto=\$a; echo \${a}here\${b} is \${toto}"
run_test "Var in external binary" "a=3; b=4; file=file; cat \$file >\$a; ls && cat \$a >\$b; rm \$a \$b; echo \$a\$b"
run_test "Test variable quotes" "toto=\"Hello World\"; echo 'this is \$toto hehe' \"This is the real \$toto\$\" \"Hello \\$toto\" >\"\$UID\"; cat <\"\$UID\""
run_test "Hard expansions" "a='sh    42'; echo 42\"\$a\"sh 42\$a\\sh \$a 42\$a"
run_test "Double equal" "how=varname=value; echo \"We print variable like that : \$how\""
run_test "Errors in assignment" "te\\}st=ok; test=ok; \\{test=ok; 'test'=ok; tes\\t=ok; \${ok}=ok; \${test}=ok; test=a; \$a=ok; \$test=ok; \${test}=ok; \${test}=ok"
run_test "Order of assignment" "x=\"oncex upon\" y=\"time\"; echo \$x \$y; a=\"oncea upon\" b=\"ime\" && echo \$a \$b; p=\"oncep upon\" v=\"me\" echo \$p \$v"
run_test "Check assignment before command" "testa=3 testabc=4 testabcef=5 testseofia='this is a test' printenv | grep 'test.*=' | sort"
run_test "Double alloc at once" "a=3 q=1; echo \$a \$q; a=4 echo \$a; echo \$a"
run_test "Args in variable" "a='-ne'; echo \$a    ok; b='-l'; ls \$b; c='ls -l'; \$c"
run_test "In for loop" "b=this is a test; c='this     as     well'; for a in \$b \$c \"\$c\"; do echo \$a; done"
run_test "redir string" "tat=1; echo ok \$tat>tst; cat tst; tat=twok; echo ok \$twok>other; cat other"
run_test "Nested echo" "a=3 bash -c 'echo \$a'"
run_test "Var redir output file" "a=2; echo this 2>&\$a; cat \$a"
run_test "Print args" "for a in \"\$*\"; do echo \$a; done; for a in \"\$@\"; do echo \$a; done"
run_test "question mark" "cat heheh; echo \$?"
run_test "Check IFS print" "echo \$IFS"
run_test "Brakets compound list non terminated" "a=3; { echo \$a; a=4; echo \$a }; echo \$a"
run_test "Nested bracekts compound list non terminated" "a=3; { echo \$a; { echo \$a; a=4; echo \$a } }; echo \$a"
run_test "Define unkown" "a=\$b; c=thisis\$d\\o"
run_test "Multiple definition per line" "a=3 b=4; echo \$a \$b"
run_test "Multiple words" "a=test text; echo \$a"
run_test "Commented content" "a=#123; echo \$a"
run_test "simple while" "while false; do echo ye silly gronk; done;"
run_test "if (while)" "if while false; do echo deez nutz; done; then echo nah; fi"
run_test "While auto stop" \"while\n    ls && who > output && true\n    echo 2<heylol || ls; rm file && echo \"ok\" > file\ndo\n    ls; cat file\ndone\"
run_test "Until simple" "n=true; until ! \$n; do echo toto; n=false; done"
run_test "Commande simple: echo" "echo 'Hello, World!'"
run_test "Commande conditionnelle: if-then-else" "if true; then echo OK; else echo Fail; fi"
run_test "Commande conditionnelle: if-then-elif-else" "if false; then echo Fail; elif true; then echo OK; else echo Fail; fi"
run_test "Liste de commandes avec ;" "echo foo; echo bar"
run_test "Liste de commandes avec ; final" "echo foo; echo bar;"
run_test "List de commandes avec erreur" "echo test; ech loupe; qsd"
run_test "Liste composée avec \\n" "if true; then echo OK; echo Still OK; fi"
run_test "Quotes simples" "echo 'This is a test'"
run_test "Quotes simples avec spécial" "echo 'Special # characters should not be treated as comments'"
run_test "Commentaires" "echo OK # This is a 'good' comment"
run_test "Commentaires complexes" "echo \\#not_a_comment # But this is a comment"
run_test "Builtin true" "true"
run_test "Builtin false" "false"
run_test "Builtin echo simple" "echo Hello"
run_test "Builtin echo avec -n" "echo -n 'No newline'"
run_test "Builtin echo avec -e" "echo -e 'Line1\\nLine2'"
run_test "Echo simple avec une chaîne courte" "echo Hello"
run_test "Echo avec chaîne longue" "echo This is a very long string to test the handling of long outputs by the shell"
run_test "Builtin true avec commande enchaînée" "true; echo Success"
run_test "Builtin false avec commande enchaînée" "false; echo Should not appear"
run_test "Echo avec caractères spéciaux" "echo 'Special characters: \$ \` # ! ~ \%'"
run_test "If simple avec true" "if true; then echo True condition; fi"
run_test "If simple avec false" "if false; then echo Should not appear; fi"
run_test "If-then-else avec true" "if true; then echo True branch; else echo False branch; fi"
run_test "If-then-else avec false" "if false; then echo True branch; else echo False branch; fi"
run_test "If-elif-else complexe" "if false; then echo First branch; elif true; then echo Second branch; else echo Final branch; fi"
run_test "If-elif-else toutes fausses" "if false; then echo First branch; elif false; then echo Second branch; else echo Final branch; fi"
run_test "If error case on condition" "if ech qsd; then echo camarche; fi"
run_test "Elif error case : missing then" "if false; then echo fqd; elif true; echo qsd; fi"
run_test "If without fi at then end" "if true; then echo test;"
run_test "if with retour à la ligne" "if false 
true
then 
    echo a 
    echo b; echo c 
fi"
run_test "commande after if" "if true; then echo coucou; fi echo qsd"
run_test "Commandes séparées par ;" "echo First; echo Second; echo Third"
run_test "Commandes enchaînées mélangeant true/false" "true; echo Passed; false; echo Failed"
run_test "Commandes enchaînées avec if" "if true; then echo In condition; fi; echo Outside condition"
run_test "Commande pwd" "pwd"
run_test "Commande date" "date"
run_test "Commande combinée avec echo" "echo 'Current directory:'; pwd"
run_test "Commande avec beaucoup d'échos (court)" "echo line1; echo line2; echo line3; echo line4; echo line5"
run_test "If-then-else complexe avec beaucoup de lignes" "if true; then echo Line1; echo Line2; echo Line3; else echo Failure; fi"
run_test "test Comment" "echo zizi 
echo zaza
echo testo #comment
echo after comment"
run_test "Commande simple: ls" "ls"
run_test "Commande simple avec option: ls -l" "ls -l"
run_test "Commande simple avec option: ls -a" "ls -a"
run_test "Commande simple avec option: ls -la" "ls -la"
run_test "Quotes doubles simples" "echo \"Double quotes test\""
run_test "Quotes doubles avec spécial" "echo \"Special \$ # characters inside quotes\""
run_test "Quotes imbriquées" "echo 'This \"is\" nested quotes'"
run_test "Quotes multiples" "echo 'One'; echo \"Two\"; echo 'Three'"
run_test "Condition avec plusieurs elif" "if false; then echo One; elif false; then echo Two; elif true; then echo Three; else echo Four; fi"
run_test "simple pipeline" "echo coucou | tr o e"
run_test "double pipeline" "echo coucou | tr o e | sort"
run_test "double pipeline V2" "ls | tr o e | sort"
run_test "quadruple pipeline" "ls | tr o e | sort | cat -e"
run_test "error case : missing first commande" "| tr o e"
run_test "error case : missing seconde commande" "echo coucou | "
run_test "error case : missing third commande" "echo coucou | tr o e |"
run_test "error case : missing fourth commande" "echo coucou | tr o e | sort |"
run_test "few commande between pipelines" "echo test echo coucou | tr o e; sort; ls | cat -e"
run_test "simple and" "echo 1 && echo 2"
run_test "simple and with builtin true" "true && echo 2"
run_test "simple and with builtinf false" "false && echo 2"
run_test "simple and with builtin true V2" "echo 2 && true"
run_test "simple and with builtinf false V2" "echo 2 && false"
run_test "simple error case" "ech test || echo camarche";
run_test "simple or" "echo 1 || echo 2"
run_test "simple or with builtin true" "true || echo 2"
run_test "simple or with builtinf false" "false || echo 2"
run_test "simple or with builtin true V2" "echo 2 || true"
run_test "simple or with builtinf false V2" "echo 2 || false"
run_test "few and" "echo 1 && echo 2 && echo 3"
run_test "few and" "echo 1 && echo 2 && echo 3 && echo 4"
run_test "few and with builtin true" "echo 1 && echo 2 && true && echo 4"
run_test "few and with builtin false" "echo 1 && echo 2 && false && echo 4"
run_test "few or" "echo 1 || echo 2 || echo 3"
run_test "few or" "echo 1 || echo 2 || echo 3 || echo 4"
run_test "few or with builtin V1" "false || echo 2 || true || echo 4"
run_test "few or with builtin V2" "false || false || echo 3 || echo 4"
run_test "few or with builtin V3" "false || false || true || echo 4"
run_test "associativity" "false && echo foo || echo bar"
run_test "associativity V2" "true || echo foo && echo bar"
run_test "simple builtin true" "! true"
run_test "simple builtin false" "! false"
run_test "simple commande" "! echo test"
run_test "simple commande fail" "! slqf"
run_test "if with negation" "! if true; then echo ok; fi"
run_test "if with negation V2" "! if false; then echo ok; fi"
run_test "elif with negation" "! if false; then echo ok; elif true; then echo ok2; else echo marcheaps; fi"
run_test2 "simple redir output" "echo test > files.txt"
run_test2 "simple redir outputV2" "echo test 3> files.txt"
run_test2 "simple redir outputV3" "echo test 30> files.txt"
run_test2 "simple redir with if" "if true; then echo test; fi 2> files.txt"
run_test2 "simple redir with if" "if true; then echo test; fi 2 > files.txt"
run_test2 "simple redir input" "echo test < files.txt"
run_test2 "test tkt ca passe" "22<> files.txt"
run_test2 "simple redir output and dup" "echo test >& files.txt"
run_test2 "simple redir <>" "echo test <> files.txt"
run_test2 "simple redir >|" "echo test >| files.txt"
run_test2 "simple redir >>" "echo 2>> files.txt"
run_test2 "simple redir <&" "echo test <&1000000000000000000; echo salut"
run_test2 "error case >" "echo test >"
run_test2 "error case wrong io number" "echo test 65498> files.txt"
run_test2 "error case >" "echo test >"
run_test2 "error case >" "echo test >"
run_test2 "error case input" "test echo"
run_test2 "error case ;;" "echo t ;;"
run_test2 "error case fi" "if true then echo test fi"
run_test2 "error case |" "echo |"
run_test2 "error case X" ""
run_test2 "simple redir <&" "echo test > echo > echo > echo >> testo.txt"
run_test2 "echo -e" "echo -e \\\n\\\t\a\l\u\t\ \l\e\s\ \e\n\f\a\n\t\; echo test"
run_test2 "comment" "echo not#first"
run_test2 "comment 2" "echo not #just a comment"
run_test2 "test buffer lexer" "echo ddddddddddddddddoihfaiodfosadfopdhofihasdofhopasdhopfhasdofoadshfoadsfoaodshfoashdofosadfhoasdhofoasdhfaodsifhoasdhfohadosfoadshfiohaosdhfoahsdfhadsfhdaoshfdhasofhodashfodahsofhoshfosdoifhasodhfoasdhfohsdofhodsfhosadhfhdsfhoiasdfoiashdfohsdofhodsfhoiasdfhoidashfohdsofhfhoohfishosadfohifsadhoidfoahisdfoahsohdfsohfdohfdaoshofdhhpifadphfdphfadhfdsohifadshadfsohiadfsohidfashipfdoishfopasdhfoipasdhfoihdsioufhaiosduhfisdhfiuashdofihaosdifhoasdhfoidhsfohdsofihdsfoivyhnovhnrytvoreyut0auvyntoufnoivtu98rantv9fdygvcgfonfydgoivhfgovdiogf"
run_test "while" "test=1; while $test; do echo test; done"
run_test "while 2" "test=3; while $test; do echo test; done"
run_test "while 3" "test=18; while $test; do echo $test; done"
run_test "test=4;test=5;echo $test";
run_test "test=4;test=5;echo $test" "5" "Affectation et réassignation de variable"
run_test "test var msg" "msg=world;echo hello_$msg"
run_test "no var" "echo $unset_var"
run_test "var int" "var=12;echo $var"
run_test "var 8" "test=3; echo $test; echo salut"
run_test "var 8" "test=3; echo $test; echo salut"
run_test "var 8" "test=3;pommedeterre=9; echo $test; echo salut"
run_test "var 8" "test=3;a=43;patate=3; echo $test; echo salut"
run_test "Simple exit" "exit 43"
run_test "Simple exit after command" "echo test; exit 23"
run_test "exit in a middle of a if" "if true; then echo test; exit 43; fi"
run_test "exit in a middle of a if on condition" "if true; exit 53; then echo test; exit 43; fi"
run_test "missing argument after exit" "echo test; exit; echo test; echo exit"
run_test "exit without arg but special return code" "false; exit"
run_test "exit without arg but special return code" "qsd; exit"
run_test "exit in subshell" " a=sh; (a=42; echo $a; exit 3; ls);echo $a ; exit 3"
run_test "exit in for loop" "for i in 1 2 3 4; do exit 3; done"

# Tests cd 
run_test "simple cd without arg" "cd ; pwd"
run_test "simple path" "cd /home/azec/Downloads/ ; pwd"
run_test "cd to root" "cd / ; pwd"
run_test "relative path" "cd ../ ; pwd"
run_test "relative path root" "cd ../../../../../../../ ; pwd"
run_test "relative path complicated" "cd ..//../ ; pwd"
run_test "relative path complicated" "cd ..//..///../ ; pwd"
run_test "simple folder" "cd Exec/ ; pwd"
run_test "complicated folder" "cd Exec/../Exec/../Exec/.deps ; pwd"
run_test "folder doesn't exist" "cd jenexistepas/ ; pwd"
run_test "folder doesn't exist with '/' at the begining" "cd /home/jenexistepas ; pwd"
run_test "simple path" "cd /home/azec/Downloads/ ; echo $OLDPWD; pwd"
run_test "relative path" "cd ../ ; echo $OLDPWD; pwd"
run_test "relative path root" "cd ../../../../../../../ ; echo $OLDPWD; pwd"
run_test "relative path complicated" "cd ..//../ ; echo $OLDPWD; pwd"
run_test "relative path complicated" "cd ..//..///../ ; echo $OLDPWD; pwd"
run_test "simple folder" "cd Exec/ ; echo $OLDPWD; pwd"
run_test "complicated folder" "cd Exec/../Exec/../Exec/.deps ; echo $OLDPWD; pwd"
run_test "folder doesn't exist" "cd jenexistepas/ ; echo $OLDPWD; pwd"
run_test "folder doesn't exist with '/' at the begining" "cd /jenexistepas/ ; echo $OLDPWD; pwd"
run_test "few cd in same commande" "cd ; pwd; cd /home/azec/42sh/; pwd ; cd src/ ; pwd ;cd ../ ; cd src/../src/Exec; pwd"

#Starting Test Efective
run_test "And basic" "echo ok && echo ok"

run_test "And basic not" "! echo ok && echo ok"

run_test "Or basic" "echo ok || echo ok"

run_test "Or basic not" "! echo ok || echo ok"

run_test "Long and" "echo ok && echo ok && ls && echo \$PWD && cat file && echo ok"

run_test "Test no space" "echo OK&&echo ok||echo no"

run_test "Example SCL: false and or" "false && echo foo || echo bar"
run_test "Example SCL: true or and" "true || echo foo && echo bar"

run_test "If true condition" "if echo a && true; then echo ok; else echo ko; fi"

run_test "If false condition" "if echo a && false; then echo ok; else echo ko; fi"

run_test "If true and negation" "if echo a && ! true; then echo ok; else echo ko; fi"

run_test "If false and negation" "if echo a && ! false; then echo ok; else echo ko; fi"

run_test "Bad or pipe" "echo hehe || | cat"
run_test "Bad or triple pipe" "echo e |||"

run_test "Bad or semicolon" "true || ;"
run_test "Echo and bad semicolon" "echo; || ;"

run_test "Check and newline with &\\" "echo ok &\\"
run_test "Followed by new line and and" "& echo ok"

run_test "Check and newline with pipe" "echo ok |\\"
run_test "Followed by new line and pipe" "| echo ok"





# Tests unset 
run_test "simple unset" "test=34; unset test ; echo $test"
run_test "multiple unset" "test=34; unset test ; unset test; echo $test;"
run_test "multiple unset" "test=34; test2=1024 unset test2 ; unset test; echo $test; echo $test2"
run_test "multiple unset" "test=34; test2=1024 unset test2 ; echo $test2 ; echo $test"
run_test "unset with option" "test=34; unset -v test ; echo $test"
run_test "unset with bad option" "test=34; unset -s test ; echo $test"

# Tests dot
run_test "dot simple file" "echo ca marche; . ./input ; echo fin"
run_test "dot fake" "echo coucou; ./input"
run_test "dot file doesn't exist" "echo debut; . ./jexistepas"

run_test_file "function1" "function1.sh"
run_test_file "function2" "function2.sh"
run_test_file "function3" "function3.sh"
run_test_file "function4" "function4.sh"
run_test_file "function5" "function5.sh"
run_test_file "function6" "function6.sh"
run_test_file "function7" "function7.sh"

run_test_file "loop5" "loop5.sh"
run_test_file "loop6" "loop6.sh"
run_test_file "loop7" "loop7.sh"
run_test_file "loop8" "loop8.sh"
run_test_file "loop9" "loop9.sh"
run_test_file "loop10" "loop10.sh"
run_test_file "loop11" "loop11.sh"
run_test_file "loop12" "loop12.sh"
run_test_file "loop13" "loop13.sh"

# Tests thilde expension
run_test "simple thilde" "echo ~"
run_test "simple thilde with slash" "echo ~/"
run_test "echo thilde with space" "echo ~/42sh: ~azec"
run_test "simple thilde with username" "echo ~azec"
run_test "simple thilde with fake username but one letter" "echo ~a"
run_test "simple thilde fake username" "echo ~existepas"
run_test "simple thilde slash folder name" "echo ~/test"
run_test "simple thilde folder name" "echo ~test"
run_test "thilde fater double point" "echo :~"
run_test "simple thilde with slash" "echo :~/"
run_test "simple thilde with username" "echo :~azec"
run_test "simple thilde fake username" "echo :~existepas"
run_test "simple thilde slash folder name" "echo :~/test"
run_test "simple thilde folder name" "echo :~test"


PERCENT=$(($TESTS_SUCCESS * 100 / $TESTS_RUN));
echo "$PERCENT" > "$OUTPUT_FILE";
cat "$OUTPUT_FILE";

# Résumé
echo
print_header "RÉSUMÉ DES TESTS"
echo -e "${BOLD}Tests passés :${RESET} ${GREEN}$TESTS_SUCCESS${RESET} / ${BOLD}$TESTS_RUN${RESET}"
if [ "$TESTS_SUCCESS" -eq "$TESTS_RUN" ]; then
    echo -e "${GREEN}Tous les tests ont réussi !${RESET}"
else
    echo -e "${RED}Certains tests ont échoué. Voir les détails ci-dessus.${RESET}"
fi

# Nettoyage
rm -rf "$TMP_DIR"
rm -rf several
rm -rf this
rm -rf file_err
rm -rf tst
rm -rf is
rm -rf files
rm -rf first
rm -rf '2'
rm -rf existing_file.txt
rm -rf other
rm -rf hello
rm -rf output
rm -rf ok2
rm -rf files.txt
rm -rf output.txt
rm -rf error_log.txt
rm -rf 'echo'
rm -rf atconfig
rm -rf '&2'
rm -rf filendon
rm -rf combined_output.txt
rm -rf testo.txt
rm -rf 'test.txt'
rm -rf '$a'
rm -rf '$file'
rm -rf '$b'
