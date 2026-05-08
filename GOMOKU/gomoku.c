#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>

#define SIZE 100
#define ROW 19
#define COL 19

typedef struct {
	int black;
	int white;
} Stone;

typedef struct {
	int maxBlack;
	int maxWhite;

	int startIdxBlack[ROW];
	int startIdxWhite[ROW];

	int blackGroupCount;
	int whiteGroupCount;
} Sequence;

void printBoard(char board[ROW][COL]) {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			printf("%c ", board[i][j]);
		}
		printf("\n");
	}
}

Stone analyzeLine(char board[], int length) {
	Stone line;
	int lineBlackCount = 0;
	int lineWhiteCount = 0;

	for (int i = 0; i < length; i++) {
		if (board[i] == '@')
			lineBlackCount++;
		else if (board[i] == 'o')
			lineWhiteCount++;
	}

	line.black = lineBlackCount;
	line.white = lineWhiteCount;

	return line;
}

void processRows(char board[][COL], Stone RowLine[]) {
	for (int i = 0; i < ROW; i++)
		RowLine[i] = analyzeLine(board[i], COL);
}

void processCols(char board[][COL], Stone ColLine[]) {
	char temp[ROW];

	for (int x = 0; x < COL; x++) {
		for (int y = 0; y < ROW; y++) {
			temp[y] = board[y][x];
		}
		ColLine[x] = analyzeLine(temp, ROW);
	}
}

void printRowsCols(Stone RowLine[], Stone ColLine[]) {
	for (int i = 0; i < ROW; i++) {
		printf("%2d번째 가로줄 -> 검은 돌(@): %2d개, 흰 돌(o): %2d개 ", i + 1, RowLine[i].black, RowLine[i].white);
		printf(" | ");
		printf("%2d번째 세로줄 -> 검은 돌(@): %2d개, 흰 돌(o): %2d개 ", i + 1, ColLine[i].black, ColLine[i].white);
		printf("\n");
	}
}

Sequence analyzeMaxSequence(char board[], int length) {
	Sequence TheMostStone = { 0 };
	char currentStone = board[0];
	int currentCount;
	int currentStart = 0;

	if (board[0] == '+')
		currentCount = 0;
	else {
		currentCount = 1;
		currentStart = 0;
	}

	int maxBlack = 0;
	int maxWhite = 0;

	for (int i = 1; i < length; i++) {
		if (board[i] == currentStone)
			currentCount++;
		else {
			if (currentStone == '@') {
				if (currentCount > maxBlack) {
					maxBlack = currentCount;

					TheMostStone.blackGroupCount = 0;

					TheMostStone.startIdxBlack[TheMostStone.blackGroupCount++] = currentStart;
				}
				else if (currentCount == maxBlack)
					TheMostStone.startIdxBlack[TheMostStone.blackGroupCount++] = currentStart;
			}

			if (currentStone == 'o') {
				if (currentCount > maxWhite) {
					maxWhite = currentCount;

					TheMostStone.whiteGroupCount = 0;

					TheMostStone.startIdxWhite[TheMostStone.whiteGroupCount++] = currentStart;
				}
				else if (currentCount == maxWhite)
					TheMostStone.startIdxWhite[TheMostStone.whiteGroupCount++] = currentStart;
			}

			currentStone = board[i];

			if (board[i] == '+')
				currentCount = 0;
			else {
				currentCount = 1;
				currentStart = i;
			}
		}
	}
	if (currentStone == '@') {
		if (currentCount > maxBlack) {
			maxBlack = currentCount;

			TheMostStone.blackGroupCount = 0;

			TheMostStone.startIdxBlack[TheMostStone.blackGroupCount++] = currentStart;
		}
		else if (currentCount == maxBlack)
			TheMostStone.startIdxBlack[TheMostStone.blackGroupCount++] = currentStart;
	}

	if (currentStone == 'o') {
		if (currentCount > maxWhite) {
			maxWhite = currentCount;

			TheMostStone.whiteGroupCount = 0;

			TheMostStone.startIdxWhite[TheMostStone.whiteGroupCount++] = currentStart;
		}
		else if (currentCount == maxWhite)
			TheMostStone.startIdxWhite[TheMostStone.whiteGroupCount++] = currentStart;
	}

	TheMostStone.maxBlack = maxBlack;
	TheMostStone.maxWhite = maxWhite;

	return TheMostStone;
}

void processRowsMax(char board[][COL], Sequence RowMax[]) {
	for (int i = 0; i < ROW; i++)
		RowMax[i] = analyzeMaxSequence(board[i], COL);
}

void processColsMax(char board[][COL], Sequence ColMax[]) {
	char temp[ROW];

	for (int x = 0; x < COL; x++) {
		for (int y = 0; y < ROW; y++) {
			temp[y] = board[y][x];
		}
		ColMax[x] = analyzeMaxSequence(temp, ROW);
	}
}

void printRowsColsMax(Sequence RowMax[], Sequence ColMax[]) {
	for (int i = 0; i < ROW; i++) {
		printf("%2d번째 가로줄 -> ", i + 1);

		if (RowMax[i].maxBlack < 2 && RowMax[i].maxWhite < 2)
			printf("연속된 돌 없음");
		else if (RowMax[i].maxBlack > RowMax[i].maxWhite)
			printf("@ (%2d개) ", RowMax[i].maxBlack);
		else if (RowMax[i].maxWhite > RowMax[i].maxBlack)
			printf("o (%2d개) ", RowMax[i].maxWhite);
		else
			printf("@ (%2d개), o (%2d개) ", RowMax[i].maxBlack, RowMax[i].maxWhite);

		printf(" | ");

		printf("%2d번째 세로줄 -> ", i + 1);

		if (ColMax[i].maxBlack < 2 && ColMax[i].maxWhite < 2)
			printf("연속된 돌 없음");
		else if (ColMax[i].maxBlack > ColMax[i].maxWhite)
			printf("@ (%2d개) ", ColMax[i].maxBlack);
		else if (ColMax[i].maxWhite > ColMax[i].maxBlack)
			printf("o (%2d개) ", ColMax[i].maxWhite);
		else
			printf("@ (%2d개), o (%2d개) ", ColMax[i].maxBlack, ColMax[i].maxWhite);

		printf("\n");
	}
}

void processDiagonalMax(char board[][COL], Sequence RightDiaMax[], Sequence LeftDiaMax[]) {
	char RightDiaTemp[ROW];
	int RightDiaIndex = 0;
	char LeftDiaTemp[ROW];
	int LeftDiaIndex = 0;

	for (int y = ROW - 1; y >= 1; y--) {
		int x = 0;
		int cy = y;
		int k = 0;

		while (x < COL && cy < ROW) {
			RightDiaTemp[k++] = board[cy][x];
			x++;
			cy++;
		}
		RightDiaMax[RightDiaIndex++] = analyzeMaxSequence(RightDiaTemp, k);
	}

	for (int x = 0; x < COL; x++) {
		int cx = x;
		int y = 0;
		int k = 0;

		while (cx < COL && y < ROW) {
			RightDiaTemp[k++] = board[y][cx];
			cx++;
			y++;
		}
		RightDiaMax[RightDiaIndex++] = analyzeMaxSequence(RightDiaTemp, k);
	}

	for (int y = ROW - 1; y >= 1; y--) {
		int x = COL - 1;
		int cy = y;
		int k = 0;

		while (x >= 0 && cy < ROW) {
			LeftDiaTemp[k++] = board[cy][x];
			x--;
			cy++;
		}
		LeftDiaMax[LeftDiaIndex++] = analyzeMaxSequence(LeftDiaTemp, k);
	}

	for (int x = COL - 1; x >= 0; x--) {
		int cx = x;
		int y = 0;
		int k = 0;

		while (cx >= 0 && y < ROW) {
			LeftDiaTemp[k++] = board[y][cx];
			cx--;
			y++;
		}
		LeftDiaMax[LeftDiaIndex++] = analyzeMaxSequence(LeftDiaTemp, k);
	}
}

void printDiagonalMax(Sequence RightDiaMax[], Sequence LeftDiaMax[]) {
	printf("     (0, 18)부터 1번째 대각선     |     (18, 18)부터 1번째 대각선\n");
	printf("                \u2198                 |                 \u2199\n");

	for (int i = 0; i < ROW + COL - 1; i++) {
		printf("%2d번째 \u2198 대각선 -> ", i + 1);

		if (RightDiaMax[i].maxBlack < 2 && RightDiaMax[i].maxWhite < 2)
			printf("연속된 돌 없음");
		else if (RightDiaMax[i].maxBlack > RightDiaMax[i].maxWhite)
			printf("@ (%2d개) ", RightDiaMax[i].maxBlack);
		else if (RightDiaMax[i].maxWhite > RightDiaMax[i].maxBlack)
			printf("o (%2d개) ", RightDiaMax[i].maxWhite);
		else
			printf("@ (%2d개), o (%2d개) ", RightDiaMax[i].maxBlack, RightDiaMax[i].maxWhite);

		printf(" | ");

		printf("%2d번째 \u2199 대각선 -> ", i + 1);

		if (LeftDiaMax[i].maxBlack < 2 && LeftDiaMax[i].maxWhite < 2)
			printf("연속된 돌 없음");
		else if (LeftDiaMax[i].maxBlack > LeftDiaMax[i].maxWhite)
			printf("@ (%2d개) ", LeftDiaMax[i].maxBlack);
		else if (LeftDiaMax[i].maxWhite > LeftDiaMax[i].maxBlack)
			printf("o (%2d개) ", LeftDiaMax[i].maxWhite);
		else
			printf("@ (%2d개), o (%2d개) ", LeftDiaMax[i].maxBlack, LeftDiaMax[i].maxWhite);

		printf("\n");
	}
}

void printRowsMaxXY(Sequence rowsMaxXY[]) {
	int g, x, start;

	for (int i = 0; i < ROW; i++) {
		printf("%2d번째 가로줄 -> ", i + 1);

		if (rowsMaxXY[i].maxBlack < 2 && rowsMaxXY[i].maxWhite < 2)
			printf("연속된 돌 없음");
		else if (rowsMaxXY[i].maxBlack > rowsMaxXY[i].maxWhite) {
			printf("@: ");
			for (g = 0; g < rowsMaxXY[i].blackGroupCount; g++) {
				start = rowsMaxXY[i].startIdxBlack[g];

				for (x = start; x < start + rowsMaxXY[i].maxBlack; x++)
					printf("(%d, %d)", x, i);
				
				if (g != rowsMaxXY[i].blackGroupCount - 1)
					printf(" / ");
			}
		}
		else if (rowsMaxXY[i].maxWhite > rowsMaxXY[i].maxBlack) {
			printf("o: ");
			for (g = 0; g < rowsMaxXY[i].whiteGroupCount; g++) {
				start = rowsMaxXY[i].startIdxWhite[g];

				for (x = start; x < start + rowsMaxXY[i].maxWhite; x++)
					printf("(%d, %d)", x, i);

				if (g != rowsMaxXY[i].whiteGroupCount - 1)
					printf(" / ");
			}
		}
		else {
			printf("@: ");
			for (g = 0; g < rowsMaxXY[i].blackGroupCount; g++) {
				start = rowsMaxXY[i].startIdxBlack[g];

				for (x = start; x < start + rowsMaxXY[i].maxBlack; x++)
					printf("(%d, %d)", x, i);

				if (g != rowsMaxXY[i].blackGroupCount - 1)
					printf(" / ");
			}

			printf("o: ");
			for (g = 0; g < rowsMaxXY[i].whiteGroupCount; g++) {
				start = rowsMaxXY[i].startIdxWhite[g];

				for (x = start; x < start + rowsMaxXY[i].maxWhite; x++)
					printf("(%d, %d)", x, i);

				if (g != rowsMaxXY[i].whiteGroupCount - 1)
					printf(" / ");
			}
		}

		printf("\n");
	}
}

int main() {
	char extra;
	char line2[SIZE];

	char board[ROW][COL];
	int inputX;
	int inputY;
	int turn = 0;
	int CountBlack = 0;
	int CountWhite = 0;

	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			board[i][j] = '+';
		}
	}

	while (1) {
		system("cls");

		int firstNum;
		char line1[SIZE];
		char notFirstNum;

		printBoard(board);

		printf("1. 돌 놓기\n");
		printf("2. 개수 확인\n");
		printf("0. 종료\n");
		printf("원하시는 옵션의 숫자를 입력해주세요: ");

		if (fgets(line1, sizeof(line1), stdin) == NULL)
			continue;

		int inputFirstNum = sscanf(line1, "%d %c", &firstNum, &notFirstNum);

		if (inputFirstNum != 1 || firstNum < 0 || firstNum > 2) {
			printf("없는 옵션입니다.");
			Sleep(3000);
			continue;
		}

		if (firstNum == 0) {
			printf("\n프로그램 종료\n");
			return 0;
		}

		switch (firstNum) {
		case 1: {
			printf("판에 놓인 검은 돌의 개수: %d, 판에 놓인 흰 돌의 개수: %d\n", CountBlack, CountWhite);

			if (turn % 2 == 0) {
				printf("검은 돌을 놓을 좌표(x y)를 입력하세요: ");
			}
			else {
				printf("흰 돌을 놓을 좌표(x y)를 입력하세요: ");
			}

			if (fgets(line2, sizeof(line2), stdin) == NULL)
				continue;

			int inputCount = sscanf(line2, "%d %d %c", &inputX, &inputY, &extra);

			if (inputCount != 2) {
				printf("숫자 2개만 입력해주세요.");
				Sleep(3000);
				continue;
			}

			if (inputX < 0 || inputX >= COL || inputY < 0 || inputY >= ROW) {
				printf("범위를 벗어난 좌표입니다.");
				Sleep(3000);
			}
			else if (board[inputY][inputX] == '@' || board[inputY][inputX] == 'o') {
				printf("이미 돌이 놓여 있습니다.");
				Sleep(3000);
			}
			else if (turn % 2 == 0) {
				board[inputY][inputX] = '@';
				turn++;
				CountBlack++;
			}
			else {
				board[inputY][inputX] = 'o';
				turn++;
				CountWhite++;
			}

			if (CountBlack + CountWhite == ROW * COL) {
				system("cls");

				printBoard(board);

				printf("놓인 검은 돌의 개수: %d, 놓인 흰 돌의 개수: %d\n", CountBlack, CountWhite);
				printf("게임 끝!");
				break;
			}
			break;
		}
		case 2: {
			int secondNum;
			char line3[SIZE];
			char notSecondNum;

			printf("1. 모든 가로/세로줄에 대한 검은 돌 & 흰 돌 개수\n");
			printf("2. 각각의 모든 가로/세로줄에서 가장 많이 연속된 돌의 종류/개수\n");
			printf("3. 각각의 모든 대각선줄에서 가장 많이 연속된 돌의 종류/개수\n");
			printf("4. 각각의 모든 가로줄에서 가장 많이 연속된 돌들의 좌표\n");
			printf("원하시는 옵션의 숫자를 입력해주세요: ");

			if (fgets(line3, sizeof(line3), stdin) == NULL)
				continue;

			int inputSecondNum = sscanf(line3, "%d %c", &secondNum, &notSecondNum);

			if (inputSecondNum != 1 || secondNum < 1 || secondNum > 4) {
				printf("없는 옵션입니다.");
				Sleep(3000);
				continue;
			}

			switch (secondNum) {
			case 1: {
				Stone rows[ROW];
				Stone cols[COL];

				processRows(board, rows);
				processCols(board, cols);
				printRowsCols(rows, cols);

				system("pause");

				break;
			}
			case 2: {
				Sequence rowsMax[ROW];
				Sequence colsMax[COL];

				processRowsMax(board, rowsMax);
				processColsMax(board, colsMax);
				printRowsColsMax(rowsMax, colsMax);

				system("pause");

				break;
			}
			case 3: {
				Sequence rightDiaMax[ROW + COL - 1];
				Sequence leftDiaMax[ROW + COL - 1];

				processDiagonalMax(board, rightDiaMax, leftDiaMax);
				printDiagonalMax(rightDiaMax, leftDiaMax);

				system("pause");

				break;
			}
			case 4: {
				Sequence rowsMaxXY[ROW];

				processRowsMax(board, rowsMaxXY);
				printRowsMaxXY(rowsMaxXY);

				system("pause");

				break;
			}
			}
		}
		}
	}
	return 0;
}