#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <stdbool.h>

bool is_prime(int num) {
    if (num < 2) return false;// 0 and 1 execluded
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}


void find_first_n_primes(int n) {
    int counter = 0,number = 2;   
    while (counter < n) {
        if (is_prime(number)) {
          //  printf("%d ", number); 
            counter++;}
        number++;}
    printf("\n");
}

int main(int argc, char **argv) {
    int rank, size, n;
    double Sstart, Pstart ,Send,Pend, Stime, Ptime, speedup;//to track time
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf(" %d primes will be found\n", size);
    }

    n = size;
    int prime = -1; //unassigned prime for each process
    if (rank == 0) {
        // 2 is the first prime
        prime = 2;
        int x = 2;
        
        Pstart = MPI_Wtime();
        // Send non-multiples of 2 up to n
        for (int i = 3; i <= n * n; i++) {  // n*n so we can have more numbers
            if (i % 2 != 0) {
                MPI_Send(&i, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            }
        }

        int terminator = -1;
        MPI_Send(&terminator, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
   //     printf("Process 0 found %d as prime number\n",prime);

        Pend = MPI_Wtime();
        Ptime = Pend - Pstart;
        printf("\nParallel programming:\n");
        printf("execution time: %f\n", Ptime);
        Sstart = MPI_Wtime();
        find_first_n_primes(n);
        Send = MPI_Wtime();
        Stime = Send - Sstart;
        printf("Sequential programming:\n");
        printf("execution time: %f\n", Stime);
        printf("SpeedUp: %f\n", Stime/Ptime);
        printf("Efficienct: %f\n", ((Stime/Ptime)/size)*100);
    } else {
        int x;
        MPI_Recv(&x, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        prime = x;  

        int number;
        while (1) {
            MPI_Recv(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (number == -1) {  //to send terminator
                if (rank < size - 1) {
                    MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                }
                break;
            }
            if (number % prime != 0) {  //remove multiple of the prime
                if (rank < size - 1) {
                    MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                }
            }
        }
      //  printf("Process %d found %d as prime number\n", rank, local_prime);
    }

    MPI_Finalize();

    return 0;
}





