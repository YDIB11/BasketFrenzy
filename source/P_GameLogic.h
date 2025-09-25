#ifndef P_GAMELOGIC_H
#define P_GAMELOGIC_H

// Initializes game variables, spawns first ball, sets up timers
void Gameplay_Init(void);

// Runs the infinite loop that handles movement, freeze logic, etc.
void Gameplay_Loop(void);

#endif // P_GAMELOGIC_H
