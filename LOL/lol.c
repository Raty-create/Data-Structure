#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <string.h>

#define SIZE 100

typedef struct {
	char name[50];
	int hp;
	int mp;
	int speed;
	int range;
	char position[20];
} LOL;

int main() {
	LOL champ[SIZE];
	int champCount = 0;
	char line[SIZE];

	FILE* fp = fopen("testdata.txt", "r");

	if (fp == NULL) {
		printf("파일 열기 실패");
		return 0;
	}

	while (fgets(line,  sizeof(line), fp) != NULL) {
		if (sscanf(line, "%49[^\t]\t%d\t%d\t%d\t%d\t%s", champ[champCount].name, &champ[champCount].hp, &champ[champCount].mp, &champ[champCount].speed, &champ[champCount].range, champ[champCount].position) == 6)
			champCount++;
	}

	fclose(fp);

	while (1) {
		system("cls");

		int num;

		printf("1. Search\n");
		printf("2. Insert\n");
		printf("3. Delete\n");
		printf("4. DeleteAll\n");
		printf("5. PrintAll\n");
		printf("6. FindMaxHp\n");
		printf("7. SortByHp\n");
		printf("0. Exit\n");
		printf("원하시는 옵션의 숫자를 입력해주세요: ");
		if (scanf(" %d", &num) != 1) {
			printf("숫자를 입력해주세요.");
			Sleep(3000);
			while (getchar() != '\n');
			continue;
		}

		if (num == 0) {
			printf("\n프로그램 종료\n");
			return 0;
		}

		if (num < 0 || num > 7) {
			printf("존재하지 않는 옵션입니다.(0~7)");
			Sleep(3000);
			while (getchar() != '\n');
			continue;
		}

		switch (num) {
		case 1: {
			char champName[50];
			int found = 0;

			printf("정보가 궁금한 챔피언의 닉네임을 입력해주세요: ");
			scanf(" %49[^\n]", champName);
			for (int i = 0; i < champCount; i++) {
				if (strcmp(champName, champ[i].name) == 0) {
					found = 1;

					printf("\nname: %s\n", champ[i].name);
					printf("hp: %d\n", champ[i].hp);
					printf("mp: %d\n", champ[i].mp);
					printf("speed: %d\n", champ[i].speed);
					printf("range: %d\n", champ[i].range);
					printf("position: %s\n", champ[i].position);

					system("pause");

					break;
				}
			}

			if (!found) {
				printf("존재하지 않는 챔피언입니다.");
				Sleep(3000);
				while (getchar() != '\n');
				break;
			}
			
			break;
		}
		case 2: {
			LOL newChamp;
			int found = 0;

			printf("추가할 챔피언 정보 입력\n");
			
			printf("name: ");
			scanf(" %49[^\n]", newChamp.name);
			for (int i = 0; i < champCount; i++) {
				if (strcmp(newChamp.name, champ[i].name) == 0) {
					found = 1;

					printf("이미 존재하는 챔피언 닉네임입니다.");
					Sleep(3000);
					while (getchar() != '\n');
					break;
				}
			}

			if (found == 1) {
				break;
			}

			printf("hp: ");
			if (scanf(" %d", &newChamp.hp) != 1) {
				printf("숫자를 입력해주세요.");
				Sleep(3000);
				while (getchar() != '\n');
				continue;
			}

			printf("mp: ");
			if (scanf(" %d", &newChamp.mp) != 1) {
				printf("숫자를 입력해주세요.");
				Sleep(3000);
				while (getchar() != '\n');
				continue;
			}

			printf("speed: ");
			if (scanf(" %d", &newChamp.speed) != 1) {
				printf("숫자를 입력해주세요.");
				Sleep(3000);
				while (getchar() != '\n');
				continue;
			}

			printf("range: ");
			if (scanf(" %d", &newChamp.range) != 1) {
				printf("숫자를 입력해주세요.");
				Sleep(3000);
				while (getchar() != '\n');
				continue;
			}

			printf("position: ");
			scanf(" %s", newChamp.position);
			if (strcmp(newChamp.position, "top") != 0 &&
				strcmp(newChamp.position, "jungle") != 0 &&
				strcmp(newChamp.position, "mid") != 0 &&
				strcmp(newChamp.position, "bottom") != 0 &&
				strcmp(newChamp.position, "support") != 0) {
				printf("해당 포지션은 존재하지 않는 포지션입니다.(top, jungle, mid, bottom, support)");
				Sleep(3000);
				while (getchar() != '\n');
				continue;
			}

			champ[champCount] = newChamp;

			if (champCount >= SIZE) {
				printf("더 이상 추가할 수 없습니다.");
				Sleep(3000);
				while (getchar() != '\n');
				break;
			}
			else {
				champCount++;
			}

			FILE* fp2 = fopen("testdata.txt", "a");

			if (fp2 == NULL) {
				printf("파일 열기 실패");
				Sleep(3000);
				break;
			}

			fprintf(fp2, "\n%s\t%d\t%d\t%d\t%d\t%s", newChamp.name, newChamp.hp, newChamp.mp, newChamp.speed, newChamp.range, newChamp.position);

			fclose(fp2);

			printf("추가 완료!");
			Sleep(3000);

			break;
		}
		case 3: {
			char NameDelChamp[50];
			int found = 0;

			for (int r = 0; r < champCount; r++) {
				printf("%s\t%d\t%d\t%d\t%d\t%s\n", champ[r].name, champ[r].hp, champ[r].mp, champ[r].speed, champ[r].range, champ[r].position);
			}

			printf("삭제할 챔피언의 닉네임을 입력해주세요: ");
			scanf(" %49[^\n]", NameDelChamp);

			for (int i = 0; i < champCount; i++) {
				if (strcmp(NameDelChamp, champ[i].name) == 0) {
					found = 1;

					champ[i] = champ[champCount - 1];
					champCount--;

					break;
				}
			}

			if (!found) {
				printf("존재하지 않는 챔피언입니다.");
				Sleep(3000);
				while (getchar() != '\n');
				break;
			}

			FILE* fp3 = fopen("testdata.txt", "w");

			if (fp3 == NULL) {
				printf("파일 열기 실패");
				Sleep(3000);
				break;
			}

			for (int j = 0; j < champCount; j++) {
				if (j == champCount - 1) {
					fprintf(fp3, "%s\t%d\t%d\t%d\t%d\t%s", champ[j].name, champ[j].hp, champ[j].mp, champ[j].speed, champ[j].range, champ[j].position);
				}
				else {
					fprintf(fp3, "%s\t%d\t%d\t%d\t%d\t%s\n", champ[j].name, champ[j].hp, champ[j].mp, champ[j].speed, champ[j].range, champ[j].position);
				}
			}

			fclose(fp3);

			printf("삭제 완료!");
			Sleep(3000);

			break;
		}
		case 4: {
			char PosDelChamp[20];
			int found = 0;

			for (int r = 0; r < champCount; r++) {
				printf("%s\t%d\t%d\t%d\t%d\t%s\n", champ[r].name, champ[r].hp, champ[r].mp, champ[r].speed, champ[r].range, champ[r].position);
			}

			printf("삭제할 챔피언의 포지션을 입력해주세요: ");
			scanf(" %19[^\n]", PosDelChamp);

			for (int i = 0; i < champCount; i++) {
				if (strcmp(PosDelChamp, champ[i].position) == 0) {
					found = 1;

					champ[i] = champ[champCount - 1];
					champCount--;
					i--;
				}
			}

			if (!found) {
				printf("존재하지 않는 포지션입니다.");
				Sleep(3000);
				while (getchar() != '\n');
				break;
			}

			FILE* fp4 = fopen("testdata.txt", "w");

			if (fp4 == NULL) {
				printf("파일 열기 실패");
				Sleep(3000);
				break;
			}

			for (int j = 0; j < champCount; j++) {
				if (j == champCount - 1) {
					fprintf(fp4, "%s\t%d\t%d\t%d\t%d\t%s", champ[j].name, champ[j].hp, champ[j].mp, champ[j].speed, champ[j].range, champ[j].position);
				}
				else {
					fprintf(fp4, "%s\t%d\t%d\t%d\t%d\t%s\n", champ[j].name, champ[j].hp, champ[j].mp, champ[j].speed, champ[j].range, champ[j].position);
				}
			}

			fclose(fp4);

			printf("해당 포지션인 챔피언 모두 삭제 완료!");
			Sleep(3000);

			break;
		}
		case 5: {
			for (int i = 0; i < champCount; i++) {
				printf("%s\t%d\t%d\t%d\t%d\t%s\n", champ[i].name, champ[i].hp, champ[i].mp, champ[i].speed, champ[i].range, champ[i].position);
			}
			system("pause");

			break;
		}
		case 6: {
			int maxHp = champ[0].hp;

			for (int i = 1; i < champCount; i++) {
				if (champ[i].hp > maxHp) {
					maxHp = champ[i].hp;
				}
			}

			for (int j = 0; j < champCount; j++) {
				if (champ[j].hp == maxHp) {
					printf("가장 체력이 큰 챔피언의 정보\n");
					printf("name: %s\n", champ[j].name);
					printf("hp: %d\n", champ[j].hp);
					printf("mp: %d\n", champ[j].mp);
					printf("speed: %d\n", champ[j].speed);
					printf("range: %d\n", champ[j].range);
					printf("position: %s\n\n", champ[j].position);
				}
			}

			system("pause");

			break;
		}
		case 7: {
			LOL maxHpChamp[SIZE];
			LOL tempChamp[SIZE];
			int originChampCount = champCount;
			int t = 0;

			memcpy(tempChamp, champ, sizeof(LOL)* champCount);

			while (t < originChampCount) {
				int maxHp = tempChamp[0].hp;
				int maxIndex = 0;

				for (int i = 1; i < champCount; i++) {
					if (tempChamp[i].hp > maxHp) {
						maxHp = tempChamp[i].hp;
						maxIndex = i;
					}
				}

				maxHpChamp[t] = tempChamp[maxIndex];
				t++;

				tempChamp[maxIndex] = tempChamp[champCount - 1];
				champCount--;
			}

			champCount = originChampCount;

			for (int i = 0; i < champCount; i++) {
				champ[i] = maxHpChamp[i];
			}

			FILE* fp5 = fopen("testdata.txt", "w");

			if (fp5 == NULL) {
				printf("파일 열기 실패");
				Sleep(3000);
				break;
			}

			for (int j = 0; j < champCount; j++) {
				if (j == champCount - 1) {
					fprintf(fp5, "%s\t%d\t%d\t%d\t%d\t%s", maxHpChamp[j].name, maxHpChamp[j].hp, maxHpChamp[j].mp, maxHpChamp[j].speed, maxHpChamp[j].range, maxHpChamp[j].position);
				}
				else {
					fprintf(fp5, "%s\t%d\t%d\t%d\t%d\t%s\n", maxHpChamp[j].name, maxHpChamp[j].hp, maxHpChamp[j].mp, maxHpChamp[j].speed, maxHpChamp[j].range, maxHpChamp[j].position);
				}
			}

			fclose(fp5);

			printf("파일에 체력이 큰 챔피언부터 순서대로 저장 완료!");
			Sleep(3000);
			
			break;
		}
		}
	}

	return 0;
}