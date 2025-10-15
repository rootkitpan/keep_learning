#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ROW				( 5 )
#define ROWMAX			( ROW - 1 )
#define COL				( 4 )
#define COLMAX			( COL - 1 )
#define STATES			( ROW * COL )
#define LAST_STATE		( STATES - 1 )
#define ACTIONS			( 4 )
#define EPISODES 1000
#define ALPHA			( 0.1 )		// learning rate
#define GAMMA			( 0.9 )
/*
GAMMA = 0: The agent is short-sighted, it only cares about immediate rewards.
GAMMA = 1: The agent is far-sighted,it considers all future rewards equally.
0 < GAMMA < 1: This is the sweet spot. The agent balances short-term and long-term rewards.
*/

#define EPSILON 0.1

double Q[STATES][ACTIONS];

static int STATE2ROW(int state)
{
	return state / ROW;
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
	case 0: if (row > 0) row--; break;	   // up
	case 1: if (col < COLMAX) col++; break;	   // right
	case 2: if (row < ROWMAX) row++; break;	   // down
	case 3: if (col > 0) col--; break;	   // left
	}

	return row * COL + col;
}

// Reward function
double get_reward(int state)
{
	return (state == LAST_STATE) ? 1.0 : 0.0;
}

// Choose action using epsilon-greedy
int choose_action(int state)
{
	int action;
	int is_random;
	
	if ((double)rand() / RAND_MAX < EPSILON) {
		action = rand() % ACTIONS;
		is_random = 1;
	} else {
		action = 0;
		is_random = 0;
		double max_q = Q[state][0];
		for (int a = 1; a < ACTIONS; a++) {
			if (Q[state][a] > max_q) {
				max_q = Q[state][a];
				action = a;
			}
		}
	}
	
	//printf("[choose_action] action = %d, is_random = %d\n", action, is_random);
	return action;
}

void print_q_table()
{
	printf("Q-table:\n");
	for (int s = 0; s < STATES; s++) {
		printf("State %2d: ", s);
		for (int a = 0; a < ACTIONS; a++) {
			printf("%.2f ", Q[s][a]);
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

int main()
{
	srand( (unsigned int)time(NULL) );

	for (int episode = 0; episode < EPISODES; episode++) {
		int state = 0;

		while (state != LAST_STATE) {
			int action = choose_action(state);
			int next_state = get_next_state(state, action);
			double reward = get_reward(next_state);

			// Q-learning update
			double max_next_q = Q[next_state][0];
			for (int a = 1; a < ACTIONS; a++) {
				if (Q[next_state][a] > max_next_q) {
					max_next_q = Q[next_state][a];
				}
			}

			Q[state][action] += ALPHA * (reward + GAMMA * max_next_q - Q[state][action]);
			state = next_state;
		}
	}
	
	print_q_table();
	
	print_best_path();

	return 0;
}

/*

State  0: 0.46 0.59 0.31 0.50
State  1: 0.54 0.66 0.45 0.49
State  2: 0.63 0.73 0.56 0.56
State  3: 0.66 0.70 0.81 0.63
State  4: 0.47 0.00 0.00 0.00
State  5: 0.57 0.06 0.00 0.00
State  6: 0.66 0.28 0.06 0.05
State  7: 0.70 0.71 0.90 0.56
State  8: 0.03 0.00 0.00 0.00
State  9: 0.04 0.00 0.00 0.00
State 10: 0.45 0.17 0.00 0.00
State 11: 0.77 0.83 1.00 0.22
State 12: 0.00 0.00 0.00 0.00
State 13: 0.00 0.00 0.00 0.00
State 14: 0.00 0.00 0.00 0.00
State 15: 0.00 0.00 0.00 0.00

*/


