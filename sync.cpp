#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <semaphore.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <cstdlib>

using namespace std;

int const *pN;
int N;
const char* folder= "Kenneth OS Project";
const char* filepath= "Kenneth OS Project/log.txt";
ofstream outputFile;

struct Buffer
{
    char buffer[5];
};

bool folderexists(const char *filepath)
{
    struct stat info;
    return stat(filepath,&info) ==0 && S_ISDIR(info.st_mode);
}

int create_shared_memory()
{
    key_t key = ftok("/tmp", 'A');
    int shmid = shmget(key, sizeof(Buffer), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(1);
    }
    return shmid;
}

int r1, l1 = 0, l2 = -1;
int count1 = 0, nWriting = 0;

sem_t s1, s2, s3, s4, s5;

pthread_t *prodlist, *conslist;

int shmid = create_shared_memory();

void *produce(void *arg)
{
    char ent;
    Buffer *sharedMemory = (Buffer *)shmat(shmid, NULL, 0);
    if (sharedMemory == (void *)-1)
    {
        perror("shmat");
        exit(1);
    }

    pthread_t thrid = pthread_self();
    cout << "\nProducer Created. \n"
         << endl;
    int k;

    while (count1 < nWriting)
    {
        sem_wait(&s3); // Lock s3 to allow only one producer at once
        if(count1>=nWriting)
        {
            sem_post(&s3);
            pthread_exit(NULL);
        }else
        sem_wait(&s2); // Reduce number of free spaces that can be produced,
                       // if none wait

        count1++;
        sleep(rand() % 100 * 0.01);
        int c = int(rand() % 26);
        ent = char(65 + c);
        k = 0;
        while ((*(prodlist + k)) != thrid)
            k++;
        cout << "\nIteration "<<count1<<": Producer " << k + 1 << " Made " << ent << " in position " << l1 % 5 << endl;
        outputFile<< "\nIteration "<<count1<<": Producer " << k + 1 << " Made " << ent << " in position " << l1 % 5 << endl;
        sharedMemory->buffer[l1++ % 5] = ent;

        
        sem_post(&s1); // Increase number of data to be consumed, if already
                       // waiting thread, free it also
        sem_post(&s3); // Open s3 to allow other producers
        sleep(rand() % 100 * 0.01);
    }
    pthread_exit(NULL);
}

// Consumer SectionS
void *consume(void *arg)
{
    char data;
    Buffer *sharedMemory = (Buffer *)shmat(shmid, NULL, 0);
    if (sharedMemory == (void *)-1)
    {
        perror("shmat");
        exit(1);
    }

    // Record creation of thread and its thread id
    pthread_t thrid = pthread_self();
    cout << "\nConsumer Created. \n"
         << endl;
    int s2val,k;

    while (l2< (l1 - 1))
    {
        int tempval;
        sem_getvalue(&s4,&tempval);
        sem_wait(&s4); // Lock door to allow only one consumer process at a time
        if(count1>=nWriting && tempval<=0)
        {
            sem_post(&s4);
            pthread_exit(NULL);
        }
        else
        sem_wait(&s1); // Reduce number of data that needs to be consumed,
                       // if none i.e less than or equals 0 wait
        

        sleep(rand() % 100 * 0.01);
        data = sharedMemory->buffer[++l2 % 5];
        //sem_getvalue(&s2, &s2val);
        k = 0;
        while ((*(conslist + k)) != thrid)
            k++;
        cout << "\nReader " << k + 1 << " Consumed " << data << " from position " << (l2) % 5 << endl;
        outputFile<<"\nReader " << k + 1 << " Consumed " << data << " from position " << (l2) % 5 << endl;
        sem_post(&s2); // Increase number of free data spaces that can be produced
                       // If any process waiting release also
        sem_post(&s4); // Open door to allow other consumers
        sleep(rand() % 100 * 0.01);
    }
    pthread_exit(NULL);
}

int main(int argv, char *argc[])
{
    if(!folderexists(folder))
    {
        int status= mkdir(folder,S_IRWXU| S_IRWXG| S_IROTH | S_IXOTH);
        if(!status)
            cout<<"Error Creating Folder"<<endl;
    }

    outputFile.open(filepath);
    if(outputFile.is_open())
    {
        outputFile.clear();
    }else{
        cout<<"Could not open output file"<<endl;
    }

    cout<<"Logs file is Created and stored in "<<filepath<<endl;
    int nProducers = 0, nConsumers = 0;

    // Collecting Input for variables
    while (nProducers <= 0 || nConsumers <= 0 || nWriting <= 10)
    {
        if (nProducers <= 0)
        {
            cout << "\nInput the number of producers: ";
            cin >> nProducers;
        }
        if (nConsumers <= 0)
        {
            cout << "\nInput number of consumers: ";
            cin >> nConsumers;
        }
        if (nWriting <= 10)
        {
            cout << "\nInput number of Iterations(>10): ";
            cin >> nWriting;
        }
    }

    cout << "\nNumber of producers= " << nProducers << endl;
    outputFile << "\nNumber of producers= " << nProducers << endl;
    cout << "Number of consumers= " << nConsumers << endl;
    outputFile << "Number of consumers= " << nConsumers << endl;
    cout << "Size of Buffer= " << 5 << endl;
    outputFile << "Size of Buffer= " << 5 << endl;
    cout << "Performing writing " << nWriting << " times " << endl;
    outputFile << "Performing writing " << nWriting << " times " << endl;

    const int *prodN, *consN;
    prodN = &nProducers;
    consN = &nConsumers;

    pthread_t *producer = new pthread_t[*prodN];
    prodlist = producer;
    pthread_t *consumer = new pthread_t[*consN];
    conslist = consumer;

    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,
                                PTHREAD_CREATE_JOINABLE);

    // semaphore initialization
    /*
     * Semaphore s1 to indicate the number of data that can be read
     * Semaphore s2 to indicate the number of free spaces that can be written
     * Semaphore s3 to only allow one producer write at a time
     * Semaphore s4 to allow one consumer at a time
     */
    sem_init(&s1, 0, 0);
    sem_init(&s2, 0, 5);
    sem_init(&s3, 0, 1);
    sem_init(&s4, 0, 1);

    
    int i = 0;
    // Creation of processes
    while (i < nProducers)
    {
        r1 = pthread_create(&producer[i], &attr,
                            produce, NULL);
        if (r1)
        {
            cout << "Error in creating thread" << endl;
            exit(-1);
        }
        i++;
    }

    i = 0;
    while (i < nConsumers)
    {
        r1 = pthread_create(&consumer[i], &attr,
                            consume, NULL);
        if (r1)
        {
            cout << "Error in creating thread" << endl;
            exit(-1);
        }
        i++;
    }

    pthread_attr_destroy(&attr);

    // Joining the thread
    for (i = 0; i < nProducers; i++)
    {
        r1 = pthread_join(producer[i], NULL);
        if (r1)
        {
            cout << "Error in joining thread" << endl;
            exit(-1);
        }
    }

    for (i = 0; i < nConsumers; i++)
    {
        r1 = pthread_join(consumer[i], NULL);
        if (r1)
        {
            cout << "Error in joining thread" << endl;
            exit(-1);
        }
    }
    
    outputFile.close();

    // Exiting thread
    pthread_exit(NULL);
    
    return 0;
}

// VARIABLE SIZE OF BUFFER