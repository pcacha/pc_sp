#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void success() { 
	printf("Regular expression was accepted!\n");
	exit(EXIT_SUCCESS);
}

void failure() { 
	printf("Regular expression was NOT accepted!\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	int i, string_len, state;
	char *regex;

	if(argc < 2) { 
		printf("Missing argument!");
		exit(EXIT_FAILURE);
	}

	regex = argv[1];
	string_len = strlen(regex);
	state = 0;

	for(i = 0; i <= string_len; i++) {
		if(state == 0) {
			if(i == string_len) {
				failure();
			}

			if(regex[i] == 'A') {
				state = 1;
				continue;
			}

			failure();
		}

		else if(state == 1) {
			if(i == string_len) {
				failure();
			}

			if(regex[i] == 'n') {
				state = 2;
				continue;
			}

			failure();
		}

		else if(state == 2) {
			if(i == string_len) {
				failure();
			}

			if(regex[i] == 'o') {
				state = 3;
				continue;
			}
			if(regex[i] == 'n') {
				state = 2;
				continue;
			}

			failure();
		}

		else if(state == 3) {
			if(i == string_len) {
				failure();
			}

			if(regex[i] == '1') {
				state = 4;
				continue;
			}

			failure();
		}

		else if(state == 4) {
			if(i == string_len) {
				failure();
			}

			if(regex[i] == '9') {
				state = 5;
				continue;
			}

			failure();
		}

		else if(state == 5) {
			if(i == string_len) {
				failure();
			}

			if(regex[i] == '0') {
				state = 6;
				continue;
			}
			if(regex[i] == '9') {
				state = 5;
				continue;
			}

			failure();
		}

		else if(state == 6) {
			if(i == string_len) {
				success();
			}


			failure();
		}

	}

	failure();
	return EXIT_FAILURE;
}