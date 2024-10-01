#include <stdio.h>
#define GRN "\e[0;32m"
#define RED "\e[0;31m"
#define YEL "\e[0;33m"
#define COLOR_RESET "\e[0m"
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct TradeHistory{
	int value;
	int position;
	int TP;
	int SL;
	int reward;
	struct TradeHistory* next;
};

struct TradeHistoryInnerList{
	struct TradeHistory* head;
	struct TradeHistory* tail;
};

//login register code
struct Person {
	char username[100];
	char email[100];
	char password[100];
	int balance;
	int inTrade;
	int currTP, currSL, currPosition, currValue;
	int isShort, isLong, tradeOpen, tradeActive;
	struct Person* next;
	struct TradeHistoryInnerList inner_list;
}*headRow[26], *currPerson;

struct TradeHistory* createInnerNode(int value, int position, int TP, int SL, int reward) {
	struct TradeHistory* newNode = (struct TradeHistory*)malloc(sizeof(struct TradeHistory));
	newNode->value = value;
	newNode->position = position;
	newNode->SL = SL;
	newNode->TP = TP;
	newNode->reward = reward;
	newNode->next = NULL;
	return newNode;
}

struct Person* createNodeP(const char* username, const char* email, const char* password, int balance) {
	struct Person* newNode = (struct Person*)malloc(sizeof(struct Person));
	newNode->balance = balance;
	newNode->currPosition = 0;
	newNode->currSL = 0;
	newNode->currTP = 0;
	newNode->currValue = 0;
	newNode->inTrade = 0;
	newNode->isLong = newNode->isShort = newNode->tradeActive = newNode->tradeOpen = 0;
	strcpy(newNode->email, email);
	strcpy(newNode->password, password);
	strcpy(newNode->username, username);
	newNode->next = NULL;
	newNode->inner_list.head = NULL;
    newNode->inner_list.tail = NULL;
	return newNode;
}

//insert innernode
void pushHeadTH(struct TradeHistoryInnerList* list, int value, int position, int TP, int SL, int reward) {
	struct TradeHistory* newNode = createInnerNode(value, position, TP, SL, reward);
	if(list->head == NULL) {
		list->head = list->tail = newNode;
	} else {
		newNode->next = list->head;
		list->head = newNode;
	}
}

void pushTailTH(struct TradeHistoryInnerList* list, int value, int position, int TP, int SL, int reward) {
	struct TradeHistory* newNode = createInnerNode(value, position, TP, SL, reward);
	if(list->head == NULL) {
		list->head = list->tail = newNode;
	} else {
		list->tail->next = newNode;
		list->tail = newNode;
	}
}

void pushMidTH(struct TradeHistoryInnerList* list, int value, int position, int TP, int SL, int reward) {
	struct TradeHistory* newNode = createInnerNode(value, position, TP, SL, reward);

	if(list->head == NULL) {
		list->head = list->tail = newNode;
	} else if(reward < list->head->reward) {
		pushHeadTH(list, value, position, TP, SL, reward);
	} else if(reward > list->tail->reward) {
		pushTailTH(list, value, position, TP, SL, reward);
	}
	else {
		struct TradeHistory* curr = list->head;
		while(reward > curr->next->reward) {
			curr = curr->next;
		}
		newNode->next = curr->next;
		curr->next = newNode;
	}
}

void printTradeHistory(struct TradeHistoryInnerList* list) {
    struct TradeHistory* curr = list->head;
    if(list->head == NULL) {
		puts("No Trade History");
		
		return;
	}
	printf("");
	int number = 1;
    while(curr != NULL) {
		printf("No: %d\n", number);
		printf("Value		: %d\n", curr->value);
		printf("Position	: %d\n", curr->position);
		printf("Take Profit	: %d\n", curr->TP);
		printf("Stop Loss	: %d\n", curr->SL);
		printf("Reward		: %d\n", curr->reward);
		curr = curr->next;
		number += 1;
	}
	return;
}

//innernode

int hash(const char* name) {
	int len = strlen(name);
	int sum = 0;
	for(int i = 0 ; i<len; i++) {
		sum += (int)name[i];
	}
	sum %= 26;
	return sum;
}

void insert(const char* username, const char* email, const char* password, int balance) {
	struct Person* newNode = createNodeP(username, email, password, balance);
	int index = hash(username);
	if(headRow[index] == NULL) {
		headRow[index] = newNode;
	} else {
		newNode->next = headRow[index];
		headRow[index] = newNode;
	}
	currPerson = headRow[index];
}

int search(const char* username) {
	int index = hash(username);
    if (headRow[index] != NULL) {
        if(strcmp(headRow[index]->username, username) == 0) {
        	currPerson = headRow[index];
            return 1;
        } else {
            struct Person* curr = headRow[index]->next;
            while(curr != NULL) {
                if(strcmp(curr->username, username) == 0) {
                	currPerson = curr;
                    return 1;
                }
                curr = curr->next;
            }
        }
    }
	
	return 0;
}

int unique(char* str, const char* category) {

	char username[100];
	char email[100];
	char password[100];
	int balance, found = 0;
	FILE *fp = fopen("./account/user.txt", "r");
	
	
	
	while(!feof(fp)) {
		fscanf(fp, "%[^#]#%[^#]#%[^#]#%d\n", username, email, password, &balance);
		
		if(strcmp(category, "email") == 0) {
			if(strcmp(email, str) == 0 ) {
				found = 1;
				break;
				
			}
		} else if(strcmp(category, "username") == 0) {
			if(strcmp(username, str) == 0 ) {
				found = 1;
				break;
				
			}
		}
		
	}
	fclose(fp);
	
	return found;
	
}

void append(char* username, char* email, char* password, int balance) {
	FILE *fp = fopen("./account/user.txt", "a");
	fprintf(fp, "%s#%s#%s#%d\n", username, email, password, balance);
	fclose(fp);
	return;
}

int searchFile(char* emailToSearch, char* passwordToSearch) {
	char username[100];
	char email[100];
	char password[100];
	int balance, found = 0;
	FILE *fp = fopen("./account/user.txt", "r");
	while(!feof(fp)) {
		fscanf(fp, "%[^#]#%[^#]#%[^#]#%d\n", username, email, password, &balance);
		if(strcmp(email, emailToSearch) == 0 && strcmp(password, passwordToSearch) == 0) {
			found = 1;
			break;
			
		}
	}
	fclose(fp);
	if(found == 1) {
		if(search(username) == 0) {
			insert(username, email, password, balance);
		} 
		
	}
	return found;
}

int alphaNumeric(const char* str) {
	int len = strlen(str);
	int isAlpha = 1;
	for(int i = 0; i<len; i++) {
		if(str[i] >= 'A' && str[i] <= 'Z') {
			continue;
		} else if(str[i] >= 'a' && str[i] <= 'z') {
			continue;
		} else if(str[i] >= '0' && str[i] <= '9') {
			continue;
		} 
		else {
			isAlpha = 0;
		}
	}
	return isAlpha;
}

int oneNumLetter(const char* str) {
	int len = strlen(str);
	int num = 0;
	int letter = 0;
	int correct = 0;
	for(int i = 0; i<len; i++) {
		if(letter > 0 && num > 0) {
			correct = 1;
			break;
		}
		else if(str[i] >= 'A' && str[i] <= 'Z') {
			letter++;
		} else if(str[i] >= 'a' && str[i] <= 'z') {
			letter++;
		} else if(str[i] >= '0' && str[i] <= '9') {
			num++;
		}
		if(letter > 0 && num > 0) {
	        correct = 1;
	    } 
	}
	return correct;
}

int dotcom(const char* email) {
	int len = strlen(email);
	int len2 = len - 4;	
	char check[5];
	for(int i = 0; i<4; i++) {
		check[i] = email[len2];
		len2++;
	}
	check[len2] = '\0';
	
	if(strcmp(check, ".com") == 0) {
		return 1;
	}
	return 0;
}

int validateEmail(char* email) {
	char temp[100];
	strcpy(temp, email);
	int len = strlen(email);
	if(len < 6 || len > 36) {
		return 1;
	}
	
	if(email[0] < 'A' || email[0] > 'Z') {
		if(email[0] < 'a' || email[0] > 'z') {
			return 2;
		}
	}  
	int exist = 0, found = 0;
	for(int i = 0; i<len; i++) {
		if(email[i] == '@') {
			found += 1;
		} else if(email[i] == '#') {
			return 3;
		} else if(email[i] == ' ') {
			return 4;
		}
	}
	if(found == 1) {
		exist = 1;
	} 
	if(exist != 1) {
		return 5;
	}
	char emailPart[40];
	char domainPart[40];
	if(exist == 1) {
		char* token = strtok(email, " @ ");
		strcpy(emailPart, token);
		token = strtok(NULL, " @ ");
		strcpy(domainPart, token);
	    if(alphaNumeric(emailPart) == 0) {
	    	return 6;
	    	//nathan.alex@gmail.com boleh
		}
		if(strlen(domainPart) == 4) {
			return 7;
		}
		if(dotcom(domainPart) == 0) {
			return 8;
		}
	}
	char toCheck[10];
	strcpy(toCheck, "email");
	
	if(unique(temp, toCheck) == 1) {
		return 9;
	}
	return 0;
	
}

void registerAcc() {
	char username[100];
	char email[50];
	char temp[50];
	char password[100];
	char toCheck[20];
	int len, res;
	do {
		system("cls");
		printf("Username (0 to exit) : ");
		scanf("%[^\n]", username); getchar();
		len = strlen(username);
		strcpy(toCheck, "username");
		if(strcmp(username, "0") == 0) {
			return;
		}
		if(len < 1 || len > 36) {
			puts("Username must be between 1-20 characters!");
			printf("Press Enter to Continue..."); getchar();
		} else if(alphaNumeric(username) == 0) {
			puts("Username must only contain alphabet and number");
			printf("Press Enter to Continue..."); getchar();
		} else if(unique(username, toCheck) == 1) {
			puts("Username must be unique");
			printf("Press Enter to Continue..."); getchar();
		}
	}while(len < 1 || len > 20 || alphaNumeric(username) == 0 || unique(username, toCheck) == 1);
	do {
		system("cls");
		printf("Email (0 to exit) : ");
		scanf("%[^\n]", email); getchar();
		strcpy(temp, email);
		res = validateEmail(temp);
		
		if(strcmp(email, "0") == 0) {
			return;
		}
		
		if(res == 1) {
			puts("Email must be between 6-36 characters!");
			printf("Press Enter to Continue..."); getchar();
		} else if (res == 2) {
			puts("Email must start with a character");
			printf("Press Enter to Continue..."); getchar();
		} else if (res == 3) {
			puts("Email can't contain tag '#'");
			printf("Press Enter to Continue..."); getchar();
		} else if (res == 4) {
			puts("Email can't contain white space");
			printf("Press Enter to Continue..."); getchar();
		}
		else if (res == 5) {
			puts("Email must contain one '@'");
			printf("Press Enter to Continue..."); getchar();
		} else if (res == 6) {
			puts("Email must only contain alphabet and number");
			printf("Press Enter to Continue..."); getchar();
		} else if (res == 7) {
			puts("Email must have domain");
			printf("Press Enter to Continue..."); getchar();
		} else if (res == 8) {
			puts("Email must end with '.com'");
			printf("Press Enter to Continue..."); getchar();
		} else if (res == 9) {
			puts("Email is already taken");
			printf("Press Enter to Continue..."); getchar();
		} 
		
	}while(res != 0); 
	do {
		system("cls");
		printf("Password (0 to exit) : ");
		scanf("%[^\n]", password); getchar();
		len = strlen(password);
		if(strcmp(password, "0") == 0) {
			return;
		}
		
		if(len < 8 || len > 36) {
			puts("Password must be between 8-36 characters!");
			printf("Press Enter to Continue..."); getchar();
		} else if(alphaNumeric(password) == 0) {
			puts("Password must not contain symbol");
			printf("Press Enter to Continue..."); getchar();
		} else if(oneNumLetter(password) == 0) {
			//ini salah?
			puts("Password must contain one alphabet and one number");
			printf("Press Enter to Continue..."); getchar();
		}
	}while(len < 8 || len > 36 || alphaNumeric(password) == 0 || oneNumLetter(password) == 0); 
	append(username, email, password, 10000);
}

int login() {
	char email[100] = {0};
	char password[100];
	int len;
	do {
		system("cls");
		do {
			
			printf("Email (0 to exit) : ");
			scanf("%[^\n]", email); getchar();
			len = strlen(email);
			if(strcmp(email, "0") == 0) {
				return 0;
			} else if(email[0] == 0) {
				puts("Invalid Email!");
				printf("Press Enter to Continue..."); getchar();
				system("cls");
			}
			
		}while(email[0] == '\0'); 
		system("cls");
		printf("Password (0 to exit) : ");
		scanf("%[^\n]", password); getchar();
		len = strlen(password);
		if(strcmp(password, "0") == 0) {
			return 0;
		}
		else if (searchFile(email, password) == 1) {
			return 1;
		} else if (searchFile(email, password) == 0) {
			puts("Email or Password Is Invalid!");
			printf("Press Enter to Continue..."); getchar();
			email[0] = 0;
		}
	} while(searchFile(email, password) == 0);
}


void startMenu() {
	puts("METAFI");
	puts("1. Log In To Trade");
	puts("2. Register Account");
	puts("3. Exit Application");
	printf(">> ");
}
//login register

//user menu
void userMenu() {
	puts("MetaFi - User's Menu");
	puts("---------------------");
	puts("1. Start Trade");
	puts("2. Trade History");
	puts("3. Guide");
	puts("4. Log Out");
	printf(">> ");
}
//user menu

char map[][500] = {
"+---------------------------------------------------------------------------------------------------------------------------------+",
"|-------------------------------------------------------------------------------------------------------------------------- | 330 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 320 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 310 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 300 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 290 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 280 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 270 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 260 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 250 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 240 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 230 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 220 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 210 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 200 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 190 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 180 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 170 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 160 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 150 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 140 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 130 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 120 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 110 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 100 |",
"|-------------------------------------------------------------------------------------------------------------------------- | 90  |",
"|-------------------------------------------------------------------------------------------------------------------------- | 80  |",
"|-------------------------------------------------------------------------------------------------------------------------- | 70  |",
"|-------------------------------------------------------------------------------------------------------------------------- | 60  |",
"|-------------------------------------------------------------------------------------------------------------------------- | 50  |",
"|-------------------------------------------------------------------------------------------------------------------------- | 40  |",
"|-------------------------------------------------------------------------------------------------------------------------- | 30  |",
"|-------------------------------------------------------------------------------------------------------------------------- | 20  |",
"|-------------------------------------------------------------------------------------------------------------------------- | 10  |",
"+---------------------------------------------------------------------------------------------------------------------------------+"
};

struct Candle{
	int openPrice;
	int closePrice;
	int highestPrice;
	int lowestPrice;
	struct Candle* next;
	struct Candle* prev;
}*head = NULL, *tail = NULL, *currStart = NULL;

/*
closePrice > openPrice = green
closePrice < openPrice = red
*/

struct Candle* createNode(int openPrice, int closePrice, int highestPrice, int lowestPrice) {
	struct Candle* newNode = (struct Candle*)malloc(sizeof(struct Candle));
	newNode->closePrice = closePrice;
	newNode->openPrice = openPrice;
	newNode->highestPrice = highestPrice;
	newNode->lowestPrice = lowestPrice;
	newNode->next = NULL;
	newNode->prev = NULL;
	return newNode;
}

void pushTail(int openPrice, int closePrice, int highestPrice, int lowestPrice) {
	struct Candle* newNode = createNode(openPrice, closePrice, highestPrice, lowestPrice);
	if(head == NULL) {
		head = tail = newNode;
	} else {
		newNode->prev = tail;
		tail->next = newNode;
		tail = newNode;
	
	}
}

void fullscreen()
{
	keybd_event(VK_MENU,0x38,0,0);
	keybd_event(VK_RETURN,0x1c,0,0);
	keybd_event(VK_RETURN,0x1c,KEYEVENTF_KEYUP,0);
	keybd_event(VK_MENU,0x38,KEYEVENTF_KEYUP,0);
}

void gotoxy(int x, int y) {
	COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    HANDLE hOut;
	CONSOLE_CURSOR_INFO ConCurInf;
	
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	
	ConCurInf.dwSize = 10;
	ConCurInf.bVisible = FALSE;
	
	SetConsoleCursorInfo(hOut, &ConCurInf);
}



int arr[50];

void setConversion() {
	int j = 1;
	for(int i = 33; i>0; i--) {
		arr[i] = j;
		j++;
	}
}

void set(int openPrice, int closePrice, int highestPrice, int lowestPrice, int col) {
	if(openPrice >= closePrice) {
		
		int coords;
		for(int i = openPrice+1; i<=highestPrice; i+=10) {
			coords = i / 10;
			if(coords > 0 && coords < 34) {
				
				
				gotoxy(col, arr[coords]);
				map[arr[coords]][col] = '#';
				
				printf(RED "%c" COLOR_RESET, 179);
			}
		}
		for(int i = closePrice; i<=openPrice; i+=10) {
			coords = i / 10;
			if(coords > 0 && coords < 34) {
				
				
				
				gotoxy(col, arr[coords]);
				map[arr[coords]][col] = '#';
				
				printf(RED "%c" COLOR_RESET, 219);
			}
		}
		
		for(int i = lowestPrice; i<closePrice; i+=10) {
			coords = i / 10;
			if(coords > 0 && coords < 34) {
				
				
				
				gotoxy(col, arr[coords]);
				map[arr[coords]][col] = '#';
				
				printf(RED "%c" COLOR_RESET, 179);
			}
		}
			
	} else {
		int coords;
		for(int i = closePrice+1; i<=highestPrice; i+=10) {
			coords = i / 10;
			if(coords > 0 && coords < 34) {
				
				
				gotoxy(col, arr[coords]);
				map[arr[coords]][col] = '#';
				printf(GRN "%c" COLOR_RESET, 179);
			}
		}
		for(int i = openPrice; i<=closePrice; i+=10) {
			coords = i / 10;
			
			if(coords > 0 && coords < 34) {
				
				
				gotoxy(col, arr[coords]);
				map[arr[coords]][col] = '#';
				printf(GRN "%c" COLOR_RESET, 219);
			}
		}
		
		for(int i = lowestPrice; i<openPrice; i+=10) {
			coords = i / 10;
			if(coords > 0 && coords < 34) {
			
				
				gotoxy(col, arr[coords]);
				map[arr[coords]][col] = '#';
				printf(GRN "%c" COLOR_RESET, 179);
			}
		}
	}
}

int col = 1;

void printAll() {
	struct Candle* curr = head;
	while(curr != NULL) {
		set(curr->openPrice, curr->closePrice, curr->highestPrice, curr->lowestPrice, col);
		
		curr = curr->next;
		col += 1;
	}
}

void emptyCol(int col) {
	for(int i = 1; i<=33; i++) {
		gotoxy(col, arr[i]);
		if(map[arr[i]][col] != '-') {
			map[arr[i]][col] = '-';
			printf("-");
		}
		
		
	}
}

void updatePrint(int isNext) {
	struct Candle* curr = currStart;
	int startCol = 1;
	while(curr != NULL) {
		emptyCol(startCol);
		set(curr->openPrice, curr->closePrice, curr->highestPrice, curr->lowestPrice, startCol);
		
		curr = curr->next;
		startCol += 1;
	}
	if(isNext == 1) {
		emptyCol(startCol);
	}
}

void printMap() {
	for(int i = 0; i<35; i++) {
		for(int j = 0; j<131; j++) {
			printf("%c", map[i][j]);	
		}
		puts("");
	}
}

void starting(int n, int isStart) {
	int openPrice, closePrice, highestPrice, lowestPrice, diff;
	int i = 0, random;
	if(isStart == 1) {
		
		random = rand() % 33;
		openPrice = (random + 1)*10; 
	} else {
		openPrice = tail->closePrice;
	}
	
	while(i<n) {
		//closePrice
		random = rand() % 10;
		if(random == 0) {
			closePrice = openPrice;
		} else {
			
			random = rand() % 6;
			diff = (random+1)*10;
			random = rand() % 2;
			if(random == 0) {
				diff *= -1;
			}
			closePrice = openPrice + diff;  
		}
		
//		if(closePrice < 10) {
//			closePrice = 10;
//		} else if(closePrice > 330) {
//			closePrice = 330;
//		}
		
		//highestPrice
		if(closePrice > openPrice) {
			random = rand() % 10;
			if(random < 3) {
				highestPrice = closePrice;
			} else {
				random = rand()%5;
				diff = random * 10;
				highestPrice = diff + closePrice;
			} 
		} else {
			random = rand() % 10;
			if(random < 3) {
				highestPrice = openPrice;
			} else {
				random = rand()%5;
				diff = random * 10;
				highestPrice = diff + openPrice;
			} 
		}
		
//		if(highestPrice < 10) {
//			highestPrice = 10;
//		} else if(highestPrice > 330) {
//			highestPrice = 330;
//		}
		
		//lowestPrice
		if(closePrice > openPrice) {
			random = rand() % 10;
			if(random < 3) {
				lowestPrice = openPrice;
			} else {
				random = rand()%5;
				diff = random * 10;
				lowestPrice = openPrice -diff;
			} 
		} else {
			random = rand() % 10;
			if(random < 3) {
				lowestPrice = closePrice;
			} else {
				random = rand()%5;
				diff = random * 10;
				lowestPrice = closePrice -diff;
			} 
		}
//		if(lowestPrice < 10) {
//			lowestPrice = 10;
//		} else if(lowestPrice > 330) {
//			lowestPrice = 330;
//		}
		pushTail(openPrice, closePrice, highestPrice, lowestPrice);
		openPrice = closePrice;
		i++;	
	}
	
}

void menuTrade() {
	printf("Welcome To METAFI	| Balance	: %d\n", currPerson->balance);
	puts("1. New Day		| Value 	: ");
	puts("2. Previous		| Position 	: ");
	puts("3. Next			| TP 		: ");
	puts("4. Long			| SL 		: ");
	puts("5. Short		| ");
	printf("6. Logout >> ");
}

void colorLine(int row, char* color) {
	for(int i = 1; i<123; i++) {
		gotoxy(i, row);
		if(map[row][i] == '#' || map[row][i] == '#') {
			
			
			continue;
		}
		else if(strcmp(color, "green") == 0) {
			printf(GRN "-" COLOR_RESET);	
		}
		else if(strcmp(color, "yellow") == 0) {
			printf(YEL "-" COLOR_RESET);	
		}
		else if(strcmp(color, "red") == 0) {
			printf(RED "-" COLOR_RESET);	
		}
		else if(strcmp(color, "white") == 0) {
			printf(COLOR_RESET "-" COLOR_RESET);	
		}
		usleep(1000);
	}
}

void guide() {
	system("cls");
	puts("What Is Forex?");
	puts("\"Forex\", short for foreign exchange, is a global decentralized marketplace for trading currencies.");
	puts("In forex trading, participants buy one currency by selling another currency simultaneously, with the aim of profiting from fluctuations in exchange rates.");
	puts("It is one of the largest and most liquid financial markets in the world, operating 24 hours a day, five days a week.");
	puts("");
	puts("What Is Candle Stick?");
	puts("\"Canlde Stick\" in charts are a type of financial chart used to represent price movements in trading markets, including forex.");
	puts("They visually display the open, high, low, and close prices for a specific period.");
	puts("if the close price is higher than the open price, the candle color will be green");
	puts("then if the close price is less than the open price, the candle color will be red.");
	puts("shadow or tick is the highest and the lowest price of a candle.");
	puts("");
	puts("What Is Position?");
	puts("\"Position\" in forex trading refers to where we want to enter the market at certain price point");
	puts("");
	puts("What Is Long?");
	puts("\"Long\" in forex trading refers to a trading position where a trader buys a currency pair");
	puts("with the expectation that its value will increase over time.");
	puts("");
	puts("What Is Short?");
	puts("\"Short\" in forex trading refers to a trading position where a trader sells a currency pair");
	puts("with the expectation that its value will decrease over time.");
	puts("");
	puts("What is Take Profit?");
	puts("\"Take Profit\" in forex trading refers to where we will leave the market at the specified price point");
	puts("our position will automatically closed after the market price hit our take profit price");
	puts("");
	puts("What Is Stop Loss?");
	puts("\"Stop Loss\" in forex trading refers to where we will leave the market at the specified price point");
	puts("it's used to make sure that we don't loss all of our money and many more");
	puts("same like take profit, our position will automatically closed after the market price hit our take profit price");
	puts("");
	printf("Press Enter To Continue..."); getchar();

}

int main() {
	fullscreen();
	setConversion();

	srand(time(NULL));
	
	//login register
	starting(20, 1);
	int inputLoginRegisterMenu, loggedIn = 0, inputUserMenu;
	int input, value, position, TP, SL, error = 0, tradeOpen = 0, tradeActive = 0, reward, isLong = 0, isShort = 0;
	char color[10];
	do {
		loggedIn = 0;
		system("cls");
		startMenu();
		scanf("%d", &inputLoginRegisterMenu); getchar();
		switch(inputLoginRegisterMenu) {
			case 1: {
				loggedIn = login();
				break;
			}
			case 2: {
				registerAcc();
				break;
			}
		}
		//login register
		if(loggedIn == 1) {
			//user menu
			do {
				system("cls");
				userMenu();
				scanf("%d", &inputUserMenu); getchar();
				switch(inputUserMenu) {
					case 1: {
						//start trade
						break;
					}
					case 2: {
						//trade history
						system("cls");
						printTradeHistory(&currPerson->inner_list);
						printf("Press Enter To Continue..."); getchar();
						break;
					}
					case 3: {
						//guide
						guide();
						break;
					}
				}
			}while(inputUserMenu != 4 && inputUserMenu != 1);
			if(inputUserMenu == 1) {
				
			system("cls");
			printMap();
			//since i marked it, might have to unmarked it if i logged out login again
			if(head != NULL) {
				currStart = head;
			}
			col = 1;
			printAll();
			error = 0; 
			//if past trade
			if(currPerson->tradeOpen > 0) {
				strcpy(color, "green");
				colorLine(arr[currPerson->currTP/10], color);
				strcpy(color, "red");
				colorLine(arr[currPerson->currSL/10], color);
				strcpy(color, "yellow");
				colorLine(arr[currPerson->currPosition/10], color);
				SL = currPerson->currSL;
				TP = currPerson->currTP;
				position = currPerson->currPosition;
				value = currPerson->currValue;
				tradeOpen = currPerson->tradeOpen;
				tradeActive = currPerson->tradeActive;
				isShort = currPerson->isShort;
				isLong = currPerson->isLong;
			}

			do {
				

				
				if(tradeOpen == 2) {
					
					if(position == tail->closePrice) {
						tradeActive = 1;
						currPerson->tradeActive = 1;
					}
				}
				
				if(tradeActive == 1) {
					
					if(isLong == 1) {
						
						if(tail->highestPrice >= TP || tail->lowestPrice <= SL) {
							tradeActive = 0;
							tradeOpen = 0;
							reward = (value * abs(tail->closePrice -position)) /100;
							isLong = 0;
							currPerson->tradeOpen = 0;
							currPerson->tradeActive = 0;
							currPerson->isLong = 0;
							if(tail->lowestPrice <= SL) {
								reward *= -1;
							}
							
						} 
					} else if (isShort == 1) {
						if(tail->highestPrice >= SL || tail->lowestPrice <= TP) {
							tradeActive = 0;
							tradeOpen = 0;
							reward = (value * abs(tail->closePrice -position)) /100;
							isShort = 0;
							currPerson->tradeOpen = 0;
							currPerson->tradeActive = 0;
							currPerson->isShort = 0;
							if(tail->highestPrice >= SL) {
								reward *= -1;
							}
						} 
					}
					strcpy(color, "white");
					colorLine(arr[TP/10], color);
					strcpy(color, "white");
					colorLine(arr[SL/10], color);
					strcpy(color, "white");
					colorLine(arr[position/10], color);
					pushMidTH(&currPerson->inner_list, value, position, TP, SL, reward);
					currPerson->balance += reward;
					gotoxy(42,35);
					printf("               ");
					gotoxy(42,35);
					printf("%d", currPerson->balance);

					//now it doesnt detect tp and sl
					//will remove the colored lines
				}
				
				gotoxy(0,35);
				menuTrade();
				gotoxy(13, 41);
				scanf("%d", &input); getchar();
				switch(input) {
					case 1: {
						if(tradeOpen == 0) {
							break;
						}
						tradeOpen = 2;
						currPerson->tradeOpen = 2;
						starting(1, 0);
						set(tail->openPrice, tail->closePrice, tail->highestPrice, tail->lowestPrice, col);
						
						//problem: random openPrice;
						col += 1;
						
						break;
					}
					case 2: {
						//previous
						if(currStart->prev != NULL) {
							currStart = currStart->prev;
							updatePrint(0);	
						}
						
						break;
					}
					case 3: {
						//next
						if(currStart->next != NULL) {
							
							currStart = currStart->next;
							updatePrint(1);	
						}
						
						break;
					}
					case 4: {
						if(currPerson->tradeOpen > 0) {
							break;
						}
						//long
						do {
							if(error == 1) {
								gotoxy(26, 40);
								printf("must between 10 and user's balance");
							}
							gotoxy(42, 36);
							scanf("%d", &value); getchar();
							error = 1;
							gotoxy(26, 40);
							printf("                            ");
						}while(value < 10 || value > currPerson->balance);
						//prompt error
						do {
							if(error == 1) {
								gotoxy(26, 40);
								printf("must be between 20-330 and multiple of ten");
							}
							gotoxy(42, 37);
							scanf("%d", &position); getchar();
							error = 1;
							gotoxy(26, 40);
							printf("                            ");
						}while(position < 20 || position > 330 || position % 10 != 0);
						error = 0;
						do {
							if(error == 1) {
								gotoxy(26, 40);
								printf("must be higher than position");
							}
							gotoxy(42, 38);
							scanf("%d", &TP); getchar();
							error = 1;
							gotoxy(26, 40);
							printf("                            ");
						}while(TP < position || TP % 10 != 0 || TP < 10 || TP > 330);
						error = 0;
						do {
							if(error == 1) {
								gotoxy(26, 40);
								printf("must be lower than position");
							}
							gotoxy(42, 39);
							scanf("%d", &SL); getchar();
							error = 1;
							gotoxy(26, 40);
							printf("                            ");
						} while(SL > position || SL % 10 != 0 || SL < 10 || SL > 330);
						error = 0;
						strcpy(color, "green");
						colorLine(arr[TP/10], color);
						strcpy(color, "red");
						colorLine(arr[SL/10], color);
						strcpy(color, "yellow");
						colorLine(arr[position/10], color);
						tradeOpen = 1;
						currPerson->tradeOpen = 1;
						currPerson->currPosition = position;
						currPerson->currSL = SL;
						currPerson->currTP = TP;
						currPerson->currValue = value;
						isLong = 1;
						currPerson->isLong = 1;
						
						break;
					}
					case 5: {
						//short
						//prompt error
						if(currPerson->tradeOpen > 0) {
							break;
						}
						do {
							if(error == 1) {
								gotoxy(26, 40);
								printf("must between 10 and user's balance");
							}
							gotoxy(42, 36);
							scanf("%d", &value); getchar();
							error = 1;
							gotoxy(26, 40);
							printf("                            ");
						}while(value < 10 || value > currPerson->balance);
						
						do {
							if(error == 1) {
								gotoxy(26, 40);
								printf("must be between 20-330 and multiple of ten");
							}
							gotoxy(42, 37);
							scanf("%d", &position); getchar();
							error = 1;
							gotoxy(26, 40);
							printf("                            ");
						}while(position < 20 || position > 330 || position % 10 != 0);
						error = 0;
						do {
							if(error == 1) {
								gotoxy(26, 40);
								printf("must be lower than position");
							}
							gotoxy(42, 38);
							scanf("%d", &TP); getchar();
							error = 1;
							gotoxy(26, 40);
							printf("                            ");
						}while(TP > position || TP % 10 != 0 || TP < 10 || TP > 330);
						error = 0;
						do {
							if(error == 1) {
								gotoxy(26, 40);
								printf("must be higher than position");
							}
							gotoxy(42, 39);
							scanf("%d", &SL); getchar();
							error = 1;
							gotoxy(26, 40);
							printf("                            ");
						} while(SL < position || SL % 10 != 0 || SL < 10 || SL > 330);
						error = 0;
						strcpy(color, "green");
						colorLine(arr[TP/10], color);
						strcpy(color, "red");
						colorLine(arr[SL/10], color);
						strcpy(color, "yellow");
						colorLine(arr[position/10], color);
						tradeOpen = 1;
						currPerson->inTrade = 1;
						currPerson->currPosition = position;
						currPerson->currSL = SL;
						currPerson->currTP = TP;
						currPerson->currValue = value;
						isShort = 1;
						currPerson->isShort = 1;
						break;
					}
				}
			}while(input != 6);
			}
		}
		
		
		
	} while(inputLoginRegisterMenu != 3);
	system("cls");
	puts("Thank You For Using This Program!");

	
	return 0;
}

