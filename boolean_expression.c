/*
Submit a solution for B- (Variant 5)
Задача B-5. Парсер, логические выражения.
Time limit:	14 s
Memory limit:	64 M
Требуется написать программу, которая способна вычислять логическе выражения. 
Допустимые выражения чувствительны к регистру и могут содержать: 
1) знаки операций 'and' - и, 'or' - или, 'not' - отрицание, 'xor' - сложение по модулю 2 
2) Скобки '(', ')' 
3) Логические значения 'True', 'False' 
4) Пустое выражение имеет значение "False" 

Также требуется реализовать возможность задания переменных, которые могут состоять только из маленьких букв латинского алфавита (к примеру, 'x=True'). 
Объявление переменной имеет формат: 
<имя>=<True|False>; // допустимы пробелы 

Допускается несколько объявлений одной и той же переменной, учитывается последнее. 
Все переменные, используемые в выражении, должны быть заданы заранее. Также запрещены имена, совпадающие с ключевыми словами (and, or и т.д.). 

Необходимо учитывать приоритеты операций. Пробелы ничего не значат. 

Если выражение или объявление не удовлетворяют описанным правилам, требуется вывести в стандартный поток вывода "[error]" (без кавычек).

runid: 1888
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

// 1) CONST BLOCK

size_t KEY_DOES_NOT_EXIST_ON_ASSOCARRAY = -1;

const int EXECUTE_ERROR = -1;
const int ASSOC_ARRAY_ERROR = -1;
const int STACK_ARRAY_ERROR = -1; 

const int EXECUTE_TEXT_INIT_LENGTH = 3;
const int ASSOC_ARRAY_INIT_LENGTH = 10;
const int STACK_ARRAY_INIT_LENGTH = 10;
const int INPUT_STRING_INIT_LENGTH = 10;

const char *TRUE_STRING = "True\0";
const char *FALSE_STRING = "False\0";

const char *MY_OPERATOR_EQUAL = "=\0";
const char  MY_OPERATOR_EQUAL_CHAR = '=';
const char *MY_OPERATOR_BRACKET_OPEN = "(\0";
const char *MY_OPERATOR_BRACKET_CLOSE = ")\0";
const char  MY_OPERATOR_BRACKET_OPEN_CHAR = '(';
const char  MY_OPERATOR_BRACKET_CLOSE_CHAR = ')';

const char *MY_OPERATOR_OR = "or\0";
const char *MY_OPERATOR_AND = "and\0";
const char *MY_OPERATOR_XOR = "xor\0";
const char *MY_OPERATOR_NOT = "not\0";

const int MY_OPERATOR_OR_PRIORITY = 1001;
const int MY_OPERATOR_AND_PRIORITY = 1002;
const int MY_OPERATOR_XOR_PRIORITY = 1003;
const int MY_OPERATOR_NOT_PRIORITY = 1004;
const int MY_OPERATOR_DEFAULT_PRIORITY = -1000;


// 2) ASSOCARRAY BLOCK

// Ассоциативный массив (строка => логическое значение)
struct AssocArray {
	char **names; // ключи
	bool *values; // значения

	size_t length; // длина выделенной памяти
	size_t elements; // действительное количество элементов
};

struct AssocArray *AssocArray_init();
void AssocArray_clear(struct AssocArray *arr);
int AssocArray_find_key(struct AssocArray *arr, const char *key);
// заполнение ассоциативного массива ключами
int AssocArray_insert(struct AssocArray *arr, const char *key, const bool val);


// 3) STACKARRAY BLOCK

// Стек строк с возможностями массива (получение/задание элементов по индексу)
struct StackArray {
	char **values; // значения

	size_t length; // длина выделенной памяти
	size_t elements; // действительное количество элементов
};

int StackArray_push(struct StackArray *arr, const char *val);
char *StackArray_pop(struct StackArray *arr);
// запись элемента в стек на определенную позицию
int StackArray_set_element(struct StackArray *arr, size_t index, const char *val);
// Получение элемента из стека с определенной позицией
char *StackArray_get_element(struct StackArray *arr, size_t index);
struct StackArray *StackArray_init();
void StackArray_clear(struct StackArray *arr);


// 4) MY STRING FUNCTIONS

// удаление символов в строке с начала и с конца (Изменяет переданную строку)
char *cut(char *source, char symbol);

char *inputString();                                  // ввод строки
char *get_strcpy(const char *source);                    // копирование строки
char *get_substrcpy(const char *source, size_t max_len);  // копирование строки max_len-элементов
bool strings_are_equal(const char *first_str, const char *second_str);


// 5) EXECUTE FUNSTIONS
// исполнение команд в текст по возможности
int executeText(char *text);
// исполнение команд в строке по возможности
int executeExpression(char *expr, struct AssocArray *vars);
// выполнение операций согласно стеку операндов
int executeOperandsStack(struct StackArray *stack, struct AssocArray *vars);

int getOperatorPriority(const char *operator);
bool isOperator(const char *operator);

int main() {
	// ввод текста (разделение строк знаком '\n')

	char *text = inputString();

	if (text != NULL) {
		// Иполняем текст
		int result = executeText(text);

		if (result == 0) {
		printf("False");
		} else if (result == 1) {
			printf("True");
		} else {
			// не удалось "исполнить" текст
			printf("[error]");
		}
	} else {
		// не удалось ввести текст
		printf("[error]");
	}

	free(text);
	return 0;
}


// 2) ASSOCARRAY BLOCK

// заполнение ассоциативного массива ключами
int AssocArray_insert(struct AssocArray *arr, const char *key, const bool val) {
	assert(arr != NULL);
	assert(key != NULL);

	if (strings_are_equal(key, MY_OPERATOR_OR)
		|| strings_are_equal(key, MY_OPERATOR_AND)
			|| strings_are_equal(key, MY_OPERATOR_XOR)
				|| strings_are_equal(key, MY_OPERATOR_NOT)) {
		return ASSOC_ARRAY_ERROR;
	}

	for (size_t i = 0; i < strlen(key); i++) {
		if ((key[i] < 'a') || (key[i] > 'z')) {
			return ASSOC_ARRAY_ERROR;
		}
	}

	size_t index = KEY_DOES_NOT_EXIST_ON_ASSOCARRAY;

	// ищем ключ var в уже существующих ключах
	for (size_t i = 0; i < arr->elements; ++i) {
		if (strings_are_equal(key, arr->names[i])) {
			index = i;
		}
	}

	if (index != KEY_DOES_NOT_EXIST_ON_ASSOCARRAY) {
		arr->values[index] = val;
	} else {
		if (arr->elements == arr->length) {
	 		// удваиваем память, если достигли максимальное кол-во элементов
			size_t new_length = arr->length * 2 * sizeof(char *);
			char **new_mem_for_names = (char **)realloc(arr->names, new_length * sizeof(char*));
			if (new_mem_for_names == NULL) {
				return ASSOC_ARRAY_ERROR;
			}
			arr->names = new_mem_for_names;

			bool *new_mem_for_values = (bool *)realloc(arr->values, new_length * sizeof(bool));
			if (new_mem_for_values == NULL) {
				return ASSOC_ARRAY_ERROR;
			}
			arr->values = new_mem_for_values;

			arr->length = new_length;
		}

		// копируем ключ
		char *key_copy = (char *)calloc(strlen(key) + 1, sizeof(char));
		if (key_copy == NULL) {
			return ASSOC_ARRAY_ERROR;
		}

		strcpy(key_copy, key);

		// сохраняем ключ и значение
		arr->elements++;
		arr->names[arr->elements - 1] = key_copy;
		arr->values[arr->elements - 1] = val;
	}

	return 0;
}


int AssocArray_find_key(struct AssocArray *arr, const char *key) {
	assert(arr != NULL);
	assert(key != NULL);

	size_t index = -1;

	// ищем ключ var
	for (size_t i = 0; i < arr->elements; ++i) {
		if (strings_are_equal(key, arr->names[i])) {
			index = i;
		}
	}

	if (index == -1) {
		// ключ не найден
		return ASSOC_ARRAY_ERROR;
	}
	// ключ найден, возвращаем
	return arr->values[index];
}


struct AssocArray *AssocArray_init() {
	struct AssocArray *arr = (struct AssocArray *)calloc(1, sizeof(struct AssocArray));
	if (arr == NULL) {
		return NULL;
	}

	size_t length = ASSOC_ARRAY_INIT_LENGTH;
	arr->names = (char **)calloc(length, sizeof(char *));
	if (arr->names == NULL) {
		free(arr);
		return NULL;
	}

	arr->values = (bool *)calloc(length, sizeof(bool));
	if (arr->values == NULL) {
		free(arr->names);
		free(arr);
		return NULL;
	}

	arr->length = length;
	arr->elements = 0;

	return arr;
}

void AssocArray_clear(struct AssocArray *arr) {
	assert(arr != NULL);

	for (size_t i = 0; i < arr->elements; ++i) {
		free(arr->names[i]);
	}
	free(arr->names);
	free(arr->values);
	free(arr);
}


// 3) STACKARRAY BLOCK

int StackArray_push(struct StackArray *arr, const char *val) {
	assert(arr != NULL);
	assert(val != NULL);

	if (arr->length == arr->elements) {
		// удваиваем память, если достигли максимальное кол-во элементов
		size_t new_length = arr->length * 2 * sizeof(char *);
		char **new_mem = (char **)realloc(arr->values, new_length * sizeof(char*));
		if (new_mem == NULL) {
			return STACK_ARRAY_ERROR;
		}
		arr->values = new_mem;
		arr->length = new_length;
	}

	// копируем строку
	char *val_copy = (char *)calloc(strlen(val) + 1, sizeof(char));
	if (val_copy == NULL) {
		return STACK_ARRAY_ERROR;
	}

	strcpy(val_copy, val);

	// сохраняем строку
	arr->elements++;
	arr->values[arr->elements - 1] = val_copy;

	return 0;
}

char *StackArray_pop(struct StackArray *arr) {
	assert(arr != NULL);
	if (arr->elements == 0) {
		return NULL;
	}
	return arr->values[(arr->elements--) - 1];
}

int StackArray_set_element(struct StackArray *arr, size_t index, const char *val) {
	assert(arr != NULL);
	assert(index < arr->elements);
	assert(val != NULL);

	char *val_copy = (char *)calloc(strlen(val) + 1, sizeof(char));
	if (val_copy == NULL) {
		return STACK_ARRAY_ERROR;
	}

	strcpy(val_copy, val);


	free(arr->values[index]);
	arr->values[index] = val_copy;

	return 0;
}

char *StackArray_get_element(struct StackArray *arr, size_t index) {
	assert(arr != NULL);
	assert(index < arr->elements);

	return arr->values[index];
}


struct StackArray *StackArray_init() {
	struct StackArray *arr = (struct StackArray *)calloc(1, sizeof(struct StackArray));
	if (arr == NULL) {
		return NULL;
	}

	size_t length = STACK_ARRAY_INIT_LENGTH;
	arr->values = (char **)calloc(length, sizeof(char *));
	if (arr->values == NULL) {
		free(arr);
		return NULL;
	}

	arr->length = length;
	arr->elements = 0;

	return arr;
}

void StackArray_clear(struct StackArray *arr) {
	assert(arr != NULL);
	for (size_t i = 0; i < arr->elements; ++i) {
		free(arr->values[i]);
	}
	free(arr->values);
	free(arr);
}


// 4) MY STRING FUNCTIONS

// удаляет символы с конца и начала
char *cut(char *source, char symbol) {
	if (source == NULL) {
		return NULL;
	}
	size_t i = 0;
	size_t j = 0;
	size_t len = strlen(source);
	while (source[i] == symbol) {
		i++;
	}
	if (i > 0) {
		for (j = 0; j < len; ++j) {
			source[j] = source[j + i];
			if (source[j + i] == '\0') {
				break;
			}
		}
		source[j] = '\0';
	}

	len = strlen(source);
	if (len > 0) {
		i = len - 1;
		while (source[i] == symbol) {
			i--;
		}
		if (i < len - 1) {
			source[i + 1] = '\0';
		}
	}
	return source;
}

char *get_strcpy(const char *source) {
	if (source == NULL) {
		return NULL;
	}

	size_t len = strlen(source);
	char *str = (char *)calloc(len + 1, sizeof(char));
	if (!str) {
		return NULL;
	}

	strcpy(str, source);
	return str;
}

// Копирование строки до max_len
char *get_substrcpy(const char *source, size_t max_len) {
	if (source == NULL) {
		return NULL;
	}

	size_t len = strlen(source);
	if (max_len < len) {
		len = max_len;
	}
	char *str = (char *)calloc((len + 1), sizeof(char));

	if (!str) {
		return NULL;
	}

	for (size_t i = 0; i < len; ++i) {
		str[i] = source[i];
	}
	str[len] = '\0';
	return str;
}

char *inputString() {
	char *str = NULL;
	char c = 0;

	size_t i = 0;
	size_t length = INPUT_STRING_INIT_LENGTH;

	while (scanf("%c", &c) == 1) {
		char *new_str = str;
		if (str == NULL) {
			new_str = (char *)calloc(length, sizeof(char));
		} else if (i == length - 2) {
			length *= 2;
			new_str = (char *)realloc(str, sizeof(char) * length);
		}

		if (new_str == NULL) {
			free(str);
			return NULL;
		}
		str = new_str;

		str[i++] = c;
	}

	if (str != NULL) {
		str[i] = '\0';
	}

	return str;
}


bool strings_are_equal(const char *first_str, const char *second_str) {
	if (strlen(first_str) != strlen(second_str)) {
		return false;
	}

	for (size_t i = 0; i < strlen(first_str); ++i) {
		if (first_str[i] != second_str[i]) {
			return false;
		}
	}

	return true;
}


// 5) EXECUTE FUNSTIONS

// исполнение команд в текст по возможности
int executeText(char *text) {
	assert(text != NULL);

	struct AssocArray *vars = AssocArray_init();
	if (vars == NULL) {
		return EXECUTE_ERROR;
	}

	size_t expressions_count = 0;
	size_t expression_mem = EXECUTE_TEXT_INIT_LENGTH;

	char **expressions = (char **)calloc(EXECUTE_TEXT_INIT_LENGTH, sizeof(char*));

	if (expressions == NULL) {
		AssocArray_clear(vars);
		return EXECUTE_ERROR;
	}

	char *text_part = strtok(text, ";\n");
	while (text_part != NULL) {
		if (strlen(text_part) > 0) {
			if (expression_mem == expressions_count) {
				expression_mem *= 2;
				char **new_expressions_mem = (char **)realloc(expressions, expression_mem * sizeof(char *));
				if (new_expressions_mem == NULL) {
					free(expressions);
					AssocArray_clear(vars);
					return EXECUTE_ERROR;
				}
				expressions = new_expressions_mem;
			}
			expressions[expressions_count++] = text_part;
		}
		text_part = strtok(NULL, ";\n");
	}

	int res = EXECUTE_ERROR;

	for (size_t i = 0; i < expressions_count; ++i) {
		char *expression = get_strcpy(expressions[i]);
		if (expression == NULL) {
			free(expressions);
			AssocArray_clear(vars);
			return EXECUTE_ERROR;
		}
		res = executeExpression(expression, vars);
		free(expression);

		if (res == EXECUTE_ERROR) {
			break;
		}
	}

	free(expressions);
	AssocArray_clear(vars);

	return res;
}


// исполнение команд в строке по возможности
int executeExpression(char *expr, struct AssocArray *vars) {
	assert(expr != NULL);
	assert(vars != NULL);


	size_t expr_length = strlen(expr);                 // <!-- ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ
	for (size_t i = 0; i < expr_length; ++i) {
		if (expr[i] == MY_OPERATOR_EQUAL_CHAR) {
			char *var_name = cut(get_substrcpy(expr, i), ' ');

			if (var_name == NULL) {
				return EXECUTE_ERROR;
			}
			char *var_expr = cut(get_substrcpy(expr + i + 1, expr_length - i), ' ');

			if (var_expr == NULL) {
				free(var_name);
				return EXECUTE_ERROR;
			}


			int value = executeExpression(var_expr, vars);
			if (value != EXECUTE_ERROR) {
				if (AssocArray_insert(vars, var_name, value) == ASSOC_ARRAY_ERROR) {
					free(var_name);
					free(var_expr);
					return EXECUTE_ERROR;
				}
			}


			free(var_name);
			free(var_expr);
			return value;
		}
	}                                                  // ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ -->
	// если не встретился символ '=', пробует обработать выражение.

	struct StackArray *operators = StackArray_init();  // <!-- ОБРАБОТКА КОНЕЧНОГО ВЫРАЖЕНИЯ
	if (operators == NULL) {
		return EXECUTE_ERROR;
	}

	struct StackArray *operands = StackArray_init();
	if (operands == NULL) {
		free(operators);
		return EXECUTE_ERROR;
	}

	char *token = strtok(expr, " ");
	size_t token_length = (token == NULL) ? 0 : strlen(token);
	while (token != NULL) {
		token_length = strlen(token);
		if (token_length > 0) {
			// обрабатываем открывающие скобки
			while (token[0] == MY_OPERATOR_BRACKET_OPEN_CHAR) {
				token_length = strlen(token);
				if (token_length == 0) {
					break;
				}
				if (StackArray_push(operators, MY_OPERATOR_BRACKET_OPEN) == STACK_ARRAY_ERROR) {
					StackArray_clear(operators);
					StackArray_clear(operands);
					return EXECUTE_ERROR;
				}
				cut(token, MY_OPERATOR_BRACKET_OPEN_CHAR);
			}

			// обрабатываем операторы и операнды
			char *token_copy = cut(get_substrcpy(token, token_length), MY_OPERATOR_BRACKET_CLOSE_CHAR);
			if (token_copy == NULL) {
				StackArray_clear(operators);
				StackArray_clear(operands);
				return EXECUTE_ERROR;
			}

			if (isOperator(token_copy)) {
				// нашли оператор
				char *operator = NULL;
				while (operators->elements > 0) {
					// все операторы большего или равного приоритета заносим в стек операндов
					operator = StackArray_pop(operators);
					if (operator == NULL || getOperatorPriority(operator) < getOperatorPriority(token_copy) ) {
						free(operator);
						break;
					}
					if (StackArray_push(operands, operator) == STACK_ARRAY_ERROR) {
						StackArray_clear(operators);
						StackArray_clear(operands);
						free(operator);
						return EXECUTE_ERROR;
					}
					free(operator);
				}
				// заносим текущий оператор в стек операторов
				if (StackArray_push(operators, token_copy) == STACK_ARRAY_ERROR) {
					StackArray_clear(operators);
					StackArray_clear(operands);
					free(token_copy);
					return EXECUTE_ERROR;
				}
			} else if (strlen(token_copy) > 0) {
	  			// заносим операнд в стек операндов
				if (StackArray_push(operands, token_copy) == STACK_ARRAY_ERROR) {
					StackArray_clear(operators);
					StackArray_clear(operands);
					free(token_copy);
					return EXECUTE_ERROR;
				}
			}

			// обрабатываем закрывающие скобки
			while (token && (token_length = strlen(token)) > 0 && token[token_length - 1] == MY_OPERATOR_BRACKET_CLOSE_CHAR) {
				char *top_token = NULL;
				while ((top_token = StackArray_pop(operators)) != NULL && !strings_are_equal(top_token, MY_OPERATOR_BRACKET_CLOSE)) {
					if (StackArray_push(operands, top_token) == STACK_ARRAY_ERROR) {
						StackArray_clear(operators);
						StackArray_clear(operands);
						free(top_token);
						free(token_copy);
						return EXECUTE_ERROR;
					}
					free(top_token);
					top_token = NULL;
				}
				free(top_token);
				token[token_length - 1] = '\0';
			}
			free(token_copy);
		}
		token = strtok(NULL, " ");
	}                                                  // ОБРАБОТКА КОНЕЧНОГО ВЫРАЖЕНИЯ -->

	while (operators->elements > 0) {
		char *operator = StackArray_pop(operators);
		if (StackArray_push(operands, operator) == STACK_ARRAY_ERROR) {
			StackArray_clear(operators);
			StackArray_clear(operands);
			free(operator);
			return EXECUTE_ERROR;
		}
		free(operator);
	}

	int res = executeOperandsStack(operands, vars);

	StackArray_clear(operators);
	StackArray_clear(operands);

	return res;
}

// выполняет операции, записанные в стек операндов
int executeOperandsStack(struct StackArray *stack, struct AssocArray *vars) {
	assert(stack != NULL);
	assert(vars != NULL);

	for (size_t i = 0; i < stack->elements; ++i) {
		char *cur = StackArray_get_element(stack, i);

		if (!isOperator(cur) && !strings_are_equal(cur, TRUE_STRING) != 0 && !strings_are_equal(cur, FALSE_STRING) != 0 && AssocArray_find_key(vars, cur) == ASSOC_ARRAY_ERROR) {
			return EXECUTE_ERROR;
		}

		if (!isOperator(cur)) {
			continue;
		}

		if (strings_are_equal(cur, MY_OPERATOR_NOT)) {
			// оператор NOT
			char *operand = StackArray_get_element(stack, i - 1);
			int eval = executeExpression(operand, vars);

			if (eval != EXECUTE_ERROR) {
				if (StackArray_set_element(stack, i, (!eval ? TRUE_STRING : FALSE_STRING)) == STACK_ARRAY_ERROR) {
					return EXECUTE_ERROR;
				}
	 		} else {
				return EXECUTE_ERROR;
			}
		} else {
			// операторы xor, or, and
			char *operand1 = StackArray_get_element(stack, i - 1);
			char *operand2 = StackArray_get_element(stack, i - 2);

			int eval1 = executeExpression(operand1, vars);
			int eval2 = executeExpression(operand2, vars);

			int res = EXECUTE_ERROR;

			if (eval1 == EXECUTE_ERROR || eval2 == EXECUTE_ERROR) {
				return EXECUTE_ERROR;
			} else if (strings_are_equal(cur, MY_OPERATOR_XOR)) {
				res = eval1 ^ eval2;
			} else if (strings_are_equal(cur, MY_OPERATOR_OR)) {
				res = eval1 || eval2;
			} else if (strings_are_equal(cur, MY_OPERATOR_AND)) {
				res = eval1 && eval2;
			}

			if (res == 0 || res == 1) {
				if (StackArray_set_element(stack, i, res ? TRUE_STRING : FALSE_STRING) == STACK_ARRAY_ERROR) {
					return EXECUTE_ERROR;
				}
			} else {
				return EXECUTE_ERROR;
			}
		}
	}

	char *last_val = StackArray_pop(stack);
	int result = EXECUTE_ERROR;
	if (last_val != NULL) {
		if (strings_are_equal(last_val, TRUE_STRING)) {
			result = 1;
		} else if (strings_are_equal(last_val, FALSE_STRING)) {
			result = 0;
		} else {
			result = AssocArray_find_key(vars, last_val);
		}
	}
	free(last_val);

	return result;
}


int getOperatorPriority(const char *operator) {
	if (operator == NULL) {
		return MY_OPERATOR_DEFAULT_PRIORITY;
	}

	if (strings_are_equal(operator, MY_OPERATOR_NOT)) {
		return MY_OPERATOR_NOT_PRIORITY;
	} else if (strings_are_equal(operator, MY_OPERATOR_XOR)) {
		return MY_OPERATOR_XOR_PRIORITY;
	} else if (strings_are_equal(operator, MY_OPERATOR_AND)) {
		return MY_OPERATOR_AND_PRIORITY;
	} else if (strings_are_equal(operator, MY_OPERATOR_OR)) {
		return MY_OPERATOR_OR_PRIORITY;
	}
	return MY_OPERATOR_DEFAULT_PRIORITY;
}

bool isOperator(const char *operator) {
	return getOperatorPriority(operator) != MY_OPERATOR_DEFAULT_PRIORITY;
}
