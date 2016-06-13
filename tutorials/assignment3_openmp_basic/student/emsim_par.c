#include "emsim.h"
#include <omp.h>

// play all (36) group games
void playGroups(team_t* teams) {
  static const int cNumTeamsPerGroup = NUMTEAMS / NUMGROUPS;
  int g, i, j, goalsI, goalsJ;

  #pragma omp parallel num_threads(NUMGROUPS)
  {
    #pragma omp for private (i, j, goalsI, goalsJ) schedule (dynamic)
    for (g = 0; g < NUMGROUPS; ++g) {
      for (i =  g * cNumTeamsPerGroup; i < (g+1) * cNumTeamsPerGroup; ++i) {
        for (j = (g+1) * cNumTeamsPerGroup - 1; j > i; --j) {
  
          // team i plays against team j in group g
          playGroupMatch(g, teams + i, teams + j, &goalsI, &goalsJ);
          teams[i].goals += goalsI - goalsJ;
          teams[j].goals += goalsJ - goalsI;
          if (goalsI > goalsJ)
            teams[i].points += 3;
          else if (goalsI < goalsJ)
            teams[j].points += 3;
          else {
            teams[i].points += 1;
            teams[j].points += 1;
          }
        }
      }
    }
  }
}

// play a specific final round
void playFinalRound(int numGames, team_t** teams, team_t** successors){
  team_t* team1;
  team_t* team2;
  int i, goals1 = 0, goals2 = 0;

  team_t* tmpSuccessors[numGames];

  #pragma omp parallel num_threads(numGames) firstprivate(goals1, goals2)
  {
    #pragma omp for private (team1, team2) schedule (dynamic)
    for (i = 0; i < numGames; ++i) {
      team1 = teams[i*2];
      team2 = teams[i*2+1];
      playFinalMatch(numGames, i, team1, team2, &goals1, &goals2);

      if (goals1 > goals2)
        tmpSuccessors[i] = team1;
      else if (goals1 < goals2)
        tmpSuccessors[i] = team2;
      else {
        playPenalty(team1, team2, &goals1, &goals2);
        if (goals1 > goals2)
          tmpSuccessors[i] = team1;
        else
          tmpSuccessors[i] = team2;
      }
    }
  }

  for (i=0; i < numGames; ++i)
	successors[i] = tmpSuccessors[i];
}
