#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#define MAX_KEY_SIZE 13


	/*float probabilities[] = {
		0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228, 0.02015,
		0.06094, 0.06966, 0.00153, 0.00772, 0.04025, 0.02406, 0.06749,
		0.07507, 0.01929, 0.00095, 0.05987, 0.06327, 0.09056, 0.02758,
		0.00978, 0.02361, 0.00150, 0.01974, 0.00074
	};*/



typedef struct{
	int val;
	int times;
}number_t;

int arr_size;


int
hex_to_int(char c){
		 if(c == 'f' || c == 'F') return 15;
	else if(c == 'e' || c == 'E') return 14;
	else if(c == 'd' || c == 'D') return 13;
	else if(c == 'c' || c == 'C') return 12;
	else if(c == 'b' || c == 'B') return 11;
	else if(c == 'a' || c == 'A') return 10;
	else return c - '0';
}


int
hex_to_ascii(char c, char d){
	int high = hex_to_int(c) * 16;
	int low = hex_to_int(d);
	return high+low;
}



void
print_array(int *arr, int size){
	int i;
	fprintf(stderr, "==========printing n = %d ===============\n", arr_size/size);
	for(i = 0; i < size; i++)
		fprintf(stderr, "%d: %d ", i, arr[i]);
	fprintf(stderr, "\n========================================\n");
}

void
print_array_ascii(int *arr, int size){
	int i;
	for(i = 0; i < size; i++){
		fprintf(stderr, "%c", arr[i]);
	}
	fprintf(stderr, "\n");
}


float
get_sum(int occurences[], int size){
	int i;
	float total = 0.0;
	for(i = 0; i < 256; i++){
		if(occurences[i] != 0) {
		float x = occurences[i] / (float) size;
		total += x*x;
		}
	}
	return total;
}

int *
get_frequencies(int int_arr[], int size){
	int i;
	int *occurences = malloc(256 * sizeof(int));
	for(i = 0; i < size; i++){
		occurences[int_arr[i]]++;
	}
	return occurences;
}
int *
get_n_values(int n, int shift, int numbers[]){
	int i;
	int size = arr_size/n;
	int *cyphertext_n = malloc(size * sizeof(int));

	for(i = 0; i < size; i++)
		cyphertext_n[i] = -1;

	for(i = 0; i < size; i++)
		cyphertext_n[i] = numbers[i*n + shift];
		
	return cyphertext_n;
}

int
get_most_likely_key_size(float sum_of_squares[]){
	int i, pos_max;
	float max = 0.0;
	for(i = 0; i < MAX_KEY_SIZE; i++)
		if(sum_of_squares[i] > max){
			max = sum_of_squares[i];
			pos_max = i;
		}
	return pos_max + 1;

}

int
is_acceptable_value(int val){
	return (val >= 32 && val <= 33) || (val >= 44 && val <= 57)
		|| (val >= 63 && val <= 90) || (val >= 97 && val <= 122);
}

int
get_shift(int *arr, int size){
	int i, j, ok, val;
	for(i = 0; i < 255; i++){
		ok = 1;
		for(j = 0; j < size; j++){
			val = i ^ arr[j];
			if(!is_acceptable_value(val)){
				ok = 0;
				break;
			}
		}
		if(ok)
			return i;
	}
	return 128;
}

void
free_all(int *cyphertext, int *frequency){
	free(cyphertext);
	free(frequency);
}

int main(int argc, char *argv[]){

	int i;
	FILE *file_cyphertext = fopen("ctext.txt", "r");
	if(file_cyphertext == NULL)
		err(1, "error fopen");

	fseek(file_cyphertext, 0, SEEK_END); // seek to end of file
	long size = ftell(file_cyphertext); // get current file pointer
	fseek(file_cyphertext, 0, SEEK_SET); // seek back to beginning of file
	char cyphertext_hex[size];
	fread(cyphertext_hex, sizeof(char), size/sizeof(char), file_cyphertext);
	fclose(file_cyphertext);


	arr_size = size/2;
	int cyphertext_ascii[arr_size];
	for(i = 0; i < size; i+=2){
		cyphertext_ascii[i/2] = hex_to_ascii(cyphertext_hex[i], cyphertext_hex[i+1]);
		
	}



	int **cyphertexts = malloc(sizeof(int *) * MAX_KEY_SIZE);
	int **frequencies = malloc(sizeof(int *) * MAX_KEY_SIZE);
	float sum_of_squares[MAX_KEY_SIZE];
	for(i = 0; i < MAX_KEY_SIZE; i++){
		*(cyphertexts + i) = get_n_values(i+1, 0, cyphertext_ascii);
	//	print_array(*(cyphertexts + i), arr_size/(i+1));
		frequencies[i] = get_frequencies(*(cyphertexts + i), arr_size/(i+1));
		sum_of_squares[i] = get_sum(frequencies[i], arr_size/(i+1));
	}

	int key_size = get_most_likely_key_size(sum_of_squares);

	for(i = 0; i < MAX_KEY_SIZE; i++){
		free_all(cyphertexts[i], frequencies[i]);
	}
	free(cyphertexts);
	free(frequencies);

	cyphertexts = malloc(sizeof(int *) * key_size);
	int key[key_size];
	printf("%d\n", key_size);
	for(i = 0; i < key_size; i++){	
//		fprintf(stderr, "i = %d\n", i+1);
		*(cyphertexts + i) = get_n_values(key_size, i, cyphertext_ascii);
//		print_array(*(cyphertexts + i), arr_size/key_size);
//		fprintf(stderr, "\n");
		key[i] = get_shift(*(cyphertexts + i), arr_size/key_size);
		if(key[i] == 128){
			printf("Couldn't find a suitable key");
			exit(EXIT_FAILURE);
		}
	}

	for(i = 0; i < arr_size; i++){
		printf("%c", cyphertext_ascii[i] ^ key[i % key_size]);
	}
	printf("\nKey is: ");
	for(i = 0; i < key_size; i++)
		printf("0x%02X ", key[i]);
	printf("\n");
	
	exit(EXIT_SUCCESS);
}