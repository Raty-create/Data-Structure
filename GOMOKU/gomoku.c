#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

#define SAVE_FILE "gomoku_save.txt"
#define SIZE 100
#define ROW 19
#define COL 19
#define COUNT(arr) (sizeof(arr) / sizeof(arr[0]))
#define MAX_PATTERN 19

// 무르기 기능을 사용하기 위해 돌을 놓은 좌표를 스택에 저장
#define MAX_MOVES (ROW * COL)
int undoHistory[MAX_MOVES][2];
int undoCount = 0;

// 무르기를 할 경우 그때의 돌 좌표 스택에 저장
int redoHistory[MAX_MOVES][2];
int redoCount = 0;

typedef struct { int black, white; } Stone;

typedef struct {
	int x[ROW + COL], y[ROW + COL];
	int count;

	int frontX, frontY;
	int gapX, gapY;
	int backX, backY;
} Pattern;

typedef struct {
	Pattern black[MAX_PATTERN], white[MAX_PATTERN];
	int blackGroupCount, whiteGroupCount;
	int maxBlack, maxWhite;
} Sequence;

// 판
void printBoard(char board[ROW][COL]) {
	printf("   ");

	for (int x = 0; x < COL; x++)
		printf("%2d ", x);
	printf("\n");

	for (int y = 0; y < ROW; y++) {
		printf("%2d ", y);

		for (int x = 0; x < COL; x++)
			printf("%2c ", board[y][x]);
		printf("\n");
	}
}

// 돌 개수 분석
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
		printf("%2d번째 가로줄 -> @: %2d개, o: %2d개 ", i + 1, RowLine[i].black, RowLine[i].white);
		printf(" | ");
		printf("%2d번째 세로줄 -> @: %2d개, o: %2d개 ", i + 1, ColLine[i].black, ColLine[i].white);
		printf("\n");
	}
}

// 흑 또는 백 패턴을 Sequence에 등록 - 새 최대값이면 기존 기록 초기화, 같은 최대값이면 추가
void updateSequence(Sequence* seq, char stone, int count, int posX[], int posY[], int posCount) {
	int* pMax = (stone == '@') ? &seq->maxBlack : &seq->maxWhite;
	int* pCnt = (stone == '@') ? &seq->blackGroupCount : &seq->whiteGroupCount;
	Pattern* pg = (stone == '@') ? seq->black : seq->white;

	if (stone != '@' && stone != 'o') return;

	if (count > *pMax) {
		*pMax = count;
		*pCnt = 0;
	}
	if (count == *pMax && *pCnt < MAX_PATTERN) {
		Pattern* g = &pg[(*pCnt)++];

		g->count = posCount;
		for (int i = 0; i < posCount; i++) {
			g->x[i] = posX[i];
			g->y[i] = posY[i];
		}
	}
}

// 최대값 비교없이 3 또는 4 위치 저장
void updateThreat(Sequence* seq, char stone, int posX[], int posY[], int posCount, Pattern* saveGap) {
	int* pCnt = (stone == '@') ? &seq->blackGroupCount : &seq->whiteGroupCount;
	Pattern* pg = (stone == '@') ? seq->black : seq->white;

	if (*pCnt >= MAX_PATTERN) return;

	Pattern* g = &pg[(*pCnt)++];

	g->count = posCount;
	for (int i = 0; i < posCount; i++) {
		g->x[i] = posX[i];
		g->y[i] = posY[i];
	}

	g->frontX = saveGap->frontX, g->frontY = saveGap->frontY;
	g->gapX = saveGap->gapX, g->gapY = saveGap->gapY;
	g->backX = saveGap->backX, g->backY = saveGap->backY;
}

// 연속된 돌 분석 (Gap 없음)
Sequence analyzeSeq(char line[], int posX[], int posY[], int length) {
	Sequence result = { 0 };

	char currentStone = '+';

	int saveX[ROW + COL];
	int saveY[ROW + COL];

	int posCount = 0;
	int currentCount = 0;

	for (int i = 0; i < length; i++) {
		if (line[i] == currentStone && line[i] != '+') {
			currentCount++;

			saveX[posCount] = posX[i];
			saveY[posCount] = posY[i];

			posCount++;
		}
		else {
			updateSequence(&result, currentStone, currentCount, saveX, saveY, posCount);

			if (line[i] == '+') {
				currentStone = '+';
				currentCount = 0;
				posCount = 0;
			}
			else {
				currentStone = line[i];

				currentCount = 1;

				posCount = 0;
				saveX[posCount] = posX[i];
				saveY[posCount] = posY[i];

				posCount++;
			}
		}
	}
	updateSequence(&result, currentStone, currentCount, saveX, saveY, posCount);

	return result;
}

// 한 칸 띈 돌 분석
Sequence analyzeSequenceGap(char line[], int posX[], int posY[], int length) {
	Sequence result = { 0 };

	for (int start = 0; start < length; start++) {
		if (line[start] != '@' && line[start] != 'o') continue;

		char stone = line[start];

		int saveX[COL];
		int saveY[ROW];

		int posCount = 0;

		int count = 0;
		int emptyUsed = 0;

		for (int i = start; i < length; i++) {
			if (line[i] == stone) {
				count++;

				saveX[posCount] = posX[i];
				saveY[posCount] = posY[i];

				posCount++;
			}
			else if (line[i] == '+' && emptyUsed == 0 && i + 1 < length && line[i + 1] == stone) {
				emptyUsed = 1;
			}
			else break;
		}

		updateSequence(&result, stone, count, saveX, saveY, posCount);
	}

	return result;
}

// count가 3 또는 4인 패턴만 저장
Sequence analyzeThreat(char line[], int posX[], int posY[], int length) {
	Sequence result = { 0 };

	for (int start = 0; start < length; start++) {
		if (line[start] != '@' && line[start] != 'o') continue;

		char stone = line[start];

		int saveX[COL];
		int saveY[ROW];

		int posCount = 0;

		int count = 0;
		int emptyUsed = 0;

		Pattern saveGap = { 0 };
		saveGap.frontX = saveGap.frontY = -1;
		saveGap.gapX = saveGap.gapY = -1;
		saveGap.backX = saveGap.backY = -1;

		if (start > 0 && line[start - 1] == '+') {
			saveGap.frontX = posX[start - 1];
			saveGap.frontY = posY[start - 1];
		}

		int i;
		for (i = start; i < length; i++) {
			if (line[i] == stone) {
				count++;

				saveX[posCount] = posX[i];
				saveY[posCount] = posY[i];

				posCount++;
			}
			else if (line[i] == '+' && emptyUsed == 0 && i + 1 < length && line[i + 1] == stone) {
				emptyUsed = 1;

				saveGap.gapX = posX[i];
				saveGap.gapY = posY[i];
			}
			else break;
		}
		if (count == 3 || count == 4) {
			if (i < length && line[i] == '+') {
				saveGap.backX = posX[i];
				saveGap.backY = posY[i];
			}
			updateThreat(&result, stone, saveX, saveY, posCount, &saveGap);
		}
	}

	return result;
}

// 가로 연속 돌 분석 (Gap 없음)
void processRowsMax(char board[][COL], Sequence RowMax[]) {
	for (int y = 0; y < ROW; y++) {
		int posX[COL];
		int posY[COL];

		for (int x = 0; x < COL; x++) {
			posX[x] = x;
			posY[x] = y;
		}
		RowMax[y] = analyzeSeq(board[y], posX, posY, COL);
	}
}

// 한 칸 띈 가로 연속 돌 분석
void processRowsMaxGap(char board[][COL], Sequence rowGap[]) {
	for (int y = 0; y < ROW; y++) {
		char temp[COL];

		int posX[COL];
		int posY[COL];

		for (int x = 0; x < COL; x++) {
			temp[x] = board[y][x];

			posX[x] = x;
			posY[x] = y;
		}
		rowGap[y] = analyzeSequenceGap(temp, posX, posY, COL);
	}
}

// 가로 3 또는 4 위험 돌 분석
void processRowsThreat(char board[][COL], Sequence rowGap[]) {
	for (int y = 0; y < ROW; y++) {
		char temp[COL];

		int posX[COL];
		int posY[COL];

		for (int x = 0; x < COL; x++) {
			temp[x] = board[y][x];

			posX[x] = x;
			posY[x] = y;
		}
		rowGap[y] = analyzeThreat(temp, posX, posY, COL);
	}
}

// 세로 연속 돌 분석 (Gap 없음)
void processColsMax(char board[][COL], Sequence ColMax[]) {
	char temp[ROW];

	for (int x = 0; x < COL; x++) {
		int posX[ROW];
		int posY[ROW];

		for (int y = 0; y < ROW; y++) {
			temp[y] = board[y][x];

			posX[y] = x;
			posY[y] = y;
		}
		ColMax[x] = analyzeSeq(temp, posX, posY, ROW);
	}
}

// 한 칸 띈 세로 연속 돌 분석
void processColsMaxGap(char board[][COL], Sequence colGap[]) {
	char temp[ROW];

	int posX[ROW];
	int posY[ROW];

	for (int x = 0; x < COL; x++) {
		for (int y = 0; y < ROW; y++) {
			temp[y] = board[y][x];

			posX[y] = x;
			posY[y] = y;
		}
		colGap[x] = analyzeSequenceGap(temp, posX, posY, ROW);
	}
}

// 세로 3 또는 4 위험 돌 분석
void processColsThreat(char board[][COL], Sequence colGap[]) {
	char temp[ROW];

	int posX[ROW];
	int posY[ROW];

	for (int x = 0; x < COL; x++) {
		for (int y = 0; y < ROW; y++) {
			temp[y] = board[y][x];

			posX[y] = x;
			posY[y] = y;
		}
		colGap[x] = analyzeThreat(temp, posX, posY, ROW);
	}
}

// 대각 연속 돌 분석 (Gap 없음)
void processDiagonalMax(char board[][COL], Sequence RightDiaMax[], Sequence LeftDiaMax[]) {
	char RightDiaTemp[ROW];
	int RightDiaIndex = 0;
	char LeftDiaTemp[ROW];
	int LeftDiaIndex = 0;

	int posX[ROW + COL];
	int posY[ROW + COL];

	for (int y = ROW - 1; y >= 1; y--) {
		int x = 0;
		int cy = y;
		int k = 0;

		while (x < COL && cy < ROW) {
			RightDiaTemp[k] = board[cy][x];

			posX[k] = x;
			posY[k] = cy;

			k++;
			x++;
			cy++;
		}
		RightDiaMax[RightDiaIndex++] = analyzeSeq(RightDiaTemp, posX, posY, k);
	}

	for (int x = 0; x < COL; x++) {
		int cx = x;
		int y = 0;
		int k = 0;

		while (cx < COL && y < ROW) {
			RightDiaTemp[k] = board[y][cx];

			posX[k] = cx;
			posY[k] = y;

			k++;
			cx++;
			y++;
		}
		RightDiaMax[RightDiaIndex++] = analyzeSeq(RightDiaTemp, posX, posY, k);
	}

	for (int y = ROW - 1; y >= 1; y--) {
		int x = COL - 1;
		int cy = y;
		int k = 0;

		while (x >= 0 && cy < ROW) {
			LeftDiaTemp[k] = board[cy][x];

			posX[k] = x;
			posY[k] = cy;

			k++;
			x--;
			cy++;
		}
		LeftDiaMax[LeftDiaIndex++] = analyzeSeq(LeftDiaTemp, posX, posY, k);
	}

	for (int x = COL - 1; x >= 0; x--) {
		int cx = x;
		int y = 0;
		int k = 0;

		while (cx >= 0 && y < ROW) {
			LeftDiaTemp[k] = board[y][cx];

			posX[k] = cx;
			posY[k] = y;

			k++;
			cx--;
			y++;
		}
		LeftDiaMax[LeftDiaIndex++] = analyzeSeq(LeftDiaTemp, posX, posY, k);
	}
}

// 한 칸 띈 대각 연속 돌 분석
void processDiagonalMaxGap(char board[][COL], Sequence RightDiaGap[], Sequence LeftDiaGap[]) {
	char RightDiaTemp[ROW];
	int RightDiaIndex = 0;
	char LeftDiaTemp[ROW];
	int LeftDiaIndex = 0;

	int posX[COL];
	int posY[ROW];

	for (int y = ROW - 1; y >= 1; y--) {
		int x = 0;
		int cy = y;
		int k = 0;

		while (x < COL && cy < ROW) {
			RightDiaTemp[k] = board[cy][x];

			posX[k] = x;
			posY[k] = cy;

			k++;
			x++;
			cy++;
		}
		RightDiaGap[RightDiaIndex++] = analyzeSequenceGap(RightDiaTemp, posX, posY, k);
	}

	for (int x = 0; x < COL; x++) {
		int cx = x;
		int y = 0;
		int k = 0;

		while (cx < COL && y < ROW) {
			RightDiaTemp[k] = board[y][cx];

			posX[k] = cx;
			posY[k] = y;

			k++;
			cx++;
			y++;
		}
		RightDiaGap[RightDiaIndex++] = analyzeSequenceGap(RightDiaTemp, posX, posY, k);
	}

	for (int y = ROW - 1; y >= 1; y--) {
		int x = COL - 1;
		int cy = y;
		int k = 0;

		while (x >= 0 && cy < ROW) {
			LeftDiaTemp[k] = board[cy][x];

			posX[k] = x;
			posY[k] = cy;

			k++;
			x--;
			cy++;
		}
		LeftDiaGap[LeftDiaIndex++] = analyzeSequenceGap(LeftDiaTemp, posX, posY, k);
	}

	for (int x = COL - 1; x >= 0; x--) {
		int cx = x;
		int y = 0;
		int k = 0;

		while (cx >= 0 && y < ROW) {
			LeftDiaTemp[k] = board[y][cx];

			posX[k] = cx;
			posY[k] = y;

			k++;
			cx--;
			y++;
		}
		LeftDiaGap[LeftDiaIndex++] = analyzeSequenceGap(LeftDiaTemp, posX, posY, k);
	}
}

// 대각 3 또는 4 위험 돌 분석
void processDiagonalThreat(char board[][COL], Sequence RightDiaGap[], Sequence LeftDiaGap[]) {
	char RightDiaTemp[ROW];
	int RightDiaIndex = 0;
	char LeftDiaTemp[ROW];
	int LeftDiaIndex = 0;

	int posX[COL];
	int posY[ROW];

	for (int y = ROW - 1; y >= 1; y--) {
		int x = 0;
		int cy = y;
		int k = 0;

		while (x < COL && cy < ROW) {
			RightDiaTemp[k] = board[cy][x];

			posX[k] = x;
			posY[k] = cy;

			k++;
			x++;
			cy++;
		}
		RightDiaGap[RightDiaIndex++] = analyzeThreat(RightDiaTemp, posX, posY, k);
	}

	for (int x = 0; x < COL; x++) {
		int cx = x;
		int y = 0;
		int k = 0;

		while (cx < COL && y < ROW) {
			RightDiaTemp[k] = board[y][cx];

			posX[k] = cx;
			posY[k] = y;

			k++;
			cx++;
			y++;
		}
		RightDiaGap[RightDiaIndex++] = analyzeThreat(RightDiaTemp, posX, posY, k);
	}

	for (int y = ROW - 1; y >= 1; y--) {
		int x = COL - 1;
		int cy = y;
		int k = 0;

		while (x >= 0 && cy < ROW) {
			LeftDiaTemp[k] = board[cy][x];

			posX[k] = x;
			posY[k] = cy;

			k++;
			x--;
			cy++;
		}
		LeftDiaGap[LeftDiaIndex++] = analyzeThreat(LeftDiaTemp, posX, posY, k);
	}

	for (int x = COL - 1; x >= 0; x--) {
		int cx = x;
		int y = 0;
		int k = 0;

		while (cx >= 0 && y < ROW) {
			LeftDiaTemp[k] = board[y][cx];

			posX[k] = cx;
			posY[k] = y;

			k++;
			cx--;
			y++;
		}
		LeftDiaGap[LeftDiaIndex++] = analyzeThreat(LeftDiaTemp, posX, posY, k);
	}
}

// 연속 돌 수 요약 문자열 출력
static void printSeqSummary(Sequence* s) {
	int b = s->maxBlack, w = s->maxWhite;
	if (b < 2 && w < 2) { printf("연속된 돌 없음"); return; }
	if (b > w) printf("@ (%2d개) ", b);
	else if (w > b) printf("o (%2d개) ", w);
	else printf("@ (%2d개), o (%2d개) ", b, w);
}

// 가로/세로 연속 돌 개수 출력 (Gap 없음)
void printRowsColsMax(Sequence RowMax[], Sequence ColMax[]) {
	for (int i = 0; i < ROW; i++) {
		printf("%2d번째 가로줄 -> ", i + 1); printSeqSummary(&RowMax[i]);
		printf(" | ");
		printf("%2d번째 세로줄 -> ", i + 1); printSeqSummary(&ColMax[i]);
		printf("\n");
	}
}

// 대각 연속 돌 출력 (Gap 없음)
void printDiagonalMax(Sequence RightDiaMax[], Sequence LeftDiaMax[]) {
	printf("     (0, 18)부터 1번째 대각선     |     (18, 18)부터 1번째 대각선\n");
	printf("                \u2198                 |                 \u2199\n");

	for (int i = 0; i < ROW + COL - 1; i++) {
		printf("%2d번째 \u2198 대각선 -> ", i + 1); printSeqSummary(&RightDiaMax[i]);
		printf(" | ");
		printf("%2d번째 \u2199 대각선 -> ", i + 1); printSeqSummary(&LeftDiaMax[i]);
		printf("\n");
	}
}

// 패턴 하나의 좌표를 출력 (Gap 포함)
static void printPatternCoords(Pattern* grp, int showGap) {
	for (int i = 0; i < grp->count; i++) {
		printf("(%d, %d)", grp->x[i], grp->y[i]);

		if (showGap && i < grp->count - 1 && (abs(grp->x[i + 1] - grp->x[i]) > 1 || abs(grp->y[i + 1] - grp->y[i]) > 1)) printf(" [+] ");
		else printf(" ");
	}
}

// 가로/세로/대각 연속 돌 좌표 출력 (Gap 상관x)
void printSequence(Sequence seqs[], int count, const char* label, int showGap) {
	for (int n = 0; n < count; n++) {
		printf("%2d번째 %s -> ", n + 1, label);

		if (seqs[n].maxBlack < 2 && seqs[n].maxWhite < 2) {
			printf("연속된 돌 없음\n");
			continue;
		}

		if (seqs[n].maxBlack >= seqs[n].maxWhite) {
			printf("@: ");

			for (int g = 0; g < seqs[n].blackGroupCount; g++) {
				printPatternCoords(&seqs[n].black[g], showGap);

				if (g != seqs[n].blackGroupCount - 1) printf("/ ");
			}
		}

		if (seqs[n].maxWhite >= seqs[n].maxBlack) {
			printf("o: ");

			for (int g = 0; g < seqs[n].whiteGroupCount; g++) {
				printPatternCoords(&seqs[n].white[g], showGap);

				if (g != seqs[n].whiteGroupCount - 1) printf("/ ");
			}
		}
		printf("\n");
	}
}

// 돌 위치를 highlight (analySequenceGap 결과용)
static void markHighlight(char highlight[ROW][COL], Sequence seqs[], int count) {
	for (int n = 0; n < count; n++) {
		int b = seqs[n].maxBlack;
		int w = seqs[n].maxWhite;

		if (b >= 2 && b >= w) {
			for (int g = 0; g < seqs[n].blackGroupCount; g++) {
				Pattern* p = &seqs[n].black[g];

				for (int i = 0; i < p->count; i++)
					highlight[p->y[i]][p->x[i]] = 1;
			}
		}
		if (w >= 2 && w >= b) {
			for (int g = 0; g < seqs[n].whiteGroupCount; g++) {
				Pattern* p = &seqs[n].white[g];

				for (int i = 0; i < p->count; i++)
					highlight[p->y[i]][p->x[i]] = 2;
			}
		}
	}
}

// 빈칸 highlight (analyThreat 결과용)
static void markThreat(char highlight[ROW][COL], Sequence seqs[], int count, int turn) {
	int showBlack = (turn % 2 == 1);
	int showWhite = (turn % 2 == 0);

	for (int n = 0; n < count; n++) {
		if (showBlack) {
			for (int g = 0; g < seqs[n].blackGroupCount; g++) {
				Pattern* p = &seqs[n].black[g];

				if (p->count == 3 || p->count == 4) {
					if (p->frontX != -1) highlight[p->frontY][p->frontX] = 3;
					if (p->gapX != -1) highlight[p->gapY][p->gapX] = 3;
					if (p->backX != -1) highlight[p->backY][p->backX] = 3;
				}
			}
		}
		if (showWhite) {
			for (int g = 0; g < seqs[n].whiteGroupCount; g++) {
				Pattern* p = &seqs[n].white[g];

				if (p->count == 3 || p->count == 4) {
					if (p->frontX != -1) highlight[p->frontY][p->frontX] = 3;
					if (p->gapX != -1) highlight[p->gapY][p->gapX] = 3;
					if (p->backX != -1) highlight[p->backY][p->backX] = 3;
				}
			}
		}
	}
}

// 최대 연속 돌 위치를 강조해서 판 출력
void printBoardHighlight(char board[ROW][COL], Sequence seqs1[], int count1, Sequence seqs2[], int count2,
	Sequence threatRow[], int tCountRow, Sequence threatCol[], int tCountCol, Sequence threatRDia[], int tCountRDia, Sequence threatLDia[], int tCountLDia, int turn) {
	char highlight[ROW][COL] = { 0 };

	for (int y = 0; y < ROW; y++)
		for (int x = 0; x < COL; x++)
			highlight[y][x] = 0;

	if (seqs1) markHighlight(highlight, seqs1, count1);
	if (seqs2) markHighlight(highlight, seqs2, count2);
	if (threatRow) markThreat(highlight, threatRow, tCountRow, turn);
	if (threatCol) markThreat(highlight, threatCol, tCountCol, turn);
	if (threatRDia) markThreat(highlight, threatRDia, tCountRDia, turn);
	if (threatLDia) markThreat(highlight, threatLDia, tCountLDia, turn);

	// 판 출력 (강조 위치는 대괄호로 표시)
	printf("   ");
	for (int x = 0; x < COL; x++) printf("%2d ", x);
	printf("\n");

	for (int y = 0; y < ROW; y++) {
		printf("%2d ", y);
		for (int x = 0; x < COL; x++) {
			if (highlight[y][x]) printf("[%c]", board[y][x]);
			else printf("%2c ", board[y][x]);
		}
		printf("\n");
	}
}

static int readInt(int low, int high) {
	char buf[SIZE];
	char extra;
	int val;

	if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;
	if (buf[0] == 'u' || buf[0] == 'U') return -999;
	if (buf[0] == 'r' || buf[0] == 'R') return -998;
	if (sscanf(buf, "%d %c", &val, &extra) != 1) return 0;
	return (val >= low && val <= high) ? val : 0;
}

static void handlePlaceStone(char board[][COL], int* turn, int* countBlack, int* countWhite) {
	char line[SIZE];
	char extra;
	int inputX, inputY;

	printf("판에 놓인 검은 돌의 개수: %d, 판에 놓인 흰 돌의 개수: %d\n", *countBlack, *countWhite);
	printf("%s 돌을 놓은 좌표(x y)를 입력해주세요 (ex. 1 1): ", (*turn % 2 == 0) ? "검은" : "흰");

	if (fgets(line, sizeof(line), stdin) == NULL) return;
	if (sscanf(line, "%d %d %c", &inputX, &inputY, &extra) != 2) {
		printf("잘못된 입력입니다. 2개의 숫자(x y)를 입력해주세요."); Sleep(3000); return;
	}

	if (inputX < 0 || inputX >= COL || inputY < 0 || inputY >= ROW) {
		printf("범위를 벗어난 좌표입니다."); Sleep(3000); return;
	}
	if (board[inputY][inputX] != '+') {
		printf("이미 돌이 놓여 있습니다."); Sleep(3000); return;
	}

	if (*turn % 2 == 0) {
		board[inputY][inputX] = '@'; (*countBlack)++;
	}
	else {
		board[inputY][inputX] = 'o'; (*countWhite)++;
	}

	undoHistory[undoCount][0] = inputX;
	undoHistory[undoCount][1] = inputY;
	undoCount++;

	(*turn)++;

	if (*countBlack + *countWhite == ROW * COL) {
		system("cls");
		printBoard(board);
		printf("판에 놓인 검은 돌의 개수: %d, 판에 놓인 흰 돌의 개수: %d\n", *countBlack, *countWhite);
		printf("게임 끝!");
	}
}

static void handleAnalyzeMenu(char board[][COL]) {
	static const char* printSecondMenu[] = {
		"모든 가로/세로줄 돌 개수",
		"가로/세로줄 최대 연속 돌",
		"대각선 최대 연속 돌",
		"가로줄 연속 돌 좌표",
		"한 칸 띈 돌(가로)",
		"한 칸 띈 돌(세로)",
		"한 칸 띈 돌(대각선)" };

	for (int i = 0; i < (int)COUNT(printSecondMenu); i++)
		printf("%d. %s\n", i + 1, printSecondMenu[i]);
	printf("원하시는 옵션의 숫자를 입력해주세요: ");

	int secondNum = readInt(1, (int)COUNT(printSecondMenu));
	if (secondNum == 0) {
		printf("없는 옵션입니다."); Sleep(3000); return;
	}

	static Sequence rows[ROW], cols[COL];
	static Sequence rightDia[ROW + COL - 1], leftDia[ROW + COL - 1];

	switch (secondNum) {
	case 1: {
		Stone rowLine[ROW], colLine[COL];
		processRows(board, rowLine);
		processCols(board, colLine);
		printRowsCols(rowLine, colLine);
		break;
	}
	case 2:
		processRowsMax(board, rows);
		processColsMax(board, cols);
		printRowsColsMax(rows, cols);
		break;
	case 3:
		processDiagonalMax(board, rightDia, leftDia);
		printDiagonalMax(rightDia, leftDia);
		break;
	case 4:
		processRowsMax(board, rows);
		printSequence(rows, ROW, "가로줄", 0);
		break;
	case 5:
		system("cls");
		processRowsMaxGap(board, rows);
		printBoardHighlight(board, rows, ROW, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, 0);
		printSequence(rows, ROW, "가로줄", 1);
		break;
	case 6:
		system("cls");
		processColsMaxGap(board, cols);
		printBoardHighlight(board, cols, COL, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, 0);
		printSequence(cols, COL, "세로줄", 1);
		break;
	case 7:
		system("cls");
		processDiagonalMaxGap(board, rightDia, leftDia);
		printBoardHighlight(board, rightDia, ROW + COL - 1, leftDia, ROW + COL - 1, NULL, 0, NULL, 0, NULL, 0, NULL, 0, 0);
		printf("   (0, 18)부터 첫 번째 \u2198 대각선\n");
		printSequence(rightDia, ROW + COL - 1, "\u2198 대각선", 1);
		printf("\n");
		printSequence(leftDia, ROW + COL - 1, "\u2199 대각선", 1);
		break;
	}
	system("pause");
}

// 현재 게임 상태를 txt 파일로 저장
void saveGame(char board[ROW][COL], int turn, int countBlack, int countWhite) {
	FILE* file = fopen(SAVE_FILE, "w");
	if (file == NULL) {
		printf("저장 실패: 파일을 열 수 없습니다.\n");
		Sleep(3000);
		return;
	}

	// 차례, 돌 개수 정보 저장
	fprintf(file, "%d %d %d\n", turn, countBlack, countWhite);

	// 판 정보 저장
	for (int y = 0; y < ROW; y++) {
		for (int x = 0; x < COL; x++) {
			fprintf(file, "%c", board[y][x]);
		}
		fprintf(file, "\n");
	}

	// undo 정보 저장
	fprintf(file, "%d\n", undoCount);
	for (int i = 0; i < undoCount; i++) fprintf(file, "%d %d\n", undoHistory[i][0], undoHistory[i][1]);

	// redo 정보 저장
	fprintf(file, "%d\n", redoCount);
	for (int i = 0; i < redoCount; i++) fprintf(file, "%d %d\n", redoHistory[i][0], redoHistory[i][1]);

	fclose(file);
	printf("게임이 저장되었습니다.");
	Sleep(3000);
}

// 저장한 게임 상태를 txt 파일에서 로드
int loadGame(char board[ROW][COL], int* turn, int* countBlack, int* countWhite) {
	FILE* file = fopen(SAVE_FILE, "r");
	if (file == NULL) {
		printf("저장된 게임이 없습니다.");
		Sleep(3000);
		return 0;
	}

	// 차례, 돌 개수 정보 로드
	if (fscanf(file, "%d %d %d\n", turn, countBlack, countWhite) != 3) {
		printf("파일을 읽을 수 없습니다.");
		fclose(file);
		Sleep(3000);
		return 0;
	}

	// 판 정보 로드
	for (int y = 0; y < ROW; y++) {
		for (int x = 0; x < COL; x++) {
			int ch = fgetc(file);
			if (ch == '\n' || ch == EOF) {
				x--;
				continue;
			}
			board[y][x] = (char)ch;
		}
	}

	// undo 정보 로드
	if (fscanf(file, "%d\n", &undoCount) != 1) {
		printf("무르기 정보를 읽을 수 없습니다.");
		Sleep(3000);
		return 0;
	}

	for (int i = 0; i < undoCount; i++) {
		if (fscanf(file, "%d %d\n", &undoHistory[i][0], &undoHistory[i][1]) != 2) {
			printf("무르기 정보 로드 실패");
			fclose(file);
			Sleep(3000);
			return 0;
		}
	}

	// redo 정보 로드
	if (fscanf(file, "%d\n", &redoCount) != 1) {
		printf("무르기 취소 정보를 읽을 수 없습니다.");
		Sleep(3000);
		return 0;
	}

	for (int i = 0; i < redoCount; i++) {
		if (fscanf(file, "%d %d\n", &redoHistory[i][0], &redoHistory[i][1]) != 2) {
			printf("무르기 취소 정보 로드 실패");
			fclose(file);
			Sleep(3000);
			return 0;
		}
	}

	fclose(file);
	printf("게임 로드 중...");
	Sleep(3000);
	return 1;
}

// 저장된 게임이 있는지 체크
int isSaveFileExists() {
	FILE* file = fopen(SAVE_FILE, "r");
	if (file == NULL) return 0; // 파일 없음
	fclose(file);

	return 1; // 파일 있음
}

// 저장된 게임 삭제
void deleteSaveFile() {
	if (remove(SAVE_FILE) == 0) {
		printf("삭제 완료!");
		Sleep(3000);
	}
	else {
		printf("삭제 실패(저장된 게임이 없음)");
		Sleep(3000);
	}
}

// 무르기
void  undoMove(char board[ROW][COL], int* turn, int* countBlack, int* countWhite) {
	if (*turn == 0 || undoCount == 0) {
		printf("무를 수 있는 수가 없습니다.");
		Sleep(3000);
		return;
	}

	int lastIdx = undoCount - 1;
	int x = undoHistory[lastIdx][0];
	int y = undoHistory[lastIdx][1];

	board[y][x] = '+';
	undoCount--;

	if (*turn % 2 == 0) (*countWhite)--;
	else (*countBlack)--;

	(*turn)--;

	// 어떤 돌을 물렀는지 스택에 저장
	redoHistory[redoCount][0] = x;
	redoHistory[redoCount][1] = y;
	redoCount++;

	printf("무르기 완료!");
	Sleep(3000);
}

// 무르기 취소
void redoMove(char board[ROW][COL], int* turn, int* countBlack, int* countWhite) {
	if (redoCount == 0) {
		printf("무르기를 취소할 수 있는 돌이 없습니다.");
		Sleep(3000);
		return;
	}

	int lastIdx = redoCount - 1;
	int x = redoHistory[lastIdx][0];
	int y = redoHistory[lastIdx][1];

	if (*turn % 2 == 0) {
		board[y][x] = '@';
		(*countBlack)++;
	}
	else {
		board[y][x] = 'o';
		(*countWhite)++;
	}

	redoCount--;
	(*turn)++;

	undoHistory[undoCount][0] = x;
	undoHistory[undoCount][1] = y;
	undoCount++;

	printf("무르기 취소 완료!");
	Sleep(3000);
}

int main() {
	char board[ROW][COL];
	int turn = 0, countBlack = 0, countWhite = 0;

	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++)
			board[i][j] = '+';
	}

	if (isSaveFileExists()) {
		printf("저장된 게임이 있습니다.\n");
		printf("1. 새 게임 시작\n");
		printf("2. 저장된 게임 로드\n");
		printf("선택 (1 or 2): ");

		int choice = readInt(1, 2);
		if (choice == 2) loadGame(board, &turn, &countBlack, &countWhite);
	}

	const char* printFirstMenu[] = { "종료", "돌 놓기", "개수 확인", "게임 저장", "게임 로드", "저장 삭제", };

	while (1) {
		static Sequence rowT[ROW], colT[COL];
		static Sequence rightDiaT[ROW + COL - 1], leftDiaT[ROW + COL - 1];

		system("cls");
		processRowsThreat(board, rowT);
		processColsThreat(board, colT);
		processDiagonalThreat(board, rightDiaT, leftDiaT);
		printBoardHighlight(board, NULL, 0, NULL, 0, rowT, ROW, colT, COL, rightDiaT, ROW + COL - 1, leftDiaT, ROW + COL - 1, turn);

		for (int i = 0; i < (int)COUNT(printFirstMenu); i++)
			printf("%d. %s\n", i + 1, printFirstMenu[i]);
		printf("원하시는 옵션의 숫자를 입력해주세요 (무르기: U / 무르기 취소: R): ");

		int firstNum = readInt(1, (int)COUNT(printFirstMenu));
		if (firstNum == -999) {
			undoMove(board, &turn, &countBlack, &countWhite);
			continue;
		}
		if (firstNum == -998) {
			redoMove(board, &turn, &countBlack, &countWhite);
			continue;
		}

		if (firstNum == 0) {
			printf("없는 옵션입니다."); Sleep(3000); continue;
		}

		switch (firstNum) {
		case 1:
			printf("\n프로그램 종료\n");
			return 0;
		case 2:
			handlePlaceStone(board, &turn, &countBlack, &countWhite);
			break;
		case 3:
			handleAnalyzeMenu(board);
			break;
		case 4:
			saveGame(board, turn, countBlack, countWhite);
			break;
		case 5:
			loadGame(board, &turn, &countBlack, &countWhite);
			break;
		case 6:
			deleteSaveFile();
			break;
		}
	}
	return 0;
}