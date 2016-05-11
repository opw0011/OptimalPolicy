#include"grid.h"

/* the possible directions for an action */
// N(-1, 0) S(1, 0) W(0, -1) E(0, 1)
const int direction_set[4][3][2] = {
  { 
    {-1, 0}, 
    {0, -1}, 
    {0, 1}
  },
  { 
    {1, 0}, 
    {0, -1}, 
    {0, 1}
  },
  { 
    {0, -1}, 
    {-1, 0}, 
    {1, 0}
  },
  { 
    {0, 1}, 
    {-1, 0},
    {1, 0} 
  }
};

/* probability for 3 possible directions */
const double proba_set[3] = {DIR1_PROBA, DIR2_PROBA, DIR2_PROBA};

/* direction names */
const char *dir_names_short[] = {"N", "S", "W", "E"};


void Grid::Intialize(void) {
  /*
   * add
   */
  for (int i = 0; i < nrow_; i++) {
    for (int j = 0; j < ncol_; j++) {
      // initial all the states value to zero
      states_[i][j].set_value(VALUE_DEFAULT);
      // initail all the policy to default = North
      states_[i][j].set_policy(POLICY_DEFAULT);
    }
  }
}

void Grid::PolicyIteration(void) {
  cout << endl << "Policy Iteration" << endl << endl;
  /* 
   * add
   */
  Intialize();  // initialize the grid
  int cnt = 0;


  cout << "iter: " << cnt << endl;
  print_values();
  print_policy();
  cout << endl;
  bool policy_stable = false;
  while(!policy_stable) {
    // policy evaluation
    PolicyEvaluation();
    // policy improvement
    policy_stable = PolicyImprovement();

    cnt++;
    cout << "iter: " << cnt << endl;
    print_values();
    print_policy();
    cout << endl;
  }
}


void Grid::ValueIteration(void) {
  Intialize();  // initialize the grid
  
  cout << endl << "Value Iteration" << endl << endl;
  int cnt = 0;
  double delta = 1;
  /*
   * add
   */
  cout << "iter: " << cnt << endl;
  print_values();
  cout << endl;
  while (delta >= theta_) {
    delta = 0;
    vector<StateRow> new_states = states_; // make a copy of the original states

    // for each states i,j
    for (int i = 0; i < nrow_; i++) {
      for (int j = 0; j < ncol_; j++) {
        // cout << "cor: " << i << "," << j << endl;
        State& current_state = new_states[i][j];
        double v = current_state.value(); // original state value

        double max_state_value = v;
        unsigned max_policy = 0;  // default = NORTH

        // for each Action, N S W E
        for(int dir = 0; dir <= 3; dir++) {
          Action action = current_state.get_action(dir);
          double new_state_value = 0.0;

          // calcualte the expected state value
          for(int l = 0; l < action.size(); l++) {
            NextState nextState = action[l];
            // cout << nextState.print_str();
            new_state_value += nextState.proba * (nextState.reward + gamma_ * get_state_value(nextState.id));
            // cout << "new_state_value: " << new_state_value << endl;
          }

          if(new_state_value > max_state_value){
            max_state_value = new_state_value;  
            max_policy = dir;    
          }
        }

        current_state.set_value(max_state_value); // set state value with the max_state_value with centain directions
        current_state.set_policy(max_policy);

        // cout << states_[i][j].value() << "//";
        double diff = abs(v - current_state.value());
        delta = (delta > diff) ? delta : diff; 
      }
    }

    // after the iterations, copy the new states
    states_ = new_states;

    /*
     * add
     */ 
    cnt++;
    cout << "iter: " << cnt << endl;
    cout << "delta: " << delta << endl;
    print_values();
    cout << endl;
    /*
     * add
     */
    // if(cnt >=24) break;
  }

  /* determine policy */
  
  /*
   * add
   */
  cout << "final policy: " << endl;
  print_policy();
  cout << endl;
}


void Grid::PolicyEvaluation(void) {
  /* 
   * add
   */
   double delta = 1;
   vector<StateRow> new_states = states_; // make a copy of the original states
   int count = 0;
   while (delta >= theta_) {
    delta = 0;

    for (int i = 0; i < nrow_; i++) {
      for (int j = 0; j < ncol_; j++) {
        State& current_state = new_states[i][j];
        double v = current_state.value(); // original state value

        // pick the stored policy
        unsigned selected_policy = current_state.policy();
        // cout << "SELECTED policy: " << selected_policy << endl;
        Action action = current_state.get_action(selected_policy);

        double new_state_value = 0.0;
        // calcualte the expected state value
        for(int l = 0; l < action.size(); l++) {
          NextState nextState = action[l];
          // cout << nextState.print_str();
          new_state_value += nextState.proba * (nextState.reward + gamma_ * get_state_value(nextState.id));
          // cout << "new_state_value: " << new_state_value << endl;
        }
        current_state.set_value(new_state_value); // update the state value

        double diff = abs(v - current_state.value());
        delta = (delta > diff) ? delta : diff; 
      }
    }

    // after the iterations, copy the new states
    states_ = new_states;

    count++;

    // print_values();
    // cout << "Interation: " << count << endl;
    // if (count >= 2) break;

   }
}

bool Grid::PolicyImprovement(void) {
  /*
   * add
   */
  bool stable = true;

  // for each states i,j
  for (int i = 0; i < nrow_; i++) {
    for (int j = 0; j < ncol_; j++) {
      State& current_state = states_[i][j];

      unsigned b = current_state.policy();  // store original policy

      // find the policy with the max state value
      double max_state_value = current_state.value();
      unsigned max_policy = 0;  // default = NORTH

      // for each Action, N S W E
      for(int dir = 0; dir <= 3; dir++) {
        Action action = current_state.get_action(dir);
        double new_state_value = 0.0;

        // calcualte the expected state value
        for(int l = 0; l < action.size(); l++) {
          NextState nextState = action[l];
          // cout << nextState.print_str();
          new_state_value += nextState.proba * (nextState.reward + gamma_ * get_state_value(nextState.id));
          // cout << "new_state_value: " << new_state_value << endl;
        }

        if(new_state_value > max_state_value){
          max_state_value = new_state_value;  
          max_policy = dir;    
        }
      }

      // if original policy not equal optimized policy
      if (b != max_policy) {
        stable = false;
        current_state.set_policy(max_policy); // update pi(s)
      }
    }
  }
  return stable;
}


Grid::Grid(unsigned row, unsigned col):
    nrow_(row), 
    ncol_(col), 
    a_(A_POS),
    ap_(AP_POS),
    b_(B_POS),
    bp_(BP_POS),
    a_reward_(AREWARD),
    b_reward_(BREWARD),
    theta_(THETA_DEFALT),
    gamma_(GAMMA)
{

  // initialize states
  for (int i = 0; i < nrow_; i++) {
    StateRow state_row;
    states_.push_back(state_row);
    //states_row_t states_row;
    for (int j = 0; j < ncol_; j++) {
      StateID current_id(i, j);
      vector<Action> actions;
      // for each state, genreate result for 4 different actions (NSWE)
      for (int k = 0; k < 4; k++) 
        actions.push_back(generate_action(k, current_id));
      
      State current_state(current_id, actions);
      states_.back().push_back(current_state);
    }
  }
}


/* generate result for an action (different next states/rewards/probabilities)*/
Action Grid::generate_action(unsigned dir, StateID current_id) const {
  Action action;

  double reward;
  double proba;

  if (current_id == a_) 
    action.push_back(NextState(ap_, 1, a_reward_));
  else if (current_id == b_) 
    action.push_back(NextState(bp_, 1, b_reward_));
  else {
    // a non-deterministic action may generate different results
    for (int i = 0; i < 3; i++) {
      int newrow =  current_id.row + direction_set[dir][i][0];
      int newcol =  current_id.col + direction_set[dir][i][1];
      
      if(isingrid(newrow, newcol)) 
        reward = 0;
      else {
        newrow = current_id.row;
        newcol = current_id.col;
        reward = -1;		
      }
      proba = proba_set[i];
      action.push_back(NextState(StateID(newrow, newcol), proba, reward));
    }
  }
  return action;
}


bool Grid::isingrid(int row, int col) const {
  return  (row >= 0) && (row < nrow_) && (col >= 0) && (col < ncol_);
}


void Grid::print(void) const {
  cout << "Grid size: (" << nrow_ << ", " << ncol_ << ")" << endl;
  cout << "Point A: " << a_.print_str() << " -> AP: " <<  ap_.print_str();
  cout << " reward: " << a_reward_ << endl;
  cout << "Point B: " << b_.print_str() << " -> BP: " <<  bp_.print_str();
  cout << " reward: " << b_reward_ << endl;
  cout << "theta: " << theta_<< endl;
  print_values();
  print_policy();
  cout << endl;
}


void Grid::print_values(void) const {
  cout << "values";
  cout << endl;
  for (int i = 0; i < nrow_; i++) {
    for (int j = 0; j < nrow_; j++) {
      cout << states_[i][j].value() << " ";
    }
    cout << endl;
  }
}


void Grid::print_policy(void) const {
  cout << "actions";
  cout << endl;
  for (int i = 0; i < nrow_; i++) {
    for (int j = 0; j < nrow_; j++) {
      cout << dir_names_short[states_[i][j].policy()] << " ";
    }
    cout << endl;
  }
}



