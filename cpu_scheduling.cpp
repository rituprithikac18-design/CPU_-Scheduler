#include<iostream>
using namespace std;

int IDcount=0;

int randomInt(int Low, int High)
{
    int p;
    p= Low+(rand() %(High-Low));
    return p;
}

class processes{
    processes *nextproc,*prevproc;
    int ID,priority;
    int arrivalTime,startTime,finishTime,delay;
    int runTime,Turnaround,origRunTime;
    public:
    processes()
    {
        ID=IDcount++;
        arrivalTime=randomInt(1,10);
        Turnaround=finishTime=delay=startTime=-1;
        origRunTime=runTime=randomInt(1,10);
        priority=randomInt(0,3);
        nextproc=prevproc=NULL;
    }
    processes(int a)
    {
        ID=IDcount++;
        arrivalTime=a;
        Turnaround=finishTime=delay=startTime=-1;
        origRunTime=runTime=randomInt(1,10);
        priority=randomInt(0,3);
        nextproc=prevproc=NULL;
    }
    processes *getnext(){return nextproc;}
    processes *getprev(){return prevproc;}
    void setnext(processes *t);
    processes *getthis(){return this;}
    void setprev(processes *t){prevproc=t;}
    int getArrivalTime(){return arrivalTime;}
    int getrunTime(){return runTime;}
    int getpriority(){return priority;}
    void setpriority(int pr){priority=pr;}
    int getId(){return ID;}
    int getStartTime(){return startTime;}
    void starter(int a);
    void done(int fin);
    void reduceRuntime(int n){runTime-=n;}
    void display();
};

void processes:: setnext(processes *t)
{
    nextproc=t;
    if(t!=NULL)
    t->setprev(this);
}

void processes:: starter(int a)
{
    startTime=a;
    delay=startTime-arrivalTime;
    if(delay<0) delay=0;
}
void processes:: done(int fin)
{
    finishTime=fin;
    Turnaround=finishTime-startTime;
}

void processes:: display()
{
    cout<<"\nProcess ID= "<<ID<<endl;
    cout<<"Process Arrival Time= "<<arrivalTime<<endl;
    cout<<"Process Priority= "<<priority<<endl;
    if(startTime!=-1)
    {
        cout<<"Process Start Time= "<<startTime<<endl;
        cout<<"Process Finish Time= "<<finishTime<<endl;
        cout<<"Process Turnaround Time= "<<Turnaround<<endl;
        cout<<"Process Delay= "<<delay<<endl;
    }
    cout<<"Process Run Time Needed= "<<origRunTime<<endl;
    cout<<"Prev Process= "<<prevproc<<endl;
    cout<<"This Process= "<<this<<endl;
    cout<<"Next Process = "<<nextproc<<endl;
}

class slots{
    int time[2];
    processes *p;
    slots *next;
public:
    slots()
    {
        for(int i=0; i<2; i++) time[i]=-1;
        p=NULL;
        next=NULL;
    }
    slots(int start, int end, processes *t)
    {
        time[0]=start;
        time[1]=end;
        p=t;
        next=NULL;
    }
    void setstart(int a){time[0]=a;}
    void setend(int a){time[1]=a;}
    void settime(int a,int b){time[0]=a;time[1]=b;}
    void settime(int a[]){for(int i=0; i<2; i++) time[i]=a[i];}
    void setprocess(processes *t){p=t;}
    void setnext(slots *p){next=p;}
    slots* getnext(){return next;}
    void display();
};

void slots:: display()
{
    cout<<"\n"<<time[0]<<" - "<<time[1]<<": "<<p->getId()<<endl;
}

class timer{
    slots *head;
public:
    timer()
    {
        head=NULL;
    }
    timer(slots *s)
    {
        head=s;
    }
    void append(slots *s);
    void display();
};

void timer:: append(slots *s)
{
    slots *p=head;
    if(p==NULL) head=s;
    else {
        while(p->getnext()!=NULL) p=p->getnext();
        p->setnext(s);
    }
}

void timer:: display()
{
    slots *p=head;
    while(p!=NULL)
    {
        p->display();
        p=p->getnext();
    } 
}

class queuer{
    processes *head, *tail, *point;
    int len;
    public:
    queuer()
    {
        head=tail=NULL;
        len=0;
        point=head;
    }
    queuer(processes *p)
    {
        head=tail=p;
        len=1;
        point=head;
    }
    queuer(processes p[], int l)
    {
        head=tail=NULL;
        len=0;
        point=head;
        for(int i=0; i<l; i++) append(&p[i]);
    }
    int getLen(){return len;}
    void append(processes *p);
    void deleter(processes *t);
    int sethead(processes *p);
    void display();
    void updatetail();
    void inserter(processes *pr, int mode);
    void initsorter(int mode);
    void sorter(int mode);
    processes *gethead(){return head;}
};

void queuer:: append(processes *p)
    {
        if(head==NULL)
        {
            head=tail=p;
            head->setprev(NULL);
            p->setnext(NULL);
        }else{
            tail->setnext(p);
            tail=p;
            tail->setnext(NULL);
        }
        len++;
    }

void queuer:: deleter(processes *t)
    {
        processes *p=NULL;
        p=head;
        while(p!=NULL)
        {
            if(p==t) break;
            p=p->getnext();
        }
        if(p==head){
            sethead(p->getnext());
        }else{
            p->getprev()->setnext(p->getnext());
        }
        len--;
    }

int queuer:: sethead(processes *p)
    {
        if(p==NULL){head=NULL;return 0;}
        if(head->getnext()!=p)
        {
            p->setnext(head->getnext());
        }
        head=p;
        p->setprev(NULL);
        return 0;
    }

void queuer:: display()
    {
        processes *p=NULL;
        p=head;
        while(p!=NULL)
        {
            p->display();
            p=p->getnext();
        }
    }

void queuer:: updatetail()
    {
        processes *p=head;
        if(p==NULL) tail=NULL;
        else{
            while(p->getnext()!=NULL) {p=p->getnext();}
            if(tail!=p)
            {tail=p;
            tail->setnext(NULL);}
        }  
    }

void queuer:: inserter(processes *pr, int mode)
    {
        processes *p=head;
        switch(mode)
        {
            case 1:
                append(pr);
                break;
            case 2:
                for(int i=0; i<len; i++)
                {
                    if(p->getrunTime()>pr->getrunTime()) break;
                    p=p->getnext();
                }
                if(p!=NULL)
                {
                    if(p->getprev()!=NULL) p->getprev()->setnext(pr);
                    pr->setnext(p);
                    updatetail();
                    len++;
                }else{
                    append(pr);
                }
                break;
            case 3:
                append(pr);
                break;
            case 4:
                for(int i=0; i<len; i++)
                {
                    if(p->getpriority()>pr->getpriority()) break;
                    p=p->getnext();
                }
                if(p!=NULL)
                {
                    if(p->getprev()!=NULL) p->getprev()->setnext(pr);
                    pr->setnext(p);
                    updatetail();
                    len++;
                }else{
                    append(pr);
                }
                break;
            default:
                cout<<"\nDEFAULT CALLED"<<endl;
                break;
        }
    }

void queuer:: initsorter(int mode)
    {
        processes *p=head;
        int const* len1;
        len1=&len;
        processes **pr=new processes*[*len1];

        for(int i=0; i<len; i++)
        {
            pr[i]=p;
            p=p->getnext();
        }

        int n=0;
        processes *t;
        
        switch(mode)
        {
            case 1:
                cout<<"----------SELECTED FIRST COME FIRST SERVE-----------"<<endl;
                cout<<"----------SORTING BY ARRIVAL TIME-------------------"<<endl;
                for(int i=0; i<len-1; i++)
                {
                    n=i;
                    for(int j=i+1; j<len; j++)
                    {
                        if(pr[j]->getArrivalTime()<pr[n]->getArrivalTime()) n=j;
                    }

                    if(n!=i)
                    {
                        t=pr[i];
                        pr[i]=pr[n];
                        pr[n]=t;
                    }
                }
                break;
            case 2:
                cout<<"----------SELECTED SHORTEST JOB FIRST-----------"<<endl;
                cout<<"----------SORTING BY RUNTIME--------------------"<<endl;
                for(int i=0; i<len-1; i++)
                {
                    n=i;
                    for(int j=i+1; j<len; j++)
                    {
                        if(pr[j]->getrunTime()<pr[n]->getrunTime()) n=j;
                    }

                    if(n!=i)
                    {
                        t=pr[i];
                        pr[i]=pr[n];
                        pr[n]=t;
                    }
                }
                break;
            case 3:
                cout<<"----------SELECTED ROUND ROBIN------------------"<<endl;
                cout<<"----------SORTING BY PRIORITY---------------"<<endl;
                for(int i=0; i<len-1; i++)
                {
                    n=i;
                    for(int j=i+1; j<len; j++)
                    {
                        if(pr[j]->getArrivalTime()<pr[n]->getArrivalTime()) n=j;
                    }

                    if(n!=i)
                    {
                        t=pr[i];
                        pr[i]=pr[n];
                        pr[n]=t;
                    }
                }
                break;
            case 4:
                cout<<"----------SELECTED PRIORITY SCHEDULING----------"<<endl;
                cout<<"----------SORTING BY PRIORITY---------------"<<endl;
                for(int i=0; i<len-1; i++)
                {
                    n=i;
                    for(int j=i+1; j<len; j++)
                    {
                        if(pr[j]->getpriority()<pr[n]->getpriority()) n=j;
                    }

                    if(n!=i)
                    {
                        t=pr[i];
                        pr[i]=pr[n];
                        pr[n]=t;
                    }
                }
                break;
            default:
                cout<<"\nDEFAULT CALLED"<<endl;
                break;
        }

        int j=len;
        head=tail=NULL;
        len=0;
        for(int i=0; i<j; i++) append(pr[i]);
    }

void queuer:: sorter(int mode)
    {
        processes *p=head;
        int const* len1;
        len1=&len;
        processes **pr=new processes*[*len1];
        for(int i=0; i<len; i++)
        {
            pr[i]=p;
            p=p->getnext();
        }

        int n=0;
        processes *t;
        
        switch(mode)
        {
            case 1:
                for(int i=0; i<len-1; i++)
                {
                    n=i;
                    for(int j=i+1; j<len; j++)
                    {
                        if(pr[j]->getArrivalTime()<pr[n]->getArrivalTime()) n=j;
                    }

                    if(n!=i)
                    {
                        t=pr[i];
                        pr[i]=pr[n];
                        pr[n]=t;
                    }
                }
                break;
            case 2:
                for(int i=0; i<len-1; i++)
                {
                    n=i;
                    for(int j=i+1; j<len; j++)
                    {
                        if(pr[j]->getrunTime()<pr[n]->getrunTime()) n=j;
                    }

                    if(n!=i)
                    {
                        t=pr[i];
                        pr[i]=pr[n];
                        pr[n]=t;
                    }
                }
                break;
            case 3:
                for(int i=0; i<len-1; i++)
                {
                    n=i;
                    for(int j=i+1; j<len; j++)
                    {
                        if(pr[j]->getArrivalTime()<pr[n]->getArrivalTime()) n=j;
                    }

                    if(n!=i)
                    {
                        t=pr[i];
                        pr[i]=pr[n];
                        pr[n]=t;
                    }
                }
                break;
            case 4:
                for(int i=0; i<len-1; i++)
                {
                    n=i;
                    for(int j=i+1; j<len; j++)
                    {
                        if(pr[j]->getpriority()<pr[n]->getpriority()) n=j;
                    }

                    if(n!=i)
                    {
                        t=pr[i];
                        pr[i]=pr[n];
                        pr[n]=t;
                    }
                }
                break;
            default:
                cout<<"\nDEFAULT CALLED"<<endl;
                break;
        }

        int j=len;
        head=tail=NULL;
        len=0;
        for(int i=0; i<j; i++) append(pr[i]);
    }

    queuer* fcfs(queuer *q, timer *t)
    {
        queuer *nque;
        nque= new queuer();
        processes *p=NULL;
        for(int i=q->gethead()->getArrivalTime(); ; i++)
        {
            p=q->gethead();
            if(p==NULL) break;
            p->starter(i);
            for(int j=0;j<p->getrunTime();j++)
            {
                if(randomInt(0,100)<10)
                {
                    processes *pr;
                    pr=new processes(i+j);
                    q->append(pr);
                }
                if(j==p->getrunTime()-1)
                {
                    slots *s;
                    s=new slots(i,i+j+1,p);
                    t->append(s);
                    i+=j;
                    p->done(i+1);
                    q->deleter(p);
                    nque->append(p);
                }
            }
        }
        return nque;
    }

    queuer *sjf(queuer *q,timer *t)
    {
        queuer *nque;
        nque= new queuer();
        processes *p=NULL;
        for(int i=q->gethead()->getArrivalTime(); ; i++)
        {
            p=q->gethead();
            if(p==NULL) break;
            p->starter(i);
            for(int j=0;j<p->getrunTime();j++)
            {
                if(randomInt(0,100)<10)
                {
                    processes *pr;
                    pr=new processes(i+j);
                    q->append(pr);
                }
                if(j==p->getrunTime()-1)
                {
                    slots *s;
                    s=new slots(i,i+j+1,p);
                    t->append(s);
                    i+=j;
                    p->done(i+1);
                    q->deleter(p);
                    nque->append(p);
                }
            }
            q->sorter(2);
        }
        return nque;
    }

    queuer *priorityscheduling(queuer *q,timer *t)
    {
        queuer *nque;
        nque= new queuer();
        processes *p=NULL;
        for(int i=q->gethead()->getArrivalTime(); ; i++)
        {
            p=q->gethead();
            if(p==NULL) break;

            p->starter(i);
            for(int j=0;j<p->getrunTime();j++)
            {
                if(randomInt(0,100)<10)
                {
                    processes *pr;
                    pr=new processes(i+j);
                    q->append(pr);
                }
                if(j==p->getrunTime()-1)
                {
                    slots *s;
                    s=new slots(i,i+j+1,p);
                    t->append(s);
                    i+=j;
                    p->done(i+1);
                    q->deleter(p);
                    nque->append(p);
                }
            }
            q->sorter(4);
        }
        return nque;
    }

    queuer *roundrobin(queuer *q,timer *t,int quartum)
    {
        queuer *nque;
        nque= new queuer();
        processes *p=NULL;
        for(int i=q->gethead()->getArrivalTime(); ; i++)
        {
            p=q->gethead();
            if(p==NULL) break;
            if(p->getStartTime()==-1) p->starter(i);
            for(int j=0;j<p->getrunTime() && j<quartum;j++)
            {
                if(randomInt(0,100)<15)
                {
                    processes *pr;
                    pr=new processes(i+j);
                    q->append(pr);
                }
                if(j==p->getrunTime()-1)
                {
                    slots *s;
                    s=new slots(i,i+j+1,p);
                    t->append(s);
                    i+=j;
                    p->done(i+1);
                    q->deleter(p);
                    nque->append(p);
                    break;
                }
                if(j==quartum-1)
                {
                    slots *s;
                    s=new slots(i,i+j+1,p);
                    t->append(s);
                    i+=j;
                    q->deleter(p);
                    q->append(p);
                    p->reduceRuntime(j+1);
                }
            }
            if(q->gethead()!=NULL) q->sorter(4);
        }
        return nque;
    }

    queuer *cpuschedule(queuer *q, int mode, timer *t,int quartum)
    {
        queuer *nque=NULL;
        if(mode==1) nque=fcfs(q,t);
        else if(mode==2) nque=sjf(q,t);
        else if(mode==3) nque=roundrobin(q,t,quartum);
        else if(mode==4) nque=priorityscheduling(q,t);
        return nque;
    }

int main()
{
    srand(0);

    processes a[4];

    queuer q(a,4);
    q.display();

    int mode=0,quartum=0;
    for(int i=0; i<1; i++)
    {
    cout<<"\nEnter the CPU scheduling algorithm to use: "<<endl;
    cout<<"1.First Come First Served(FCFS) \n2.Shortest Job First(SJF) \n3.Round Robin \n4.Priority Scheduling"<<endl;
    cin>>mode;
    if(mode<1||mode>4)
    {
        i--;
        cout<<"\nERROR PLEASE RETRY\n"<<endl;
    }
    if(mode==3)
    {
    for(int j=0; j<1; j++)
    {
        cout<<"\nEnter the Size of the quartum to use: ";
        cin>>quartum;
        if(quartum<1) 
        {
            j--;
            cout<<"\nERROR PLEASE RETRY\n"<<endl;
        }
    }
    }
    }
    
    cout<<"\n-----------ORIGINAL ARRAY--------"<<endl;
    queuer *nque=NULL;
    switch(mode)
    {
        case 1:
            q.initsorter(mode);
            break;
        case 2:
            q.initsorter(mode);
            break;
        case 3:
            q.initsorter(mode);
            break;
        case 4:
            q.initsorter(mode);
            break;
        default:
            cout<<"DEFAULT CALLED"<<endl;
            break;
    }

    q.display();

    timer t;
    nque=cpuschedule(&q,mode,&t,quartum);
    cout<<"\n\n--------------COMPLETE QUEUES------------------"<<endl;
    nque->display();

    cout<<"\n\n-------------------------TIME SECTION DISPLAY-------------------"<<endl;
    t.display();
}

//Sorting function to change. Modes.