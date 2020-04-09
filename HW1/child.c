#include "ver1.h"

int main(int argc, char *argv[]) {
	char board[64], job[1], isLast[1];
	int verifyRead;

	while ((verifyRead = read(STDIN_FILENO, board, 64)) == 64) { //stuck on here because the father can't send the board so child wait to read

		if ((verifyRead = read(STDIN_FILENO, job, 1)) == -1) {
			write(STDERR_FILENO, "failed read job\n ", 16);
			exit(1);
		}

		if ((verifyRead = read(STDIN_FILENO, isLast, 1)) == -1) {
			write(STDERR_FILENO, "failed read if last\n ", 21);
			exit(1);
		}

		sprintf(isLast, "%d", atoi(isLast));

		switch (job[0]) {
		case '1':
			if (!checkL(board, STDOUT_FILENO))
				write(STDOUT_FILENO, "0", 1);
			break;
		case '2':
			if (!checkC(board, STDOUT_FILENO))
				write(STDOUT_FILENO, "0", 1);
			break;
		case '3':
			if (!cLRslant(board, STDOUT_FILENO))
				write(STDOUT_FILENO, "0", 1);
			break;
		case '4':
			if (!cRLslant(board, STDOUT_FILENO))
				write(STDOUT_FILENO, "0", 1);
			break;

		}
		if (isLast[0] == '1')
			exit(0);
	}
	if (verifyRead == -1) {
		write(STDERR_FILENO, "failed read board\n ", 18);
		exit(1);
	}

	return 0;
}

int checkL(char * str, int pfd) {

	int count;
	for (int i = 0; i < 8; ++i) {
		count = 0;
		for (int j = 0; j < 8; ++j) {
			if (str[i * 8 + j] == 'Q') {
				count++;
				if (count > 1) {
					write(pfd, NUMBER_ONE, 1);
					return 1;
				}
			}

		}
		if (count == 0) {
			write(pfd, NUMBER_ONE, 1);
			return 1;
		}
	}
	return 0;
}

int checkC(char * str, int pfd) {

	int count;
	for (int i = 0; i < 8; ++i) {
		count = 0;
		for (int j = 0; j < 8; ++j) {
			if (str[j * 8 + i] == 'Q') {
				count++;
				if (count > 1) {
					write(pfd, NUMBER_ONE, 1);
					return 1;
				}

			}

		}
		if (count == 0) {
			write(pfd, NUMBER_ONE, 1);
			return 1;
		}
	}
	return 0;
}

int cLRslant(char * str, int pfd) {

	int count;

	for (int i = 0; i < 8; ++i) {

		count = 0;
		for (int j = 0; j < 8 && j * 8 + j + i < 64 - i * 7; ++j) {
			if (str[j * 8 + j + i] == 'Q') {
				count++;
				if (count > 1) {
					write(pfd, NUMBER_ONE, 1);
					return 1;
				}

			}

		}

	}

	for (int i = 1; i < 8; ++i) {
		count = 0;
		for (int j = 0; j < 8 && i * 8 + j * 9 < 64 - i; ++j) {

			if (str[i * 8 + j * 9] == 'Q') {
				count++;
				if (count > 1) {
					write(pfd, NUMBER_ONE, 1);
					return 1;
				}
			}

		}

	}
	return 0;
}

int cRLslant(char * str, int pfd) {

	int count;
	for (int i = 0; i < 7; ++i) {
		count = 0;
		for (int j = 0; j < 8 && i + 7 * j <= i * 8; ++j) {
			if (str[i + 7 * j] == 'Q') {
				count++;
				if (count > 1) {
					write(pfd, NUMBER_ONE, 1);
					return 1;
				}

			}

		}

	}

	for (int i = 0; i < 8; ++i) {
		count = 0;
		for (int j = 0; j < 8 && (j + 1 + i) * 7 + i <= 8 * 7 + i; ++j) {
			if (str[(j + 1 + i) * 7 + i] == 'Q') {
				count++;
				if (count > 1) {
					write(pfd, NUMBER_ONE, 1);
					return 1;
				}

			}

		}

	}
	return 0;
}
