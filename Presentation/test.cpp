while (true)
{
    turn = j;             
                                turn = i;
                                flag[j] = ture;
                                while (flag[i] && turn == i)
                                    ;
    flag[i] = true;
    while (flag[j] && turn == j)
        ;
    /*critical section*/
    flag[i] = false;
    /*remainder section*/
}

