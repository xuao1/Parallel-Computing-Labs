#!/bin/bash

GREEN="\033[0;32m"
YELLOW="\033[0;33m"
RESET="\033[0m"

echo -e "${GREEN}Running Pi_0${RESET}"
./Pi_0 > Pi_0.out
echo -e "${YELLOW}Pi_0 output:${RESET}"
cat Pi_0.out

echo -e "${GREEN}Running Pi_1${RESET}"
./Pi_1 > Pi_1.out
echo -e "${YELLOW}Pi_1 output:${RESET}"
cat Pi_1.out

echo -e "${GREEN}Running Pi_2${RESET}"
./Pi_2 > Pi_2.out
echo -e "${YELLOW}Pi_2 output:${RESET}"
cat Pi_2.out

echo -e "${GREEN}Running Pi_3${RESET}"
./Pi_3 > Pi_3.out
echo -e "${YELLOW}Pi_3 output:${RESET}"
cat Pi_3.out

echo -e "${GREEN}Running Pi_4${RESET}"
./Pi_4 > Pi_4.out
echo -e "${YELLOW}Pi_4 output:${RESET}"
cat Pi_4.out

rm Pi_0.out Pi_1.out Pi_2.out Pi_3.out Pi_4.out
