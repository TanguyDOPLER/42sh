#!/usr/bin/env bash

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
TEST_SHELL="./42sh"

# Compteur de tests
TESTS_RUN=0
TESTS_SUCCESS=0

# Fonction pour afficher une bannière
print_header() {
    echo -e "${CYAN}${BOLD}=== $1 ===${RESET}"
}

# Fonction pour exécuter un test
run_test() {
    local description="$1"
    local command="$2"

    ((TESTS_RUN++))

    # Exécution dans /bin/bash
    echo -e "$command" | bash --posix >"$BASH_OUT" 2>"$BASH_ERR"
    local bash_ret=$?

    # Exécution dans 42sh
    echo -e "$command" | $TEST_SHELL >"$SH_OUT" 2>"$SH_ERR"
    local sh_ret=$?

    # Comparaison des résultats
    if diff -q "$BASH_OUT" "$SH_OUT" >/dev/null && (([ -n "$BASH_ERR" ] && [ -n "$SH_ERR" ]) || ([ -z "$BASH_ERR" ] && [ -z "$SH_ERR" ])) && [ "$bash_ret" -eq "$sh_ret" ]; then
        echo -e "$GREEN[OK]${RESET} $description"
        ((TESTS_SUCCESS++))
    else
        echo -e "${RED}[FAIL]${RESET} $description"
        echo -e "${YELLOW}Command:${RESET} $command"
        echo -e "${BLUE}--- Expected stdout ---${RESET}"
        cat "$BASH_OUT"
        echo -e "${BLUE}--- Actual stdout ---${RESET}"
        cat "$SH_OUT"
        echo -e "${BLUE}--- Expected stderr ---${RESET}"
        cat "$BASH_ERR"
        echo -e "${BLUE}--- Actual stderr ---${RESET}"
        cat "$SH_ERR"
        echo -e "${BLUE}Expected return code:${RESET} $bash_ret"
        echo -e "${BLUE}Actual return code:${RESET} $sh_ret"
        echo
    fi
}

run_test2() {
    local description="$1"
    local command="$2"

    ((TESTS_RUN++))

    # Exécution dans /bin/bash
    echo -e "$command" | bash --posix >"$BASH_OUT" 2>"$BASH_ERR"
    local bash_ret=$?
    cat -e files.txt > $BASH_RED

    # Exécution dans 42sh
    echo -e "$command" | $TEST_SHELL >"$SH_OUT" 2>"$SH_ERR"
    local sh_ret=$?
    cat -e files.txt > $SH_RED

    # Comparaison des résultats
    if diff -q "$BASH_OUT" "$SH_OUT" >/dev/null && (([ -n "$BASH_ERR" ] && [ -n "$SH_ERR" ]) || ([ -z "$BASH_ERR" ] && [ -z "$SH_ERR" ])) && [ "$bash_ret" -eq "$sh_ret" ] && diff -q "$BASH_RED" "$SH_RED";  then
        echo -e "$GREEN[OK]${RESET} $description"
        ((TESTS_SUCCESS++))
    else
        echo -e "${RED}[FAIL]${RESET} $description"
        echo -e "${YELLOW}Command:${RESET} $command"
        echo -e "${BLUE}--- Expected stdout ---${RESET}"
        cat "$BASH_OUT"
        echo -e "${BLUE}--- Actual stdout ---${RESET}"
        cat "$SH_OUT"
        echo -e "${BLUE}--- Expected stderr ---${RESET}"
        cat "$BASH_ERR"
        echo -e "${BLUE}--- Actual stderr ---${RESET}"
        cat "$SH_ERR"
        echo -e "${BLUE}Expected return code:${RESET} $bash_ret"
        echo -e "${BLUE}Actual return code:${RESET} $sh_ret"
        echo
    fi
}



# Début des tests
print_header "TESTS POUR 42SH - STEP 1"

## 1. Commandes simples
run_test "Commande simple: echo" "echo 'Hello, World!'"
run_test "Commande conditionnelle: if-then-else" "if true; then echo OK; else echo Fail; fi"
run_test "Commande conditionnelle: if-then-elif-else" "if false; then echo Fail; elif true; then echo OK; else echo Fail; fi"
run_test "Liste de commandes avec ;" "echo foo; echo bar"
run_test "Liste de commandes avec ; final" "echo foo; echo bar;"
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
# Tests simples avec echo, true, et false
run_test "Echo simple avec une chaîne courte" "echo Hello"
run_test "Echo avec chaîne longue" "echo This is a very long string to test the handling of long outputs by the shell"
run_test "Builtin true avec commande enchaînée" "true; echo Success"
run_test "Builtin false avec commande enchaînée" "false; echo Should not appear"
run_test "Echo avec caractères spéciaux" "echo 'Special characters: \$ \` # ! ~ \%'"

# Tests avec structures conditionnelles
run_test "If simple avec true" "if true; then echo True condition; fi"
run_test "If simple avec false" "if false; then echo Should not appear; fi"
run_test "If-then-else avec true" "if true; then echo True branch; else echo False branch; fi"
run_test "If-then-else avec false" "if false; then echo True branch; else echo False branch; fi"
run_test "If-elif-else complexe" "if false; then echo First branch; elif true; then echo Second branch; else echo Final branch; fi"
run_test "If-elif-else toutes fausses" "if false; then echo First branch; elif false; then echo Second branch; else echo Final branch; fi"

# Tests de commandes enchaînées
run_test "Commandes séparées par ;" "echo First; echo Second; echo Third"
run_test "Commandes enchaînées mélangeant true/false" "true; echo Passed; false; echo Failed"
run_test "Commandes enchaînées avec if" "if true; then echo In condition; fi; echo Outside condition"

# Tests avec des commandes variées
run_test "Commande pwd" "pwd"
run_test "Commande date" "date"
run_test "Commande combinée avec echo" "echo 'Current directory:'; pwd"

# Tests de taille croissante
run_test "Commande avec beaucoup d'échos (court)" "echo line1; echo line2; echo line3; echo line4; echo line5"
run_test "If-then-else complexe avec beaucoup de lignes" "if true; then echo Line1; echo Line2; echo Line3; else echo Failure; fi"
run_test "test Comment" "echo zizi 
echo zaza
echo testo #comment
echo after comment"


# Tests simples supplémentaires
run_test "Commande simple: ls" "ls"
run_test "Commande simple avec option: ls -l" "ls -l"
run_test "Commande simple avec option: ls -a" "ls -a"
run_test "Commande simple avec option: ls -la" "ls -la"

# Tests avec des quotes
run_test "Quotes doubles simples" "echo \"Double quotes test\""
run_test "Quotes doubles avec spécial" "echo \"Special \$ # characters inside quotes\""
run_test "Quotes imbriquées" "echo 'This \"is\" nested quotes'"
run_test "Quotes multiples" "echo 'One'; echo \"Two\"; echo 'Three'"

# Tests conditionnels
run_test "Condition avec plusieurs elif" "if false; then echo One; elif false; then echo Two; elif true; then echo Three; else echo Four; fi"

# Tests pipelines
run_test "simple pipeline" "echo coucou | tr o e"
run_test "double pipeline" "echo coucou | tr o e | sort"
run_test "double pipeline V2" "ls | tr o e | sort"
run_test "quadruple pipeline" "ls | tr o e | sort | cat -e"
run_test "error case : missing first commande" "| tr o e"
run_test "error case : missing seconde commande" "echo coucou | "
run_test "error case : missing third commande" "echo coucou | tr o e |"
run_test "error case : missing fourth commande" "echo coucou | tr o e | sort |"
run_test "few commande between pipelines" "echo test echo coucou | tr o e; sort; ls | cat -e"


# Tests And / Or
run_test "simple and" "echo 1 && echo 2"
run_test "simple and with builtin true" "true && echo 2"
run_test "simple and with builtinf false" "false && echo 2"
run_test "simple and with builtin true V2" "echo 2 && true"
run_test "simple and with builtinf false V2" "echo 2 && false"
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


# Tests negation
run_test "simple builtin true" "! true"
run_test "simple builtin false" "! false"
run_test "simple commande" "! echo test"
run_test "simple commande fail" "! slqf"
run_test "if with negation" "! if true; then echo ok; fi"
run_test "if with negation V2" "! if false; then echo ok; fi"
run_test "elif with negation" "! if false; then echo ok; elif true; then echo ok2; else echo marcheaps; fi"

# Tests IO/rediréction
run_test2 "simple redir" "echo test > files.txt"



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

