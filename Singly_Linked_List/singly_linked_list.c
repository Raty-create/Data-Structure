#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>

#define SIZE 100
#define NAMELIMIT 52
#define POSLIMIT 22
#define COUNT(arr) (sizeof(arr) / sizeof(arr[0]))

typedef struct {
	char name[NAMELIMIT];
	int hp;
	int mp;
	int speed;
	int range;
	char position[POSLIMIT];
} LOL;

typedef LOL element;

typedef struct ListNode {
	element data;
	struct ListNode* link;
} ListNode;

typedef struct {
	ListNode* head;
	ListNode* tail;
} List;

List Array2SLinkedList(LOL arr[], int length) {
	List list = { NULL,NULL };

	if (length == 0) return list;

	ListNode* node = (ListNode*)malloc(sizeof(ListNode));
	node->data = arr[0];

	list.head = list.tail = node;

	for (int i = 1; i < length; i++) {
		node = (ListNode*)malloc(sizeof(ListNode));
		node->data = arr[i];

		list.tail->link = node;
		list.tail = node;
	}

	list.tail->link = list.head;

	return list;
}

void PrintNode(ListNode* node) {
	if (node == NULL) return;

	printf("Name: %s\n", node->data.name);
	printf("Hp: %d\n", node->data.hp);
	printf("Mp: %d\n", node->data.mp);
	printf("Speed: %d\n", node->data.speed);
	printf("Range: %d\n", node->data.range);
	printf("Position: %s\n", node->data.position);
}

void FilePrint(FILE* fp, List list) {
	if (list.head != NULL) {
		ListNode* save = list.head;

		do {
			fprintf(fp, "%s\t%d\t%d\t%d\t%d\t%s\n", save->data.name, save->data.hp, save->data.mp, save->data.speed, save->data.range, save->data.position);
			save = save->link;
		} while (save != list.head);;
	}
}

void FreeAll_SL(List list) {
	if (list.head == NULL) return;

	ListNode* p, * start, * next;
	p = start = list.head;

	do {
		next = p->link;
		free(p);
		p = next;
	} while (p != start);
}

ListNode* SearchName_SL(List list, const char* name) {
	if (list.head == NULL) return NULL;

	ListNode* p = list.head;

	do {
		if (strcmp(p->data.name, name) == 0)
			return p;
		else
			p = p->link;
	} while (p != list.head);

	return NULL;
}

ListNode* SearchPos_SL(List list, const char* position) {
	if (list.head == NULL) return NULL;

	ListNode* p = list.head;

	do {
		if (strcmp(p->data.position, position) == 0)
			return p;
		else
			p = p->link;
	} while (p != list.head);

	return NULL;
}

List Insert_SL(List list, LOL value) {
	ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
	newNode->data = value;

	if (list.head == NULL) {
		list.head = list.tail = newNode;
		newNode->link = newNode;
		return list;
	}

	if (newNode->data.hp >= list.head->data.hp) {
		newNode->link = list.head;
		list.head = newNode;
		list.tail->link = newNode;

		return list;
	}

	ListNode* currentNode = list.head;
	ListNode* prev = list.tail;
	do {
		if (prev->data.hp >= newNode->data.hp && currentNode->data.hp <= newNode->data.hp) {
			newNode->link = currentNode;
			prev->link = newNode;

			return list;
		}
		else {
			prev = prev->link;
			currentNode = currentNode->link;
		}
	} while (currentNode != list.head);

	if (newNode->data.hp <= list.tail->data.hp) {
		list.tail->link = newNode;
		list.tail = newNode;
		newNode->link = list.head;

		return list;
	}

	return list;
}

List Delete_SL(List list, const char* name) {
	if (list.head == NULL) return list;

	ListNode* p, * prev;
	p = list.head;
	prev = list.tail;

	do {
		if (strcmp(p->data.name, name) == 0) {
			if (p == list.head && p == list.tail) {
				list.head = list.tail = NULL;
				free(p);

				return list;
			}
			else if (p == list.head) {
				list.head = p->link;
				prev->link = p->link;
			}
			else if (p == list.tail) {
				list.tail = prev;
				prev->link = p->link;
			}
			else
				prev->link = p->link;

			free(p);

			break;
		}
		else {
			prev = p;
			p = p->link;
		}
	} while (p != list.head);

	return list;
}

List DeleteAll_SL(List list, const char* position) {
	if (list.head == NULL) return list;

	int nodeCnt = 0;
	ListNode* temp = list.head;

	do {
		nodeCnt++;
		temp = temp->link;
	} while (temp != list.head);

	ListNode* p, * prev, * next;
	p = list.head;
	prev = list.tail;

	for (int i = 0; i < nodeCnt; i++) {
		next = p->link;

		if (strcmp(p->data.position, position) == 0) {
			if (p == list.head && p == list.tail) {
				list.head = list.tail = NULL;
				free(p);

				return list;
			}
			else if (p == list.head) {
				list.head = next;
				prev->link = next;
			}
			else if (p == list.tail) {
				list.tail = prev;
				prev->link = next;
			}
			else
				prev->link = next;

			free(p);
			p = next;

			nodeCnt--;
			i--;
		}
		else {
			prev = p;
			p = next;
		}
	}

	return list;
}

void PrintAll_SL(List list) {
	if (list.head == NULL) return;

	ListNode* print = list.head;

	do {
		printf("%s\t%d\t%d\t%d\t%d\t%s\n", print->data.name, print->data.hp, print->data.mp, print->data.speed, print->data.range, print->data.position);
		print = print->link;
	} while (print != list.head);
}

ListNode* FindMaxHp_SL(List list) {
	if (list.head == NULL) return NULL;

	ListNode* p, * temp;
	p = list.head;
	temp = p;

	int currentMaxHp = list.head->data.hp;

	do {
		if (p->data.hp > currentMaxHp) {
			currentMaxHp = p->data.hp;
			temp = p;

			p = p->link;
		}
		else
			p = p->link;
	} while (p != list.head);

	return temp;
}

List SortByHp_SL(List list) {
	if (list.head == NULL) return list;

	int nodeCnt = 0;
	ListNode* temp = list.head;

	do {
		nodeCnt++;
		temp = temp->link;
	} while (temp != list.head);

	for (int i = 0; i < nodeCnt; i++) {
		ListNode* p, * prev, * next;
		p = list.head;
		prev = NULL;
		next = p->link;

		for (int j = 0; j < nodeCnt - 1 - i; j++) {
			if (p->data.hp < next->data.hp) {
				ListNode* nextNext = next->link;

				if (prev == NULL)
					list.head = next;
				else
					prev->link = next;

				next->link = p;
				p->link = nextNext;

				if (p->link == list.head)
					list.tail = p;

				list.tail->link = list.head;

				prev = next;
				next = p->link;
			}
			else {
				prev = p;
				p = next;
				next = p->link;
			}
		}
	}

	return list;
}

int main() {
	LOL champ[SIZE];
	int champCnt = 0;
	char fileLine[SIZE];

	FILE* fp = fopen("testdata.txt", "r");

	if (fp == NULL) {
		printf("파일 열기 실패");
		return 0;
	}

	while (fgets(fileLine, sizeof(fileLine), fp) != NULL) {
		if (sscanf(fileLine, "%51[^\t]\t%d\t%d\t%d\t%d\t%21[^\n]", champ[champCnt].name, &champ[champCnt].hp, &champ[champCnt].mp, &champ[champCnt].speed, &champ[champCnt].range, champ[champCnt].position) == 6)
			champCnt++;
	}

	fclose(fp);

	List champLinkedList = Array2SLinkedList(champ, champCnt);

	while (1) {
		system("cls");

		int menuNum;
		char notMenuNum;
		char inputMenu[SIZE];

		char* printMenu[] = { "Exit","Search","Insert","Delete","DeleteAll","PrintAll","FindMaxHp","SortByHp" };

		for (int i = 0; i < COUNT(printMenu); i++)
			printf("%d. %s\n", i, printMenu[i]);
		printf("원하시는 옵션의 숫자를 입력해주세요: ");

		if (fgets(inputMenu, sizeof(inputMenu), stdin) == NULL)
			continue;

		if (sscanf(inputMenu, "%d %c", &menuNum, &notMenuNum) != 1 || menuNum < 0 || menuNum > COUNT(printMenu)) {
			printf("잘못된 입력입니다. 옵션 내의 숫자 한 개만 입력해주세요.");
			Sleep(3000);
			continue;
		}

		if (menuNum == 0) {
			FreeAll_SL(champLinkedList);
			printf("\n프로그램을 종료합니다.\n");
			return 0;
		}

		switch (menuNum) {
		case 1: {
			if (champLinkedList.head == NULL) {
				printf("파일에 챔피언이 없습니다.");
				Sleep(3000);
				break;
			}

			char searchName[NAMELIMIT];
			char inputName[NAMELIMIT];

			printf("정보를 확인할 챔피언의 닉네임을 입력해주세요: ");

			fgets(inputName, sizeof(inputName), stdin);
			if (sscanf(inputName, "%51[^\n]", searchName) != 1) {
				printf("잘못된 입력입니다. 닉네임을 입력해주세요.");
				Sleep(3000);
				break;
			}

			ListNode* found = SearchName_SL(champLinkedList, searchName);

			if (found == NULL) {
				printf("없는 챔피언입니다.");
				Sleep(3000);
				break;
			}
			else {
				PrintNode(found);
				system("pause");
			}

			break;
		}
		case 2: {
			LOL newChamp;

			printf("추가할 챔피언의 정보를 입력해주세요.\n");

			printf("Name: ");

			char insertName[NAMELIMIT];

			fgets(insertName, sizeof(insertName), stdin);

			if (strchr(insertName, '\n') == NULL) {
				printf("닉네임은 최대 50글자까지 가능합니다.");
				Sleep(3000);
				while (getchar() != '\n');
				break;
			}

			if (sscanf(insertName, "%51[^\n]", newChamp.name) != 1) {
				printf("잘못된 입력입니다. 닉네임을 입력해주세요.");
				Sleep(3000);
				break;
			}

			ListNode* found = SearchName_SL(champLinkedList, newChamp.name);

			if (found) {
				printf("사용 중인 닉네임입니다.");
				Sleep(3000);
				break;
			}

			printf("Hp: ");

			char insertHp[SIZE];
			char notNewHp;

			fgets(insertHp, sizeof(insertHp), stdin);
			if (sscanf(insertHp, "%d %c", &newChamp.hp, &notNewHp) != 1 || newChamp.hp < 1) {
				printf("잘못된 입력입니다. 1 이상인 하나의 숫자를 입력해주세요.");
				Sleep(3000);
				break;
			}

			printf("Mp: ");

			char insertMp[SIZE];
			char notNewMp;

			fgets(insertMp, sizeof(insertMp), stdin);
			if (sscanf(insertMp, "%d %c", &newChamp.mp, &notNewMp) != 1 || newChamp.mp < 1) {
				printf("잘못된 입력입니다. 1 이상인 하나의 숫자를 입력해주세요.");
				Sleep(3000);
				break;
			}

			printf("Speed: ");

			char insertSpeed[SIZE];
			char notNewSpeed;

			fgets(insertSpeed, sizeof(insertSpeed), stdin);
			if (sscanf(insertSpeed, "%d %c", &newChamp.speed, &notNewSpeed) != 1 || newChamp.speed < 1) {
				printf("잘못된 입력입니다. 1 이상인 하나의 숫자를 입력해주세요.");
				Sleep(3000);
				break;
			}

			printf("Range: ");

			char insertRange[SIZE];
			char notNewRange;

			fgets(insertRange, sizeof(insertRange), stdin);
			if (sscanf(insertRange, "%d %c", &newChamp.range, &notNewRange) != 1 || newChamp.range < 1) {
				printf("잘못된 입력입니다. 1 이상인 하나의 숫자를 입력해주세요.");
				Sleep(3000);
				break;
			}

			printf("Position: ");

			char insertPos[POSLIMIT];

			fgets(insertPos, sizeof(insertPos), stdin);
			if (sscanf(insertPos, "%21[^\n]", newChamp.position) != 1 ||
				strcmp(newChamp.position, "top") != 0 &&
				strcmp(newChamp.position, "jungle") != 0 &&
				strcmp(newChamp.position, "mid") != 0 &&
				strcmp(newChamp.position, "bottom") != 0 &&
				strcmp(newChamp.position, "support") != 0) {
				printf("잘못된 입력입니다. top, jungle, mid, bottom, support 중 하나만 입력해주세요.");
				Sleep(3000);
				break;
			}

			champLinkedList = Insert_SL(champLinkedList, newChamp);

			FILE* fp2 = fopen("testdata.txt", "w");

			if (fp2 == NULL) {
				printf("파일 열기 실패");
				Sleep(3000);
				break;
			}

			ListNode* save = champLinkedList.head;

			do {
				fprintf(fp2, "%s\t%d\t%d\t%d\t%d\t%s\n", save->data.name, save->data.hp, save->data.mp, save->data.speed, save->data.range, save->data.position);
				save = save->link;
			} while (save != champLinkedList.head);

			fclose(fp2);

			printf("추가 완료!");
			Sleep(3000);

			break;
		}
		case 3: {
			if (champLinkedList.head == NULL) {
				printf("파일에 챔피언이 없습니다.");
				Sleep(3000);
				break;
			}

			PrintAll_SL(champLinkedList);

			char nameDelChamp[NAMELIMIT];
			char inputNameDel[NAMELIMIT];

			printf("삭제할 챔피언의 닉네임을 입력해주세요: ");

			fgets(inputNameDel, sizeof(inputNameDel), stdin);
			if (sscanf(inputNameDel, "%51[^\n]", nameDelChamp) != 1) {
				printf("잘못된 입력입니다. 닉네임을 입력해주세요.");
				Sleep(3000);
				break;
			}

			ListNode* found = SearchName_SL(champLinkedList, nameDelChamp);

			if (found == NULL) {
				printf("없는 챔피언입니다.");
				Sleep(3000);
				break;
			}
			else {
				champLinkedList = Delete_SL(champLinkedList, nameDelChamp);

				if (champLinkedList.head == NULL) {
					printf("모든 챔피언이 삭제되었습니다.");
					Sleep(3000);
				}
				else {
					printf("해당 챔피언 삭제 완료!");
					Sleep(3000);
				}

				FILE* fp3 = fopen("testdata.txt", "w");

				FilePrint(fp3, champLinkedList);
				fclose(fp3);
			}

			break;
		}
		case 4: {
			if (champLinkedList.head == NULL) {
				printf("파일에 챔피언이 없습니다.");
				Sleep(3000);
				break;
			}

			PrintAll_SL(champLinkedList);

			char posDelChamp[NAMELIMIT];
			char inputposDel[NAMELIMIT];

			printf("삭제할 포지션을 입력해주세요: ");

			fgets(inputposDel, sizeof(inputposDel), stdin);
			if (sscanf(inputposDel, "%51[^\n]", posDelChamp) != 1) {
				printf("잘못된 입력입니다. 삭제할 포지션을 입력해주세요.");
				Sleep(3000);
				break;
			}

			ListNode* found = SearchPos_SL(champLinkedList, posDelChamp);

			if (found == NULL) {
				printf("없는 포지션입니다.");
				Sleep(3000);
				break;
			}
			else {
				champLinkedList = DeleteAll_SL(champLinkedList, posDelChamp);

				if (champLinkedList.head == NULL) {
					printf("모든 챔피언이 삭제되었습니다.");
					Sleep(3000);
				}
				else {
					printf("해당 포지션인 챔피언 모두 삭제 완료!");
					Sleep(3000);
				}

				FILE* fp4 = fopen("testdata.txt", "w");

				FilePrint(fp4, champLinkedList);
				fclose(fp4);
			}

			break;
		}
		case 5: {
			if (champLinkedList.head == NULL) {
				printf("파일에 챔피언이 없습니다.");
				Sleep(3000);
				break;
			}

			PrintAll_SL(champLinkedList);
			system("pause");

			break;
		}
		case 6: {
			if (champLinkedList.head == NULL) {
				printf("파일에 챔피언이 없습니다.");
				Sleep(3000);
				break;
			}

			ListNode* maxHpChamp = FindMaxHp_SL(champLinkedList);

			ListNode* sameMaxHp = champLinkedList.head;

			do {
				if (sameMaxHp->data.hp == maxHpChamp->data.hp) {
					PrintNode(sameMaxHp);
					printf("\n");
					sameMaxHp = sameMaxHp->link;
				}
				else
					sameMaxHp = sameMaxHp->link;
			} while (sameMaxHp != champLinkedList.head);

			system("pause");

			break;
		}
		case 7: {
			if (champLinkedList.head == NULL) {
				printf("파일에 챔피언이 없습니다.");
				Sleep(3000);
				break;
			}

			champLinkedList = SortByHp_SL(champLinkedList);

			PrintAll_SL(champLinkedList);
			system("pause");

			FILE* fp5 = fopen("testdata.txt", "w");

			if (fp5 == NULL) {
				printf("파일 열기 실패");
				Sleep(3000);
				break;
			}

			FilePrint(fp5, champLinkedList);
			fclose(fp5);

			break;
		}
		}
	}

	return 0;
}