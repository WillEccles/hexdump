#include <stdlib.h>
#include <stdio.h>

int exists(char *fname) {
	FILE *file = fopen(fname, "r");
	
	if (file) {
		fclose(file);
		return 1;
	}

	return 0;
}

int bytecount(FILE *file) {
	int start = ftell(file);
	fseek(file, 0, SEEK_END);
	int count = ftell(file);
	fseek(file, start, SEEK_SET);
	return count;
}

int main(int argc, char **argv) {
	char *srcfile = NULL;
	char *destfile = NULL;
	long maxbuffer = 10 * 1048576; // 10MB max buffer

	if (argc == 1) {
		printf("Usage: %s <source> [dest] [max buffer]\n", argv[0]);
		return 1;
	}

	// collect arguments
	if (argc >= 2) {
		srcfile = argv[1];
	}
	if (argc >= 3) {
		destfile = argv[2];
	}
	if (argc >= 4) {
		maxbuffer = labs(atol(argv[3]));
		if (maxbuffer < 3) maxbuffer = 0;
	}

	if (!exists(srcfile)) {
		fprintf(stderr, "Error: file not found: %s\n", srcfile);
		return 1;
	}

	if (destfile && exists(destfile)) {
		fprintf(stderr, "Error: destination file already exists: %s\n", destfile);
		return 1;
	}

	FILE *infile = fopen(srcfile, "rb");
	if (!infile) {
		fprintf(stderr, "Error: could not open file for reading: %s\n", srcfile);
		return 1;
	}

	if (bytecount(infile) == 0) {
		fprintf(stderr, "Error: input file is empty.\n");
		fclose(infile);
		return 1;
	}
	
	FILE *outfile = NULL;
	if (destfile) {
		outfile = fopen(destfile, "wb");
		if (!outfile) {
			fprintf(stderr, "Error: coult not open file for writing: %s\n", destfile);
			fclose(infile);
			return 1;
		}
	} else {
		outfile = stdout;
	}

	char *buffer = NULL;
	long buffsize = 0;
	if (maxbuffer) {
		// contains a space after each byte, and each byte should be exactly 2 characters
		// this means that per byte, we will need 3 characters in our buffer
		// get a buffer size that's as big as possible within limits that's divisible by 3
		for (long i = maxbuffer; i > 0; i--) {
			if (i % 3 == 0) {
				buffsize = i;
				break;
			}
		}

		buffer = calloc(buffsize, sizeof(char));
	}

	long count = 0;
	char curr = fgetc(infile);
	while (curr != EOF) {
		if (buffsize)
			sprintf(buffer + (count * 3), "%02hhX", curr);
		else
			fprintf(outfile, "%02hhX ", curr);

		if (buffsize) {
		   if (count == buffsize - 1) {
				count = 0;
				// filter out null characters, if any
				for (int i = 2; i < buffsize; i += 3) {
					if (buffer[i] == 0)
						buffer[i] = ' ';
				}
				fwrite(buffer, sizeof(char), buffsize, outfile);
			}
			else {
				count++;
			}
		}

		curr = fgetc(infile);
	}

	// flush the rest of the buffer to the file if there is some left over
	if (buffsize && count != 0) {
		for (int i = 2; i < buffsize; i += 3) {
			if (buffer[i] == 0)
				buffer[i] = ' ';
		}
		fwrite(buffer, sizeof(char), count * 3, outfile);
	}

	if (buffer)
		free(buffer);

	if (ferror(infile)) {
		fprintf(stderr, "Error reading from file: %s\n", srcfile);
		fclose(infile);
		if (destfile)
			fclose(outfile);
		return 1;
	}

	if (destfile && ferror(outfile)) {
		fprintf(stderr, "Error writing to file: %s\n", destfile);
		fclose(infile);
		fclose(outfile);
		return 1;
	}

	fclose(infile);
	if (destfile)
		fclose(outfile);

	return 0;
}
