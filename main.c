#include <stdio.h>
#include <stdlib.h>

// Brainfuck instructions
//  > Math stuff
#define INCR '+' // +1 on current MS
#define DECR '-' // -1 on current MS
//  > Memory slot stuff
#define FORW '>' // Go to next MS
#define BACK '<' // Go to previous MS
//  > Logic stuff
#define SJMP '[' // Loop till current MS value is equal to zero
#define EJMP ']' // Jump to the beginning of the loop
//  > I/O stuff
#define PRTC '.' // Print character with MS value as ASCII code
#define GETC ',' // Get an user input ASCII character code

typedef unsigned char byte;

// The brainfuck program struct
typedef struct {
      byte *values;
      int size;
      int index;

      byte *loops_starts;
      int loop_size;
      int loop_index;

} BrainfuckProgram;

void initialize(BrainfuckProgram *bfp) {
      bfp->values = malloc(bfp->size * sizeof(*bfp->values));
      bfp->loops_starts = malloc(bfp->size * sizeof(*bfp->loops_starts));
}

int allocate_values(BrainfuckProgram *bfp) {
      byte *newMem = realloc(bfp->values, bfp->size * sizeof(byte));

      if (!newMem) {
            return 1;
      }
      bfp->values = newMem;

      return 0;
}

int allocate_new_loop(BrainfuckProgram *bfp) {
      byte *newMem = realloc(bfp->loops_starts, bfp->loop_size * sizeof(byte));

      if (!newMem) {
            return 1;
      }
      bfp->loops_starts = newMem;

      return 0;
}

void set_value(BrainfuckProgram *bfp, byte value) {
      *(bfp->values + bfp->index) = value % 256;
}

int get_value(BrainfuckProgram *bfp) {
      return *(bfp->values + bfp->index);
}

// FILE functions
int get_file_name(int argc, char *argv[], char** fname) {
     if (argc == 1) {
           printf("File name is missing\n");
           return 1;
     }

     *fname = argv[1];
     return 0;
}

int get_file_size(FILE *file) {
	fseek(file, 0L, SEEK_END);
	int size = ftell(file);
	rewind(file);
	return size;
}

// Main function
int main(int argc, char *argv[]) {
      // Vector part
      BrainfuckProgram bfp = {
            .values = NULL,
            .size = 1,
            .index = 0,

            .loops_starts = NULL,
            .loop_index = 0,
            .loop_size = 0

      };


      initialize(&bfp);

      // FILE Part
	char* fname = NULL;
      int error = get_file_name(argc, argv, &fname);
      if (error) {
            goto exit;
      }

	// Create file variable
	FILE *fin = NULL;
	fin = fopen(fname, "r");

	int size = get_file_size(fin);

	// Create a char array of the right size
	byte *prog = NULL;
	prog = malloc(size * sizeof(*prog));
      // Read 1 byte size times
	fread(prog, 1, size, fin);

	for (int i = 0; i < size; i++) {
            byte current =  *(prog+i);
		switch (current) {
			case INCR:
                        set_value(&bfp, get_value(&bfp) + 1);
				break;

			case DECR:
                        set_value(&bfp, get_value(&bfp) - 1);
				break;

			case FORW:
                        // if index+1 == size then we are at the
                        // last element of the vector cause
                        // size = index - 1
                        if (bfp.index+1 == bfp.size) {
                              bfp.size++;

                              int error = allocate_values(&bfp);
                              if (error) {
                                    printf("an error occured while forwarding pointer");
                                    goto close;
                              }
                        }
                        bfp.index++;

				break;

			case BACK:
                        // bfp.index == 0 then !bfp.index == true
                        if (!bfp.index) {
                             printf("can go back from index 0\n");
                             goto close;
                        }
                        bfp.index--;

				break;

			case SJMP:
                        if (bfp.loop_index == bfp.size) {
                              bfp.loop_size++;

                              int error = allocate_new_loop(&bfp);
                              if (error) {
                                    printf("an error occured while forwarding pointer");
                                    goto close;
                              }

                        }
                        bfp.loop_index++;
                        *(bfp.loops_starts + bfp.loop_index) = i;
				break;

			case EJMP:
                        if (bfp.loop_index == -1) {
                              printf("cannot go back");
                              goto close;
                        }

                        if (get_value(&bfp) != 0) {
                              i = *(bfp.loops_starts + bfp.loop_index);
                        
                        } else {
                              bfp.loop_index--;

                        }
				break;

			case PRTC:
                        printf("%c", get_value(&bfp));
				break;

			case GETC:
                        ; // Semicolon here because of the label error
                        byte buf;
                        scanf("%c", &buf);
                        set_value(&bfp, buf);
				break;
                  
		}
	}

      close:
            fclose(fin);

      exit:
            ;
            
	return 0;
}

