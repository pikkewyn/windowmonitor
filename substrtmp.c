#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *lcommon( char* string1, char* string2 ) 
{
	int strlen1 = strlen( string1 );
	int strlen2 = strlen( string2 );

	int len = strlen1 < strlen2 ? strlen1 : strlen2;
	int longest = 0;
	
  int **ptr = malloc( 2 * sizeof( int* ) );
	
  int* ret = calloc( strlen1 + 1, sizeof( int ) );

  for( int i = 0; i < 2; i++ )
		ptr[ i ] = calloc( strlen2, sizeof( int ) );

	int k = 0;
	
  for( int i = 0; i < strlen1; i++ ) 
  {
		memcpy( ptr[0], ptr[1], strlen2 * sizeof( int ) );
		for (int j = 0; j < strlen2; j++) 
    {
			if (string1[i] == string2[j]) 
      {
				if (i == 0 || j == 0) 
        {
					ptr[1][j] = 1;
				} else i
        {
					ptr[1][j] = ptr[0][j-1] + 1;
				}
				if (ptr[1][j] > longest) 
        {
					longest = ptr[1][j];
					k = 0;
					ret[k++] = longest;
				}
				if (ptr[1][j] == longest) 
        {
					ret[k++] = i;
					ret[k] = -1;
				}
			} else 
      {
				ptr[1][j] = 0;
			}
		}
	}

	for (int i = 0; i < 2; i++)
		free(ptr[i]);
	free(ptr);

	ret[0] = longest;
	
  return ret;
}

int main(int argc, char *argv[]) {
	int i, longest, *ret;

	if (argc != 3) {
		printf("usage: longest-common-substring string1 string2\n");
		exit(1);
	}

	ret = lcommon(argv[1], argv[2]);
	if ((longest = ret[0]) == 0) {
		printf("There is no common substring\n");
		exit(2);
	}

	i = 0;
	while (ret[++i] != -1) {
		printf("%.*s\n", longest, &argv[1][ret[i]-longest+1]);
	}

	exit(0);
}
