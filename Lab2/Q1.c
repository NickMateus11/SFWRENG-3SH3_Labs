#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

// global variable
int globalVar = 0;

// function 1
void *fun1(void *arg)
{
    // static and local variables
    static int staticVar = 0;
    int localVar = 0;
    
    // update variables
    globalVar++;
    staticVar++;
    localVar++;
    
    // show change in variables
    printf("Running Function 1: global variable = %d, static variable = %d, and local variable = %d\n", globalVar, staticVar, localVar);
}

// function 2
void *fun2(void *arg)
{
    static int staticVar = 0;
    int localVar = 0;
    
    // update variables
    globalVar++;
    staticVar++;
    localVar++;
    
    // show change in variables
    printf("Running Function 2: global variable = %d, static variable = %d, and local variable = %d\n", globalVar, staticVar, localVar);
}

int main()
{
    // create 5 threads
    pthread_t thr1, thr2, thr3, thr4, thr5;
    int err;
    
    // show initial status of global variable
    printf("All variables (global, static, and local) initially set to 0\n");
    
    // run 1st thread---------------------------------------------------------------------------------------------------------------
    printf("\nEXPERIMENT 1:\n");
    printf("Thread 1 executing...\n");
    err = pthread_create(&thr1, NULL, &fun1, NULL);
    if (err != 0)
        perror("Thread 1 couldn't be created!\n");
    pthread_join(thr1, NULL);
    printf("Thread 1 done executing\n");
    
    printf("Thread 1 executing...\n");
    err = pthread_create(&thr1, NULL, &fun1, NULL);
    if (err != 0)
        perror("Thread 1 couldn't be created!\n");
    pthread_join(thr1, NULL);
    printf("Thread 1 done executing\n");
    
    // run 2nd thread-----------------------------------------------------------------------------------------------------------------
    printf("Thread 2 executing...\n");
    err = pthread_create(&thr2, NULL, &fun2, NULL);
    if (err != 0)
        perror("Thread 2 couldn't be created!\n");
    pthread_join(thr2, NULL);
    printf("Thread 2 done executing\n");
    
    printf("Thread 2 executing...\n");
    err = pthread_create(&thr2, NULL, &fun2, NULL);
    if (err != 0)
        perror("Thread 2 couldn't be created!\n");
    pthread_join(thr2, NULL);
    printf("Thread 2 done executing\n");
    
    // run 3rd and 4th threads on same function EXCLUSIVELY--------------------------------------------------------------------------
    printf("\nEXPERIMNT 2:\n");
    printf("Thread 3 executing...\n");
    err = pthread_create(&thr3, NULL, &fun1, NULL);
    if (err != 0)
        perror("Thread 3 couldn't be created!\n");
    pthread_join(thr3, NULL);
    printf("Thread 3 done executing\n");
    
    printf("Thread 4 executing...\n");
    err = pthread_create(&thr4, NULL, &fun1, NULL);
    if (err != 0)
        perror("Thread 4 couldn't be created!\n");
    pthread_join(thr4, NULL);
    printf("Thread 4 done executing\n");
    
    // run 4th and 5th threads on same function CONCURRENTLY-------------------------------------------------------------------------
    printf("\nEXPERIMENT 3:\n");
    printf("Thread 4 executing...\n");
    err = pthread_create(&thr4, NULL, &fun1, NULL);
    if (err != 0)
        perror("Thread 4 couldn't be created!\n");
    
    printf("Thread 5 executing...\n");
    err = pthread_create(&thr5, NULL, &fun1, NULL);
    if (err != 0)
        perror("Thread 5 couldn't be created!\n");
    
    pthread_join(thr4, NULL);
    printf("Thread 4 done executing\n");
    pthread_join(thr5, NULL);
    printf("Thread 5 done executing\n");
    
    exit(0);
}

/*EXPLANATION: 

Global variables are accessible by any function in the process. Since threads share the memory of the process they detach from, 
they can access this same global variable. Local variables are accessible only within the function they are declared. Static variables
(declared locally) are similar to local variables, except they retain their value from the previous execution of the function.

What we have done in this program is create 1 global variable, 1 local static variable, and 1 local variable. We also created
two functions that update each of these variables. Each of these variables are type 'int' and are incremented when a function is executed.
We then created 5 threads to experiment with and demonstrate the differences between these variables as mentioned above:

experiment 1: execute function 1 with thread 1 twice, and execute function 2 with thread 2 twice.
OUTCOME: we see that global variable retains its value between all 4 executions going from 0 to 4.
         we see that static variable retains its value for each function between its 2 executions going from 0 to 2.
         we see that local variable is always the same (= 1) because it is reset each function call.
         
experiment 2: execute function 1 with threads 3 and 4 one after the other.
OUTCOME: we see that once again the global variable retains its value from the previous calls going from 4 to 6.
         we see that the static variable also retains its values from previous calls going from 2 to 4.
         we see that the local variable still remains as 1 (because it is reset each function call).
        
experiment 3: execute function 1 with threads 4 and 5 running concurrently.
OUTCOME: we see that once again the global variable retains its value from the previous calls going from 6 to 8.
         we see that the static variable also retains its values from previous calls going from 4 to 6.
         we see that the local variable still remains as 1 (because it is reset each function call).
*/
