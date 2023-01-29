# sudo apt install ncurses-dev
set -e
gcc main.c -lncursesw -Werror -Wpedantic -Wall -lm
./a.out
