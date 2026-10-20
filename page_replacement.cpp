#include<iostream>
using namespace std;

int PgCount=0;
int stepCounter=1;

int randomInt(int Low, int High)
{
    int p;
    p= Low+(rand() %(High-Low));
    return p;
}
class steps;
class timer;
class page;
class buffer;
void shifter(page *p[], int a, int len);
int findmin(int arr[], page *p[], int len, int len2);

class page{
    int pgNo,refbit;
    page *nextPg,*prevPg;
    public:
        page()
        {
            pgNo=++PgCount;
            nextPg=prevPg=NULL;
            refbit=-1;
        }
        void setrefbit(int bit){refbit=bit;}
        int getrefbit(){return refbit;}
        int getPgno(){return pgNo;}
        page *getnext(){return nextPg;}
        page *getprev(){return prevPg;}
        void setnext(page *t)
        {
            nextPg=t;
            if(t!=NULL)
            t->setprev(this);
        }
        page *getthis(){return this;}
        void setprev(page *t){prevPg=t;}
};

class step{
    int stepNo,mode;
    page *cPage,*pPage;
    step *nextStep;
public:
    /**MODE 1= PAGE FAULT
     * MODE 2= PAGE REPLACEMENT
     * MODE 3= PAGE HIT
    **/
    step()
    {
        stepNo=0;
        mode=-1;
        cPage=pPage=NULL;
        nextStep=NULL;
    }
    step(int m, int b, page *p) //MODE 3 PAGE HIT
    {
            mode= m;
            cPage=p;
            pPage=NULL;
            stepNo=b;
            nextStep=NULL;
    }
    step(int m, int b, page *p, page *q)    //MODE 1 AND 2 PAGE FAULT & REPLACEMENT 
    {
        mode=m;
        cPage=p;
        pPage=q;
        stepNo=b;
        nextStep=NULL;
    }
    int getmode(){return mode;}
    void setnext(step *s){nextStep=s;}
    step *getnext(){return nextStep;}
    void display(){
        cout<<"\nStep Number= "<<stepNo<<endl;
        if(mode==1) {
            cout<<"\nPage Fault of page "<<cPage->getPgno()<<endl;
        }
        else if(mode==2) {
            cout<<"\nPage Replacement of page "<<pPage->getPgno()<<" with "<<cPage->getPgno()<<endl;
        }
        else if(mode==3) {
            cout<<"\nPage Hit of page "<<cPage->getPgno()<<endl;
        }
    }
};

class snapshot
{
    page **p;
    int len,stepNumber;
    int *indicator;
    snapshot *nextSnap;
public:
    snapshot()
    {
        p=NULL;
        stepNumber=len=0;
        nextSnap=NULL;
        indicator=NULL;
    }
    snapshot(page *pg[],int l, int nStep)
    {
        len=l;
        int const *len1;
        len1=&len;
        p=new page*[*len1];
        indicator=new int[*len1];
        for(int i=0; i<len; i++)
        {
            p[i]=pg[i];
            if(pg[i]!=NULL)
            {
                indicator[i]=pg[i]->getrefbit();
            }else{
                indicator[i]=-1;
            }
        }
        nextSnap=NULL;
        stepNumber=nStep;
        
    }
    void setnext(snapshot *s){nextSnap=s;}
    snapshot *getnext(){return nextSnap;}
    void display()
    {
        cout<<"\nSTEP NO "<<stepNumber<<endl;
        for(int i=0; i<len; i++)
        {
            cout<<"Position "<<i+1<<" = ";
            if(p[i]==NULL) 
            {
                cout<<"NULL"<<endl;
            }else{
                cout<<p[i]->getPgno();
                if(indicator[i]!=-1) cout<<"("<<indicator[i]<<")"<<endl;
                else cout<<endl;
            }
        }
    }
    void minDisplay()
    {
        for(int i=0; i<len; i++)
        {
            cout<<"Position "<<i+1<<" = ";
            if(p[i]==NULL) 
            {
                cout<<"NULL"<<endl;
            }else{
                cout<<p[i]->getPgno();
                if(indicator[i]!=-1) cout<<"("<<indicator[i]<<")"<<endl;
                else cout<<endl;
            }
        }
    }
};

class snapsRecorder{
    snapshot *head,*current;
public:
    snapsRecorder()
    {
        head=current=NULL;
    }
    void append(snapshot *s)
    {
        if(head==NULL) 
        {
            head=current=s;
        }else{
            current->setnext(s);
            current=s;
        }
    }
    void display(int mode)
    {
        if(mode==1)
        {
            snapshot *s=NULL;
            s=head;
            while(s!=NULL){
                s->display();
                s=s->getnext();
            }
        }else if(mode==2)
        {
            current->minDisplay();
            current=current->getnext();
        }
    }
    void resetCurrent() {current=head;}

};

class timer{
    step *head,*tail;
    int nPageFault,nPageRep,nPageHit;
    float hitRatio;
public:
    timer() {
        head=tail=NULL;
        nPageFault=nPageRep=nPageHit=hitRatio=0;
    }
    timer(step *s) {
        head=tail=s;
        nPageFault=nPageRep=nPageHit=hitRatio=0;    
    }
    void append(step *s)
    {
        if(tail==NULL)
        {
            head=tail=s;
        }else{
            tail->setnext(s);
            tail=s;
        }
        switch(s->getmode())
        {
            case 1:
                nPageFault++;
                break;
            case 2:
                nPageRep++;
                break;
            case 3:
                nPageHit++;
                break;
        }
    }
    int getPageFault(){return nPageFault+nPageRep;}
    int getPageHit(){return nPageHit;}
    int getPageRep(){return nPageRep;}
    float calcHitRatio()
    {
        hitRatio= (float)nPageHit/(float)(nPageFault+nPageHit+nPageRep);
        hitRatio*=100;
        return hitRatio;
    }
    void display(snapsRecorder *record)
    {
        cout<<"\n-----------STEPS DISPLAY------------"<<endl;
        step *s=head;
        while(s!=NULL)
        {
            s->display();
            record->display(2);
            s=s->getnext();
        }

        cout<<"\n\nSUMMARY"<<endl;
        cout<<"Page Hit= "<<nPageHit<<endl;
        cout<<"Page Fault= "<<this->getPageFault()<<endl;
        cout<<"Page Replacement= "<<nPageRep<<endl;
        cout<<"Total Steps= "<<(nPageFault+nPageHit+nPageRep)<<endl;
        cout<<"Hit Ratio= "<<calcHitRatio()<<endl;
    }
};

class buffer
{
    int len,point;
    page *head,*end;
    page **p;
    timer *time;
    snapsRecorder *record;
public:
    buffer()
    {
        point=0;
        len=randomInt(5,7);
        head=end=NULL;
        int const* len1;
        len1=&len;
        p= new page*[*len1];
        //p=t;
        initialize(p);
        time=new timer();
        record= new snapsRecorder();
    }
    buffer(int a)
    {
        point=0;
        len=a;
        head=end=NULL;
        int const* len1;
        len1=&len;
        p= new page*[*len1];
        initialize(p);
        time=new timer();
        record= new snapsRecorder();
    }
    buffer(int bSize, int arrSize, page t[])
    {
        point=0;
        if(bSize<arrSize) {
            cout<<"\nCalled because of size of array greater than size of buffer\n"<<endl;
            len=arrSize;
        }else{
            len=bSize;
        }
        head=end=NULL;
        int const* len1;
        len1=&len;
        p= new page*[*len1];
        initialize(p);
        for(int i=0; i<arrSize; i++) p[i]=&t[i];
        time=new timer();
        record= new snapsRecorder();
    }
    buffer(page t[], int arrSize)
    {
        point=0;
        int i=randomInt(5,7);
        if(i<arrSize) {
            cout<<"\nCalled because of size of array greater than size of buffer\n"<<endl;
            len=arrSize;
        }else{
            len=i;
        }
        head=end=NULL;
        int const* len1;
        len1=&len;
        p= new page*[*len1];
        initialize(p);
        for(int i=0; i<arrSize; i++) {p[i]=&t[i];}
        time=new timer();
        record= new snapsRecorder();
    }
    void initialize(page *pg[])
    {
        for(int i=0; i<len; i++) pg[i]=NULL;
    }
    timer *getTimer(){return time;}
    void FIFOaddpage(page *q,int i)
    {
        page *t=NULL;
        if(i<len)
        {
            p[i]=q;
        }else{
            p[point]=q;
            point++;
            point%=len;
            //PAGE REPLACEMENT
        }
    }
    page* FINUFOaddpage(page *q,int i)
    {
        page *ret=NULL;
        page *t=NULL;
        if(i<len)
        {
            p[i]=q;
        }else{
            while(p[point]->getrefbit()!=0)
            {
                p[point]->setrefbit(0);
                point++;
                point%=len;
            }
            ret=p[point];
            p[point]=q;
            point++;
            point%=len;
            //PAGE REPLACEMENT
        }
        q->setrefbit(1);
        return ret;
    }
    void LRUaddpage(page *q,int i)
    {
        if(i<len)
        {
            p[i]=q;
        }else{
            p[len-1]=q;
            shifter(p,len-1,len);
        }
    }
    void optimaladdpage(page *q, int i)
    {
            p[i]=q;
    }
    void refpage(page *q, int algNo, int arr[], int len2)
    {
        //algNo=1 FIFO
        //algNo=2 FINUFO
        //algNo=3 LRU
        //algNo=4 OPTIMAL PAGE ALGORITHM
        int i=0,mode=0;
        for(i=0; i<len; i++) 
        {
            if(p[i]==q) {mode=1;break;}
            if(p[i]==NULL) {mode=0;break;}
        }

        if(algNo==1)    //FIFO 
        {
            if(mode==1)
            {
                step *s= new step(3,stepCounter++,q);
                time->append(s);
                //cout<<"Page Hit Here"<<endl;
            }else{
                if(i==len)
                {
                    step *s= new step(2,stepCounter++,q,p[point]);
                    time->append(s);
                    //cout<<"Page Replacement Here"<<endl;
                    FIFOaddpage(q,i);
            }else{
                step *s= new step(1,stepCounter++,q,p[point]);
                time->append(s);
                //cout<<"Page Fault Here"<<endl;
                FIFOaddpage(q,i);
            }
        }
        }else if(algNo==2)      //2ND CHANCE
        {
            if(mode==1)
            {
                step *s= new step(3,stepCounter++,q);
                time->append(s);
                p[i]->setrefbit(1);
                //cout<<"Page Hit Here"<<endl;
            }else{
                if(i==len)
                {
                    page *pgrec=FINUFOaddpage(q,i);
                    step *s= new step(2,stepCounter++,q,pgrec);
                    time->append(s);
                    //cout<<"Page Replacement Here"<<endl;
            }else{
                //cout<<"Page Fault Here"<<endl;
                page *pgrec=FINUFOaddpage(q,i);
                step *s= new step(1,stepCounter++,q,pgrec);
                time->append(s);
            }
        }
        }else if(algNo==3){     //LEAST RECENTLY USED
            if(mode==1)
            {
                step *s= new step(3,stepCounter++,q);
                time->append(s);
                shifter(p,i,len);
                //PAGE HIT HERE
            }else{
                if(i==len)
                {
                    step *s= new step(2,stepCounter++,q,p[len-1]);
                    time->append(s);
                    LRUaddpage(q,i);
                    //PAGE REPLACEMENT HERE
                }else{
                    step *s= new step(1,stepCounter++,q,p[i]);
                    time->append(s);
                    LRUaddpage(q,i);
                    //PAGE FAULT HERE
                }
            }
        }else if(algNo==4)      //OPTIMAL
        {
            if(mode==1)
            {
                step *s= new step(3,stepCounter++,q);
                time->append(s);
                //PAGE HIT HERE
            }else{
                if(i==len)
                {
                    i= findmin(arr,p,len,len2);
                    step *s= new step(2,stepCounter++,q,p[i]);
                    time->append(s);
                    optimaladdpage(q,i);
                    //PAGE REPLACEMENT HERE
                }else{
                    step *s= new step(1,stepCounter++,q,p[i]);
                    time->append(s);
                    optimaladdpage(q,i);
                    //PAGE FAULT HERE
                }
            }
        }
            snapshot *snap= new snapshot(p,len,stepCounter-1);
            record->append(snap);
    }
    void display()
    {
        record->resetCurrent();
        cout<<"\n--------FINAL BUFFER-----------"<<endl;
        for(int i=0; i<len; i++)
        {
            cout<<"Position "<<i+1<<" : "<<p[i]->getPgno();
            if(p[i]->getrefbit()!=-1)
            {
                cout<<"("<<p[i]->getrefbit()<<")"<<endl;;
            }else{
                cout<<endl;
            }
        }
        time->display(record);
    }
};

void shifter(page *p[], int a, int len)
{
    page *t=p[a];
    for(int i=a; i>0; i--)
    {
        p[i]=p[i-1];
    }
    p[0]=t;
}

int findmin(int arr[], page *p[], int len, int len2)
{
    int const *len1;
    len1=&len;
    int *min= new int[*len1];
    for(int i=0; i<len; i++)
    {
        min[i]=-1;
        for(int j=stepCounter; j<len2; j++)
        {
            if(p[i]->getPgno()==arr[j]+1)
            {
                min[i]=j;
                break;
            }
        }
    }
    int n=0;
    for(int i=0; i<len; i++)
    {
        if(min[i]==-1) return i;
        if(min[i]>min[n]) n=i;
    }
    return n;
}

int main()
{
    srand(0);
    int q;

    for(int i=0; i<1; i++)
    {
        cout<<"Enter the number of processes you want: ";
        cin>>q;
        if(q<=0)
        {
            cout<<"Error. Number of processes less than 1. Please Retry\n"<<endl;
            i--;
        }
    }

    page p[20];

    int t;
    cout<<endl;
    for(int i=0; i<1; i++)
    {
        cout<<"Enter the number of page sequences you want: ";
        cin>>t;
        if(t<=0)
        {
            cout<<"Error. Number of page sequences less than 1. Please Retry\n"<<endl;
            i--;
        }
    }

    int const *len2;
    len2=&t;
    int *arr= new int[*len2];

    cout<<"Page Sequence "<<endl;
    for(int i=0; i<t; i++) {
        if(i==t-1) 
        {
            arr[i]=randomInt(0,q);
            cout<<arr[i]+1<<endl;
            break;
        }
        arr[i]=randomInt(0,q);
        cout<<arr[i]+1<<",";
    }
    
    int num;
    cout<<endl;
    for(int i=0; i<1; i++)
    {
        cout<<"Enter the Size of buffer you want: ";
        cin>>num;
        if(num<=0)
        {
            cout<<"Error. Size of buffer less than 1. Please Retry\n"<<endl;
            i--;
        }
    }

    int md;
    cout<<endl;
    for(int i=0; i<1; i++)
    {
        cout<<"Enter the page replacement algorithm you want: "<<endl;
        cout<<"1. FIFO\n2. 2nd Chance \n3. LRU \n4. OPTIMAL\nInput: ";
        cin>>md;
        if(md<=0 || md>4)
        {
            cout<<"Error. Invalid Page Algorithm Number. Please Retry\n"<<endl;
            i--;
        }
    }

    buffer b(num);
    for(int i=0; i<t; i++)
    b.refpage(&p[arr[i]],md,arr,t);

    b.display();
}

//Create Snapshots of buffer at every iteration -----DONE----
//2nd Chance        -------DONE----------
//LRU               -------DONE----------
//OPTIMAL           -------DONE----------