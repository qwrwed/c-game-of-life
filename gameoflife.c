#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gol.h"

#define CHAR_MAX 32
#define CHAR_MAX_FORMAT "%32s"

// optionally compile with debug statements
#define ENABLE_PRINT_DEBUG 0
#if ENABLE_PRINT_DEBUG == 1
	#define DEBUG_EXECUTE(s) {s}
#else
	#define DEBUG_EXECUTE(s)
#endif

int main(const int argc, const char * argv[]) {
	
	char input_filename[CHAR_MAX+1] = "";
	char output_filename[CHAR_MAX+1] = "";
	int number_of_generations = -1;
	int enable_print_statistics = 0;
	int enable_torus_topology = 0;
	char *flags = malloc(sizeof(char)*argc);

	// parse argument flags and arguments
	for (int i=1; i<argc; i++){
		if ((argv[i][0] != '-') || (strlen(argv[i]) != 2)){ // ensure it is the right format for a switch
			fprintf(stderr, "ERROR: Invalid switch '%s'\n", argv[i]);
			exit(1);
		} else {
			flags[strlen(flags)] = argv[i][1];
			
			switch(argv[i][1]) {
				case 'i':
					if (i == (argc-1)){ // check to make sure '-i' is not the last thing (must have a value)
						fprintf(stderr, "ERROR: Missing argument after switch '%s'\n", argv[i]);
						exit(1);
					} else {
						if ((strcmp(input_filename, argv[i+1]) == 0) || (strcmp(input_filename, "") == 0)) {
							strcpy(input_filename, argv[i+1]);
							i++;							
						} else {
							fprintf(stderr, "ERROR: Repeated switch '%s': value '%s' conflicts with previous value '%s'\n", argv[i], argv[i+1], input_filename);
							exit(0);
						}
						
					}
					break;
				case 'o':
					if (i == (argc-1)){ // check to make sure '-o' is not the last thing (must have a value)
						fprintf(stderr, "ERROR: Missing argument after switch '%s'\n", argv[i]);
						exit(1);
					} else {
						if ((strcmp(output_filename, argv[i+1]) == 0) || (strcmp(output_filename, "") == 0)) {
							strcpy(output_filename, argv[i+1]);
							i++;
						} else {
							fprintf(stderr, "ERROR: Repeated switch '%s': value '%s' conflicts with previous value '%s'\n", argv[i], argv[i+1], input_filename);
							exit(0);
						}
					}
					break;
				case 'g':
					if (i == (argc-1)){ // check to make sure '-g' is not the last thing (must have a value)
						fprintf(stderr, "ERROR: Missing argument after switch '%s'\n", argv[i]);
						exit(1);
					} else {
						if ((number_of_generations == -1) || (number_of_generations == atoi(argv[i+1]))) {
							number_of_generations = atoi(argv[i+1]);
							if (number_of_generations < 0){
								fprintf(stderr, "ERROR: Invalid number of generations '%s'\n", argv[i+1]);
								exit(1);
							}
							i++;
						} else {
							fprintf(stderr, "ERROR: Repeated switch '%s': value '%s' conflicts with previous value '%d'\n", argv[i], argv[i+1], number_of_generations);
							exit(0);
						}
						
					}
					break;
				case 's':
					enable_print_statistics = 1;
					break;
				case 't':
					enable_torus_topology = 1;
					break;
				default:
					fprintf(stderr, "ERROR: Unrecognised switch '%s'\n", argv[i]);
					exit(1);
			}
		}
	};

	if (number_of_generations < 0) {
		number_of_generations = 5;
	}

	DEBUG_EXECUTE(
		printf("\n");
		printf("Input filename: %s\n", input_filename);
		printf("Output filename: %s\n", output_filename);
		printf("Number of generations: %d\n", number_of_generations);
		printf("Print statistics: %d\n", enable_print_statistics);
		printf("Use torus topology: %d\n", enable_torus_topology);
	)

	struct universe v;
	int noFilename;
	
	//read file
	FILE *inputFile;
	noFilename = (strcmp(input_filename, "") == 0);
	if (noFilename) {
		inputFile = stdin;
	} else {
		inputFile = fopen(input_filename, "r");
	}
  	read_in_file(inputFile, &v);
	if (inputFile != stdin) {
		fclose(inputFile);
	}

	//optionally use torus topology for the rule
	int (*rule)(struct universe *u, int column, int row);
	if (enable_torus_topology) {
		rule = will_be_alive_torus;
	} else {
		rule = will_be_alive;
	}
	
	// evolve by specified number of generations
	for (int i = 0; i < number_of_generations; i++) {
		evolve(&v, rule);
		DEBUG_EXECUTE(
			printf("!");
			write_out_file(stdout, &v);
			for(int j = 0; j < 10000000; j++){}
		)
	}
	

	// output final generation to specified file
	FILE *outputFile;
	noFilename = (strcmp(output_filename, "") == 0);
	if (noFilename) { // write to stdout, but don't close
		outputFile = stdout;
		DEBUG_EXECUTE(
			//system("clear");
		)
	} else { // write to file and close
		outputFile = fopen(output_filename, "w");
	}
	write_out_file(outputFile, &v);
	if (outputFile != stdout) {
		fclose(outputFile);
	}
  	
	// print statistics after the final generation has been output
	if (enable_print_statistics) {
		print_statistics(&v);
	}

	return 0;
}
