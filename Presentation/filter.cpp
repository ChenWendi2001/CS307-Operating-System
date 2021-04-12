// initialization
level[N] = { -1 };     // current level of processes 0...N-1
waiting[N-1] = { -1 }; // the waiting process of each level 0...N-2

// code for process #i
for(int l = 0; l < N-1; ++l) {
    level[i] = l;
    waiting[l] = i;
    while(waiting[l] == i &&
          (there exists k ≠ i, such that level[k] ≥ l)) {
        // busy waitS
    }
}

// critical section

level[i] = -1; // exit section

