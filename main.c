/*
Составить программу построчной обработки текста. Суть обработки - отбор строк, содержащих одинаковое количество открывающих и закрывающих круглых скобок. 

Программа считывает входные данные со стандартного ввода, и печатает результат в стандартный вывод. 

Процедура отбора нужных строк должна быть оформлена в виде отдельной функции, которой на вход подается массив строк (который необходимо обработать), количество переданных строк, а также указатель на переменную, в которой необходимо разместить результат - массив отобранных строк. 
В качестве возвращаемого значения функция должна возвращать количество строк, содержащихся в результирующем массиве. 

Программа должна уметь обрабатывать ошибки - такие как неверные входные данные(отсутствие входных строк) или ошибки выделения памяти и т.п.
В случае возникновения ошибки нужно выводить об этом сообщение "[error]" и завершать выполнение программы. 

Run ID: 1220

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const long int MAX_STR_LEN = 1024;
const long int MAX_ARR_LEN = 256;

void free_str_arr(char** str, long int len);
int get_str(char* str, long int max_len);
long int get_arr_str(char** str_arr, long int arr_len, long int max_str_len);
long int get_right_string(char** str_arr, long int str_arr_len, long int max_str_len, char** res_arr);

int main() {
	char** str_arr = (char**)calloc(MAX_ARR_LEN, sizeof(char*));
	if (str_arr == NULL) {
		printf("error");
		return 0;
	}

	for (long int i = 0; i < MAX_ARR_LEN; i++) {
		str_arr[i] = (char*)calloc(MAX_STR_LEN, sizeof(char));
		if (str_arr[i] == NULL) {
			free_str_arr(str_arr, i - 1);
			printf("error");
			return 0;
		}
	}

	int str_arr_len = get_arr_str(str_arr, MAX_ARR_LEN, MAX_STR_LEN);
	if (str_arr_len == 0) {
		printf("error");
		return 0;
	}

	char** res_arr = (char**)calloc(str_arr_len, sizeof(char*));
	if (res_arr == NULL) {
		printf("error");
		return 0;
	}

	for (long int i = 0; i < str_arr_len; i++) {
		res_arr[i] = (char*)calloc(MAX_STR_LEN, sizeof(char));
		if (res_arr[i] == NULL) {
			free_str_arr(res_arr, i - 1);
			printf("error");
			return 0;
		}
	}

	long int res_arr_len = get_right_string(str_arr, str_arr_len, MAX_STR_LEN, res_arr);
	for (long int i = 0; i < res_arr_len; i++) {
		long int j = 0;
		while ((j < MAX_STR_LEN) && (res_arr[i][j] != '\0') && (res_arr[i][j] != '\n') && (res_arr[i][j] != '\r')) {
			printf("%c", res_arr[i][j]);
			j++;
		}
		printf("\n");
	}

	free_str_arr(str_arr, MAX_ARR_LEN);
	free_str_arr(res_arr, str_arr_len);
	return 0;
}

void free_str_arr(char** str, long int len) {
	for (long int i = 0; i < len; i++) {
		free(str[i]);
	}
	free(str);
}

int get_str(char* str, long int max_len) {
	long int i = 0;
	char buff = getchar();
	while ((i < max_len - 1) && (buff != '\0') && (buff != '\n') && (buff != '\r') && (buff != EOF)) {
		str[i] = buff;
		buff = getchar();
		i++;
	}
	str[i] = '\0';
	if (buff == EOF) {
		str[i] = '\0';
		return 1;
	}
	return 0;
}

long int get_arr_str(char** str_arr, long int arr_len, long int max_str_len) {
	int len = 0;
	while (len < arr_len) {
		int code = get_str(str_arr[len], max_str_len);
		if (code == 1) { 
			if ((str_arr[len][0] == '\0') || (str_arr[len][0] == '\r') || (str_arr[len][0] =='\n')) {
				free(str_arr[len]);
			} else {
				len++;
			}
			return len;
		}
		len++;
	}
	return len;
}

long int get_right_string(char** str_arr, long int str_arr_len, long int max_str_len, char** res_arr) {
	long int res_len = 0;
	for (long int i = 0; i < str_arr_len; i++) {
		long int opens_count = 0;
		long int closes_count = 0;

		long int j = 0;
		while ((j < max_str_len - 1) && (str_arr[i][j] != '\0') && (str_arr[i][j] != '\n') && (str_arr[i][j] != '\r')) {
			if (str_arr[i][j] == '(') {
				opens_count++;
			} else if (str_arr[i][j] == ')') {
				closes_count++;
			}
			j++;
		}
		if (opens_count == closes_count) {
			for (long int k = 0; k < j; k++) {
				res_arr[res_len][k] = str_arr[i][k];
			}
			res_arr[res_len][j] = '\0';
			res_len++;
		}
	}

	return res_len;
}


