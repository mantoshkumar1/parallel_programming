#include "emsim.h"
#include <pthread.h>
  
//way to pass args to custom function
typedef struct pthread_arguments
{
   int i;
   team_t* teams;
   int cTeamsPerGroup;
   team_t** first;
   team_t** second;
   team_t** third;
}pthread_args;

typedef struct final_round_arguments
{
   int i;
   int numGames;
   int gameNo;
   team_t* team1;
   team_t* team2;
   int* goals1;
   int* goals2;
   team_t** successors;
}final_round_args;
void * custom_final_round(void *ptr);

//each thread will call this function
void * custom_func(void *ptr)
{
   pthread_args *var = (pthread_args *)ptr;
   playGroup(var->i, var->teams, var->cTeamsPerGroup, var->first, var->second, var->third);
   //playGroup(int groupNo, team_t* teams, int cTeamsPerGroup, team_t** first, team_t** second, team_t** third);
   //pthread_exit(NULL);
   return NULL;
}

void playEM(team_t* teams, int numThreads) {
  static const int cInitialNumSuccessors = NUMGROUPS * 2 + NUMTHIRDS;
  static const int cTeamsPerGroup = NUMTEAMS / NUMGROUPS;
  team_t* successors[NUMGROUPS * 2 + NUMTHIRDS] = {0};
  team_t* bestThirds[NUMGROUPS]; //groups = 6, teams = 24, per group 4 teams
  int g, curBestThird = 0, numSuccessors = cInitialNumSuccessors;

  //thread stuffs
  pthread_t thread[4];
  int i;
  pthread_args vars[4];

  // play groups
  initialize();


  // 1 group will be managed by each four thread, main can manage rest 2 groups
  
  for(i=0; i<4;i++)
  {
      vars[i].i = i;
      vars[i].teams = teams + (i * cTeamsPerGroup);
      vars[i].cTeamsPerGroup = cTeamsPerGroup;
      vars[i].first = successors + i * 2; 
      vars[i].second = successors + (numSuccessors - (i * 2) - 1);
      vars[i].third = bestThirds + i;
      pthread_create(&thread[i], NULL, &custom_func, &vars[i]);
  }

  //for (g = 0; g < NUMGROUPS; ++g) {
  for (g = 4; g < NUMGROUPS; ++g) {
    playGroup(g,
              teams + (g * cTeamsPerGroup),
              cTeamsPerGroup,
              successors + g * 2,
              successors + (numSuccessors - (g * 2) - 1),
              bestThirds + g);
  }

  for(i=0; i<4;i++)
    pthread_join(thread[i], NULL);

  // fill best thirds
  sortTeams(NUMGROUPS, bestThirds);
  for (g = 0; g < numSuccessors; ++g) {
    if (successors[g] == NULL) {
      successors[g] = bestThirds[curBestThird++];
    }
  }

  // play final rounds
  while (numSuccessors > 1) {
    playFinalRound(numSuccessors / 2, successors, successors);
    numSuccessors /= 2;
  }
}

void playFinalRound(int numGames, team_t** teams, team_t** successors) {
  //team_t* team1M;
  //team_t* team2M;
  //int i, 
  //int goals1M = 0, goals2M = 0;
  int i, goals1[numGames], goals2[numGames];
  //printf("NUMBER OF GAMES = %d \n", numGames); // 8, 4, 2, 1
  pthread_t thread[numGames];
  final_round_args vars[numGames];
 

  //printf("hello 1");
  for (i = 0; i < numGames; i++) {
      //if(i == 4) break;
      vars[i].i = i;
      vars[i].numGames = numGames;
      vars[i].team1 = teams[i*2];
      vars[i].team2 = teams[i*2+1];
      goals1[i] = 0; goals2[i] = 0;
      vars[i].goals1 = &goals1[i];
      vars[i].goals2 = &goals2[i];
      vars[i].successors = successors;
      pthread_create(&thread[i], NULL, &custom_final_round, &vars[i]);
  }
  //printf("hello 2");

  for(i=0; i<numGames;i++) {
    pthread_join(thread[i], NULL);
  }
/*
  if(numGames > 4) {
  for (i = 4; i < numGames; ++i) {
    team1M = teams[i*2];
    team2M = teams[i*2+1];
    playFinalMatch(numGames, i, team1M, team2M, &goals1M, &goals2M);

    if (goals1M > goals2M)
      successors[i] = team1M;
    else if (goals1M < goals2M)
      successors[i] = team2M;
    else
    {
      playPenalty(team1M, team2M, &goals1M, &goals2M);
      if (goals1M > goals2M)
        successors[i] = team1M;
      else
        successors[i] = team2M;
    }
  }
  } */
  /*
  if(numGames > 4) {
	  for (i = 4; i < numGames; ++i) {
	      vars[i].i = i;
	      vars[i].numGames = numGames;
	      vars[i].team1 = teams[i*2];
	      vars[i].team2 = teams[i*2+1];
	      goals1[i] = 0; goals2[i] = 0;
	      vars[i].goals1 = &goals1[i];
	      vars[i].goals2 = &goals2[i];
	      vars[i].successors = successors;
	      pthread_create(&thread[i], NULL, &custom_final_round, &vars[i]);
	  }

	  for (i = 4; i < numGames; ++i) {
              pthread_join(thread[i], NULL);
	  }
  }
*/
}

//each thread will call this function
void * custom_final_round(void *ptr)
{
   final_round_args *var = (final_round_args *)ptr;
   int goals1;
   int goals2;

   playFinalMatch(var->numGames, var->i, var->team1, var->team2, var->goals1, var->goals2);
  // printf("test 1");
   goals1 = *(var->goals1);
   goals2 = *(var->goals2);

   if (goals1 > goals2)
     var->successors[var->i] = var->team1;
   else if (goals1 < goals2)
     var->successors[var->i] = var->team2;
   else
   {
//   printf("test 2");
     playPenalty(var->team1, var->team2, var->goals1, var->goals2);
//   printf("test 3");
     goals1 = *(var->goals1);
     goals2 = *(var->goals2);
     if (goals1 > goals2)
       var->successors[var->i] = var->team1;
     else
       var->successors[var->i] = var->team2;
   }
   return NULL;
}
//void playFinalMatch(int numGames, int gameNo,
  //                  team_t* team1, team_t* team2, int* goals1, int* goals2) {
