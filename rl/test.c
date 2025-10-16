#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ROW				( 4 )							// 
#define ROWMAX			( ROW - 1 )
#define COL				( 5 )							// 
#define COLMAX			( COL - 1 )
#define STATES			( ROW * COL )
#define LAST_STATE		( STATES - 1 )
#define EPISODES		( 1000 )
#define ALPHA			( 0.1 )							// learning rate
#define GAMMA			( 0.9 )							// 
/*
GAMMA = 0: The agent is short-sighted, it only cares about immediate rewards.
GAMMA = 1: The agent is far-sighted,it considers all future rewards equally.
0 < GAMMA < 1: This is the sweet spot. The agent balances short-term and long-term rewards.
*/
#define EPSILON 0.1


#define ACTION_UP		( 0 )
#define ACTION_RIGHT	( 1 )
#define ACTION_DOWN		( 2 )
#define ACTION_LEFT		( 3 )
#define ACTIONS			( 4 )



int holes[] = {5, 6, 7, 8, 13, 17};						// 


double Q[STATES][ACTIONS];
int R[STATES][ACTIONS];


int is_hole(int state)
{
	int i = 0;
	
	for (i = 0; i < sizeof(holes)/sizeof(holes[0]); i++) {
		if (holes[i] == state){
			return 1;
		}
	}
	return 0;
}

static int STATE2ROW(int state)
{
	return state / COL;
}

static int STATE2COL(int state)
{
	return state % COL;
}

// Simulate environment transition
int get_next_state(int state, int action)
{
	int row = STATE2ROW(state);
	int col = STATE2COL(state);
	
	
	switch (action) {
	case ACTION_UP:
		if (row > 0){
			row--;
		}
		break;
	case ACTION_RIGHT: 
		if (col < COLMAX){
			col++;
		}
		break;
	case ACTION_DOWN:
		if (row < ROWMAX){
			row++;
		}
		break;
	case ACTION_LEFT:
		if (col > 0){
			col--;
		}
		break;
	}

	return row * COL + col;
}

int get_valid_action(int state, int action[])
{
	int row = STATE2ROW(state);
	int col = STATE2COL(state);
	int i;
	int valid_sum = 0;
	
	for(i = 0; i < ACTIONS; i++){
		action[i] = 1;
	}
	
	if( row == 0 ){
		action[ACTION_UP] = 0;
	} else if( row == ROWMAX ){
		action[ACTION_DOWN] = 0;
	}
	
	if( col == 0 ){
		action[ACTION_LEFT] = 0;
	} else if( col == COLMAX ){
		action[ACTION_RIGHT] = 0;
	}
	
	for(i = 0; i < ACTIONS; i++){
		valid_sum += action[i];
	}
	
	return valid_sum;
}

// Choose action using epsilon-greedy
int choose_action(int state)
{
	int action;
	int valid_action[ACTIONS];
	int valid_action_num;
	
	valid_action_num = get_valid_action(state, valid_action);
	
	if ((double)rand() / RAND_MAX < EPSILON) {
		int choose = rand() % valid_action_num;
		int counter = 0;
		int i;
		
		for(i = 0; i < ACTIONS; i++){
			counter += valid_action[i];
			if( (choose + 1) == counter ){
				action = i;
				break;
			}
		}
		
	} else {
		double max_q;
		int is_init = 0;
		int a;
		for (a = 0; a < ACTIONS; a++) {
			if( valid_action[a] == 1 ){
				if(is_init == 0){
					max_q = Q[state][a];
					action = a;
					is_init = 1;
				} else {
					if (Q[state][a] > max_q) {
						max_q = Q[state][a];
						action = a;
					}
				}
			}
		}
	}
	
	return action;
}

void print_q_table()
{
	printf("Q-table:\n");
	for (int s = 0; s < STATES; s++) {
		printf("State %2d: ", s);
		for (int a = 0; a < ACTIONS; a++) {
			printf("%06.2f  ", Q[s][a]);
		}
		printf("\n");
	}
}


void print_best_path()
{
	int state = 0;
	printf("Best path from state 0:\n");
	printf("%d", state);

	while (state != LAST_STATE) {
		int best_action = 0;
		double max_q = Q[state][0];

		// Find the action with the highest Q-value
		for (int a = 1; a < ACTIONS; a++) {
			if (Q[state][a] > max_q) {
				max_q = Q[state][a];
				best_action = a;
			}
		}

		int next_state = get_next_state(state, best_action);
		if (next_state == state) {
			// Stuck? No progress? Break to avoid infinite loop
			printf(" -> (stuck at %d)", state);
			break;
		}

		printf(" -> %d", next_state);
		state = next_state;
	}
	printf("\n");
}



void init_rewards()
{
	for (int s = 0; s < STATES; s++) {
		for (int a = 0; a < ACTIONS; a++) {
			int next = get_next_state(s, a);
			if (is_hole(next)) {
				R[s][a] = 0;
			} else if (next == LAST_STATE) {
				R[s][a] = 1;
			} else {
				R[s][a] = 0;
			}
		}
	}
}


void train()
{
	for (int ep = 0; ep < EPISODES; ep++) {
		int state = 0;
		
		while (state != LAST_STATE) {
			int action = choose_action(state);
			int next = get_next_state(state, action);
			printf("%d  %d\n", state, action);
			double max_q = Q[next][0];
			for (int i = 1; i < ACTIONS; i++) {
				if (Q[next][i] > max_q) max_q = Q[next][i];
			}
			Q[state][action] += ALPHA * (R[state][action] + GAMMA * max_q - Q[state][action]);
			state = next;
		}
		
		getchar();
	}
}


int main()
{
	srand( (unsigned int)time(NULL) );
	
	init_rewards();
	train();
	
	print_q_table();
	print_best_path();

	return 0;
}

/*

Q-table:
State  0: 040.46  048.46  -97.22  038.85
State  1: 044.04  054.95  -97.75  040.20
State  2: 051.67  062.17  -90.15  040.98
State  3: 055.91  070.19  -94.19  051.36
State  4: 066.08  067.77  079.10  059.64
State  5: 000.00  000.00  000.00  000.00
State  6: 000.00  000.00  000.00  000.00
State  7: 000.00  000.00  000.00  000.00
State  8: 000.00  000.00  000.00  000.00
State  9: 054.96  071.96  089.00  -94.19
State 10: 000.00  000.00  000.00  000.00
State 11: 000.00  000.00  000.00  000.00
State 12: 000.00  000.00  000.00  000.00
State 13: 000.00  000.00  000.00  000.00
State 14: 068.39  082.36  100.00  -91.14
State 15: 000.00  000.00  000.00  000.00
State 16: 000.00  000.00  000.00  000.00
State 17: 000.00  000.00  000.00  000.00
State 18: 000.00  000.00  000.00  000.00
State 19: 000.00  000.00  000.00  000.00
Best path from state 0:
0 -> 1 -> 2 -> 3 -> 4 -> 9 -> 14 -> 19
*/


