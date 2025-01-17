#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>

//variable global
char filename[] = "grid";
char grid[256][256]; //trouver un truc plus opti
int grid_len_x = 10;
int grid_len_y = 10;

// display the grid in the terminal with 
// the first and last letter of the word in yellow
int print_grid(int* Start, int* End) {
	for (int i = 0; i < grid_len_x; i++) {
		for (int j = 0; j < grid_len_y; j++) {
			if ((i == Start[1] && j == Start[0]) || (i == End[1] && j == End[0])) {
				printf("\033[0;33m%c", grid[i][j]);
				printf("\033[0m"); 
			}
			else {
				printf("%c", grid[i][j]);
			}
		}
		//printf("\n");
	}
	return 0;
}
//len function (need to be move in lib file)
int len(char word[]) {
    int l = 0;
    while (word[l] != 0) {
        l++;
    }
    return l;
}
// find the rest of the word after finding the first two letters 
// and the direction of the word
int find_word(char word[], int* End, int i, int j, int vx, int vy) {
	//printf("i,j = %i,%i   vx,vy = %i,%i\n",i,j,vx,vy);
	i += vx;
	j += vy;
	for (int k = 2; k < len(word); k++) {
		if ((i >= 0 && i < grid_len_x) && (j >= 0 && j < grid_len_y) && grid[i][j] == word[k]) {
			i += vx;
			j += vy;
		}
		else {
			return 1;
		}
	}
	End[0] = j - vy;
	End[1] = i - vx;
	return 0;
}
// find the second letter and the direction of the word after 
// finding the first letter of the word
int find_second_letter(char word[], int* End, int x, int y) {
	for (int i = x-1; i <= x + 1; i++) {
		for (int j = y - 1; j <= y + 1; j++) {
			if (i >= 0 && i < grid_len_x && j >= 0 && j < grid_len_y) {
				//printf("l(%i,%i):%c\n",i,j,grid[i][j]);
				if (grid[i][j] == word[1]) {
					int nx = i + (i - x) * (len(word) - 2);
					int ny = j + (j - y) * (len(word) - 2);
					if (nx >= 0 && nx < grid_len_x && ny >= 0 && ny < grid_len_y){
						if (find_word(word, End, i, j, i - x, j - y) == 0) {
							return 0;
						}
					}
				}
			}
		}
	}

	return 1;
}

// solve the grid by finding the first letter of the word and 
// calling the other functions to find the rest of the word
int solve_grid(char word[], int* Start,int* End) 
{
	for (size_t x = 0; x < (size_t)grid_len_x; x++) 
    {
		for(size_t y = 0; y < (size_t)grid_len_y; y++) 
        {
			if(grid[x][y] == word[0]) 
            {
				Start[0] = y;
				Start[1] = x;
				if(find_second_letter(word, End, x, y) == 0) 
                {
					return 0;
                }
			}
		}
	}
	return 1;
}

// read the grid in the file and transform it into a matrix
int read_grid(char filename[]) {
	FILE *fichier = fopen(filename, "r");

	if(fichier == NULL) {
		char* arg[3] = { "../Bash/rmAllBMP.sh", "3", NULL };
        execvp("../Bash/rmALLBMP.sh", arg);
        errx(EXIT_FAILURE, "File not found in solveur: %s", filename);
        return EXIT_FAILURE;
    }
	char ligne[256];
	size_t x = 0;

	while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
		size_t y = 0;
		while (ligne[y] != 0) {
			grid[x][y] = ligne[y];
			y++;
		}
		grid_len_y = y;
		x++;
	}
	grid_len_x = x;

	return 0;
}

void toUpperCase(char str[]) 
{
    for (int i = 0; str[i] != '\0'; i++) 
    {
        // Check if the character is a lowercase letter (between 'a' and 'z')
        if (str[i] >= 'a' && str[i] <= 'z') 
        {
            // Convert to uppercase by subtracting the difference between 'a' and 'A'
            str[i] = str[i] - ('a' - 'A');
        }
    }
}

// main function, read the grid file and resolve the grid with the input word
int solver(char word[], int** Start, int** End)
{
    toUpperCase(word);	
	read_grid(filename);

	*Start = calloc(2,sizeof(int));
	*End = calloc(2,sizeof(int));

	int e = solve_grid(word,*Start,*End);
	if(e==1)
	{
		printf("ERROR: le mot n'existe pas\n");
		return -1;
	}
	else
	{
		printf("\nWord: %s\n", word);
		print_grid(*Start,*End);
		//printf("Word: %s\n", word);
		//printf("Start = (%i, %i)\n", Start[0], Start[1]);
		//printf("End = (%i, %i)\n", End[0], End[1]);
		printf("(%i, %i)(%i, %i)\n", (*Start)[0], (*Start)[1], (*End)[0], (*End)[1]);
	}
	return 0;
}

/*
//main function only for debug (remove it!)
int main(int argc, char *argv[])
{
	if(argc<2)
		printf("ERROR: argument missing\n");
	else
		return solver(argv[1]);
}

*/
