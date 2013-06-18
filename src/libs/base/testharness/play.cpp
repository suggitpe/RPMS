//#############################

#include <testharness/si_testharness.hpp>
#include <pthread.h>
#include <utilities/st_conversion.hpp>


TEST_BASE;

void producerConsumerTest();
void simpleThreadTest();
void readerWriterTest();

//////////////////////////////////////////////////
void execute( int aArgC, char *aArgV[] )
{
    RPMS_TRACE(LOGNAME, "------------------------ START ---------------------");
    simpleThreadTest();
    
    RPMS_TRACE(LOGNAME, "----------------------------------------------------");
    //producerConsumerTest();

    RPMS_TRACE(LOGNAME, "----------------------------------------------------");
    //readerWriterTest();

    RPMS_TRACE(LOGNAME, "------------------------- END ----------------------");
}

//////////////////////////////////////////////////
// ********************** READER WRITER TEST *****
#define MAXCOUNT 5

#define READER1  50000
#define READER2 100000
#define READER3	400000
#define READER4 800000
#define WRITER1 150000

class rwl
{
    public:
        pthread_mutex_t *mut;
        int writers;
        int readers;
        int waiting;
        pthread_cond_t *writeOK;
        pthread_cond_t *readOK;

        rwl()
            : writers(0), readers(0), waiting(0)
        {
            RPMS_TRACE(LOGNAME, "Creating rwl");
            mut     = new pthread_mutex_t;
            writeOK = new pthread_cond_t;
            readOK  = new pthread_cond_t;
            pthread_mutex_init(mut, NULL);
            pthread_cond_init(writeOK, NULL);
            pthread_cond_init(readOK, NULL);
        }
        
        ~rwl()
        {
            pthread_mutex_destroy(mut);
            delete mut;
            pthread_cond_destroy(writeOK);
            delete writeOK;
            pthread_cond_destroy(readOK);
            delete readOK;
        }
};

class rwargs
{
    public:
    rwl *lock;
    int id;
    long delay;

    rwargs( rwl *l, int i, long d )
        : lock(l), id(i), delay(d)
    {
        RPMS_TRACE(LOGNAME, "creating rwargs " + rpms::convert<std::string>(i) );
    }
};

static int data = 1;
void *writer (void *args);
void *reader (void *args);
void readlock (rwl *lock, int d);
void writelock (rwl *lock, int d);
void readunlock (rwl *lock);
void writeunlock (rwl *lock);



//////////////////////////////////////////////////
void readerWriterTest()
{
    RPMS_INFO(LOGNAME, "**************************************");
    RPMS_INFO(LOGNAME, "********** readerWriterTest **********");
    RPMS_INFO(LOGNAME, "**************************************");

    pthread_t r1, r2, r3, r4, w1;
    rwargs *a1, *a2, *a3, *a4, *a5;
    rwl *lock = new rwl;
    
    RPMS_DEBUG(LOGNAME, ">>>> Creating objects ...");
    a1 = new rwargs (lock, 1, WRITER1);
    a2 = new rwargs (lock, 1, READER1);
    a3 = new rwargs (lock, 2, READER2);
    a4 = new rwargs (lock, 3, READER3);
    a5 = new rwargs (lock, 4, READER4);

    RPMS_DEBUG(LOGNAME, ">>>> Creating threads ...");
    pthread_create (&w1, NULL, writer, a1);
    pthread_create (&r1, NULL, reader, a2);
    pthread_create (&r2, NULL, reader, a3);
    pthread_create (&r3, NULL, reader, a4);
    pthread_create (&r4, NULL, reader, a5);
    
    RPMS_DEBUG(LOGNAME, ">>>> Joining threads ...");
    pthread_join (w1, NULL);
    pthread_join (r1, NULL);
    pthread_join (r2, NULL);
    pthread_join (r3, NULL);
    pthread_join (r4, NULL);

    delete a1;
    delete a2;
    delete a3;
    delete a4;
    delete a5;
}

//////////////////////////////////////////////////
void *reader (void *args)
{
    rwargs *a = static_cast<rwargs *>(args);
    int d;

    do
    {
        readlock (a->lock, a->id);
        d = data;
        usleep (a->delay);
        readunlock (a->lock);
        RPMS_DEBUG( LOGNAME, "Reader" + rpms::convert<std::string>(a->id) + ": data = " + rpms::convert<std::string>(d) );
        usleep (a->delay);
    } while (d != 0);
    RPMS_DEBUG( LOGNAME, "Reader" + rpms::convert<std::string>(a->id) + ": finished" );
    return 0;
}

//////////////////////////////////////////////////
void *writer (void *args)
{
    rwargs *a = static_cast<rwargs *>(args);

    for (int i = 2; i < MAXCOUNT; ++i) 
    {
        writelock (a->lock, a->id);
   	    data = i;
   	    usleep (a->delay);
        writeunlock (a->lock);
        RPMS_DEBUG( LOGNAME, "Writer " + rpms::convert<std::string>(a->id) + ": Wrote " + rpms::convert<std::string>(i));
        usleep (a->delay);
    }
    RPMS_DEBUG( LOGNAME, "Writer " + rpms::convert<std::string>(a->id) + ": finishing... " );
    writelock (a->lock, a->id);
    data = 0;
    writeunlock (a->lock);
    RPMS_DEBUG( LOGNAME, "Writer " + rpms::convert<std::string>(a->id) + ": finished" );

    return 0;
}

//////////////////////////////////////////////////
void readlock (rwl *lock, int d)
{
    pthread_mutex_lock (lock->mut);
    if (lock->writers || lock->waiting) 
    {
        do
        {
            RPMS_DEBUG( LOGNAME, "Reader " + rpms::convert<std::string>(d) + ": blocked");
            pthread_cond_wait (lock->readOK, lock->mut);
            RPMS_DEBUG( LOGNAME, "Reader " + rpms::convert<std::string>(d) + ": unblocked");
        } while (lock->writers);
    }
    lock->readers++;
    pthread_mutex_unlock (lock->mut);
}

//////////////////////////////////////////////////
void writelock (rwl *lock, int d)
{
    pthread_mutex_lock (lock->mut);
    lock->waiting++;
    while(lock->readers || lock->writers)
    {
        RPMS_DEBUG( LOGNAME, "Writer " + rpms::convert<std::string>(d) + ": blocked");
        pthread_cond_wait (lock->writeOK, lock->mut);
        RPMS_DEBUG( LOGNAME, "Writer " + rpms::convert<std::string>(d) + ": blocked");
    }
    lock->waiting--;
    lock->writers++;
    pthread_mutex_unlock (lock->mut);
}

//////////////////////////////////////////////////
void readunlock (rwl *lock)
{
    pthread_mutex_lock (lock->mut);
    lock->readers--;
    pthread_cond_signal (lock->writeOK);
    pthread_mutex_unlock (lock->mut);
}

//////////////////////////////////////////////////
void writeunlock (rwl *lock)
{
    pthread_mutex_lock (lock->mut);
    lock->writers--;
    pthread_cond_broadcast (lock->readOK);
    pthread_mutex_unlock (lock->mut);
}

//////////////////////////////////////////////////
// ********************** PRODUCER CONSUMER TEST *
#define Q_SIZE 10
#define LOOP 20
class queue
{
    public:
    int buf[Q_SIZE];
    long head;
    long tail;
    int full;
    int empty;
    pthread_mutex_t *mut;
    pthread_cond_t *notFull;
    pthread_cond_t *notEmpty;

    queue() 
        : empty(1), full(0), head(0), tail(0)
    {
        RPMS_INFO(LOGNAME, "creating queue ...");
        mut = new pthread_mutex_t;
        pthread_mutex_init(mut, NULL);
        notFull = new pthread_cond_t;
	    pthread_cond_init (notFull, NULL);
	    notEmpty = new pthread_cond_t;
	    pthread_cond_init (notEmpty, NULL);
    }

    virtual ~queue()
    {
        RPMS_INFO(LOGNAME, "destroying queue ...");
        pthread_mutex_destroy (mut);
	    delete mut;
	    pthread_cond_destroy (notFull);
	    delete notFull;
	    pthread_cond_destroy (notEmpty);
	    delete notEmpty;
    }
};
void *producer( void *args );
void *consumer( void *args );
void queueAdd( queue *q, int in );
void queueDel( queue *q, int *out );

//////////////////////////////////////////////////
void producerConsumerTest()
{
    RPMS_INFO(LOGNAME, "**************************************");
    RPMS_INFO(LOGNAME, "******** producerConsumerTest ********");
    RPMS_INFO(LOGNAME, "**************************************");
    RPMS_TRACE(LOGNAME, "Creating queue");
    pthread_t pro, con;
    queue * fifo = new queue;
    if( fifo == 0 )
    {
        RPMS_FATAL(LOGNAME, "Queue creation failed");
        exit (-1);
    }
    pthread_create( &pro, NULL, producer, fifo );
    pthread_create( &con, NULL, consumer, fifo );
    pthread_join(pro, NULL);
    pthread_join(con, NULL);
    delete fifo;
}

//////////////////////////////////////////////////
void *producer( void *args )
{
    queue *fifo;
    
    fifo = static_cast<queue*>(args);
    for( int i = 0; i < LOOP; ++i ) 
    {
        pthread_mutex_lock(fifo->mut);
        while( fifo->full )
        {
            RPMS_WARN(LOGNAME, "producer: queue FULL");
            pthread_cond_wait( fifo->notFull, fifo->mut );
        }
        queueAdd( fifo, i );
        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal (fifo->notEmpty);
        usleep(100000);
    }

    for( int i = 0; i < LOOP; ++i )
    {
        pthread_mutex_lock(fifo->mut);
        while( fifo->full )
        {
            RPMS_INFO(LOGNAME, "producer: queue FULL");
            pthread_cond_wait( fifo->notFull, fifo->mut );
        }
        queueAdd( fifo, i );
        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal (fifo->notEmpty);
        usleep(100000);
    }
    return 0;
}

//////////////////////////////////////////////////
void *consumer( void *args )
{
    queue *fifo;
    int d;

    fifo = static_cast<queue*>(args);
    for( int i = 0; i < LOOP; ++i )
    {
        pthread_mutex_lock(fifo->mut);
        while( fifo->empty )
        {
            RPMS_INFO(LOGNAME, "consumer: queue EMPTY");
            pthread_cond_wait(fifo->notEmpty, fifo->mut);
        }
        queueDel(fifo, &d);
        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notFull);
        RPMS_INFO(LOGNAME, "consumer: RECEIVED ");
        usleep(200000);
    }

    for( int i = 0; i < LOOP; ++i )
    {
        pthread_mutex_lock(fifo->mut);
        while( fifo->empty )
        {
            RPMS_INFO(LOGNAME, "consumer: queue EMPTY");
            pthread_cond_wait(fifo->notEmpty, fifo->mut);
        }
        queueDel(fifo, &d);
        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notFull);
        RPMS_INFO(LOGNAME, "consumer: RECEIVED ");
        usleep(500000);
    }
    
    return 0;
}

//////////////////////////////////////////////////
void queueAdd( queue *q, int in )
{
    q->buf[q->tail] = in;
    q->tail++;
    if( q->tail == Q_SIZE )
    {
        q->tail = 0;
    }

    if( q->head == q->tail )
    {
        q->full = 1;
    }

    q->empty = 0;
}

//////////////////////////////////////////////////
void queueDel( queue *q, int *out )
{
    *out = q->buf[q->head];
    q->head++;
    if( q->head == Q_SIZE )
    {
        q->head = 0;
    }
    
    if( q->head == q->tail )
    {
        q->empty = 1;
    }
    q->full = 0;
}

//////////////////////////////////////////////////

// ********************** SIMPLE THREAD TEST *****
#define NUM_THREADS 10
void *printHello(void *threadid);

//////////////////////////////////////////////////
void simpleThreadTest()
{
    RPMS_INFO(LOGNAME, "**************************************");
    RPMS_INFO(LOGNAME, "********** simpleThreadTest **********");
    RPMS_INFO(LOGNAME, "**************************************");
    pthread_t threads[NUM_THREADS];
    int rc;
    for( int t= 0; t < NUM_THREADS; ++t ) 
    {
        RPMS_INFO(LOGNAME, "Creating thread [" + rpms::convert<std::string>(t) + "]" );
        rc = pthread_create( &threads[t], NULL, printHello, static_cast<void *>(&t) );
        if(rc)
        {
            RPMS_ERROR(LOGNAME, "return code from pthread_create [" + rpms::convert<std::string>(rc) + "]");
            return;
        }
    }
    
    for( int i = 0; i < NUM_THREADS; ++i )
    {
        pthread_join( threads[i], NULL);
    }
    sleep(3);
}

//////////////////////////////////////////////////
void *printHello(void *threadid)
{
    sleep(2);
    RPMS_DEBUG(LOGNAME, "hello world " + rpms::convert<std::string>(threadid) );
    pthread_exit(NULL);
}


//////////////////////////////////////////////////
