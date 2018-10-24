#include <iostream>
#include <cstdlib>
#include<math.h>
using namespace std;
/*KEY POINTS:
>> 21 point ke baad bhi chal skte hai unless it is a black jack 21 i.e. len=2
>> above point ka exception hai ace spliting in which 21 with len=2 is considered as normal 21*/



//CHANGE YOUR STATE SPACE. D CAN ONLY HAPPEN IF PLAYERCARDLEN=2 AND NOT OTHERWISE SO WE CAN CONVERT ONE STATE TO TWO DIFF STATE
//LIKE LEN==2 AND LEN>=2.





// const int p=0.5;

//state space changed as more than 2 card hand can't perform doubling move
float freward[58][12]={}; //33x10 array which stores the final rewards
char faction[58][12]={}; //33x10 array which stores the final actions----'1':hit;'2':stand;'3':double;'4':split
struct State {
    int playerval; //player's hand value
    int playersofta; // no. of soft A's in player's hand
    int playercardlen;
    int dealerval;
    int dealersofta;
    int dealercardlen;
    bool pair;
};
//beware of soft A

int statetonumber(State &s)//get your state number rn
{
    // cout<<"details of s incoming"<<endl;
    // cout<<"playerval="<<s.playerval<<endl;
    // cout<<"playercardlen="<<s.playercardlen<<endl;
    // cout<<"playersofta="<<s.playersofta<<endl;
    // cout<<"dealerval="<<s.dealerval<<endl;
    // cout<<"dealercardlen="<<s.dealercardlen<<endl;
    // cout<<"dealersofta="<<s.dealersofta<<endl;
    if(s.playersofta==1)
    {
        if(s.playercardlen==2)
        {
            if(s.playerval==12)//pair of ace--AA 33 
                return 33;
            else//one Ace and other non-face card 16 to 23
            {
                return s.playerval+3;
            }
        }
        else
        {
            if(s.playerval==12)//not possible
                return 33+33;
            else//one Ace and other non-face cards 49 to 56
            {
                return s.playerval+3+33;
            }
        }
    }
    else if(s.pair==true)
    {
        return 22+s.playerval/2;//24 to 32
    }
    else
    {
        if(s.playercardlen==2)
        {
            // cout<<
            return s.playerval-4;//simple cases of two card hand 1 to 15
        }
        else
        {
            // cout<<"playerval"<<s.playerval<<endl;
            if(s.playerval!=20)
                return s.playerval-4+33;// simple case of more than two card hand 34 to 48
            else
                return 57;
        }
    }
}

State numbertostate(int sno, int dno)
{
    State s;
    if(sno>=1 && sno<=15)
    {
        // cout<<"case1"<<endl;
        s.playerval=sno+4;
        s.playersofta=0;
        s.playercardlen=2;
        s.pair=false;
        // cout<<"case1 ends"<<endl;
    }
    else if(sno>=16 && sno<=23)
    {
        s.playerval=sno-3;
        s.playersofta=1;
        s.playercardlen=2;
        s.pair=false;
    }
    else if(sno>=24 && sno<=32)
    {
        s.playerval=2*(sno-22);
        s.playersofta=0;
        s.playercardlen=2;
        s.pair=true;
    }
    else if(sno==33)
    {
        s.playerval=12;
        s.playersofta=1;
        s.playercardlen=2;
        s.pair=true;
    }
    else if(sno>=34 && sno<=48)
    {
        s.playerval=sno+4-33;
        s.playersofta=0;
        s.playercardlen=3;
        s.pair=false;
    }
    else if(sno>=49 && sno<=56)
    {
        s.playerval=sno-3-33;
        s.playersofta=1;
        s.playercardlen=3;
        s.pair=false;
    }
    else if(sno==57)
    {
        s.playerval=20;
        s.playersofta=0;
        s.playercardlen=3;
        s.pair=false;
    }
    if(dno>=2 && dno<=10)
    {
        // cout<<"case1"<<endl;
        s.dealerval=dno;
        s.dealersofta=0;
        s.dealercardlen=1;
        // cout<<endl;
        // cout<<endl;
        // cout<<"details of s in numbertostate"<<endl;
        // cout<<"playerval="<<s.playerval<<endl;
        // cout<<"playercardlen="<<s.playercardlen<<endl;
        // cout<<"playersofta="<<s.playersofta<<endl;
        // cout<<"dealerval="<<s.dealerval<<endl;
        // cout<<"dealercardlen="<<s.dealercardlen<<endl;
        // cout<<"dealersofta="<<s.dealersofta<<endl;
        // cout<<endl;
        // cout<<endl;
    }
    else if(dno==11)
    {
        s.dealerval=dno;
        s.dealersofta=1;
        s.dealercardlen=1;
    }
    return s;
}


void statecopy(State &tostate, State &fromstate)
{
    tostate.playerval=fromstate.playerval;
    tostate.playersofta=fromstate.playersofta;
    tostate.playercardlen=fromstate.playercardlen;
    tostate.dealerval=fromstate.dealerval;
    tostate.dealersofta=fromstate.dealersofta;
    tostate.dealercardlen=fromstate.dealercardlen;
    tostate.pair=fromstate.pair;
}
//NEED TO AD
//D BLACK-JACK FEATURE ALSO. PLAYER BLACK-JACK AS WELL AS DEALER BLACK-JACK.
float standreward(State &s, float deal, float p)//expected reward after player stands. only dealer is making moves now.
{
    float reward;
    float rewardface;
    //reward to player if face-card appears
    if(s.dealerval+10>21 && s.dealersofta==0)//dealer busts
    {
        if(s.playerval==21 && s.playercardlen==2)//player black jack
            rewardface=1.5*deal;
        else
            rewardface=deal;
    }
    else if(s.dealerval+10>21 && s.dealersofta!=0)//dealer uses soft A
    {
        State temps;
        statecopy(temps,s);
        temps.dealersofta=temps.dealersofta-1;
        temps.dealercardlen=temps.dealercardlen+1;
        rewardface=standreward(temps,deal,p);
    }
    else if(s.dealerval+10==21 && s.dealercardlen==1)// dealer black jack
    {
        if(s.playerval==21 && s.playercardlen==2)//player black jack
            rewardface=0;//push
        else
            rewardface=-1*deal;//lost
    }
    else if(s.dealerval+10==21 && s.dealercardlen!=1)// dealer non black-jack 21
    {
        if(s.playerval==21 && s.playercardlen==2)//player black jack
            rewardface=1.5*deal;//win
        else if(s.playerval==21 && s.playercardlen!=2)
            rewardface=0;//push
        else 
            rewardface=-1*deal;//lost
    }
    else if(s.dealerval+10<21)
    {
        if(s.dealerval+10>=17)//stands(controversial)
        {
            if(s.dealerval+10>s.playerval)//player looses
                rewardface=-1*deal;
            else if(s.dealerval+10==s.playerval)//PUSH
            {
                rewardface=0;
            }
            else//player wins
            {
                if(s.playerval==21 && s.playercardlen==2)//player black jack
                    rewardface=1.5*deal;
                else
                    rewardface=deal;
            }
        }
        else//dealer continues hitting
        {
            State temps;
            statecopy(temps,s);
            temps.dealerval=temps.dealerval+10;
            temps.dealercardlen=temps.dealercardlen+1;
            rewardface=standreward(temps,deal,p);
        }
    }
    reward=p*rewardface;
    //reward to player if non-face card appears(loop-check in sequence)
    // number cards from 2-9 (ace not included)
    for(int i=2;i<=9;i++)
    {
        float rewardnonface;
        if(s.dealerval+i>21 && s.dealersofta==0)//bust
        {
            if(s.playerval==21 && s.playercardlen==2)//player black jack
                rewardnonface=1.5*deal;
            else
                rewardnonface=deal;
        }
        else if(s.dealerval+i>21 && s.dealersofta!=0)//use soft A
        {
            State temps;
            statecopy(temps,s);
            temps.dealersofta=temps.dealersofta-1;
            temps.dealercardlen=temps.dealercardlen+1;
            temps.dealerval=temps.dealerval+i-10;
            rewardnonface=standreward(temps,deal,p);
        }
        else if(s.dealerval+i==21 && s.dealercardlen!=1)// dealer non black-jack 21
        {
            if(s.playerval==21 && s.playercardlen==2)//player black jack
                rewardnonface=1.5*deal;//win
            else if(s.playerval==21 && s.playercardlen!=2)
                rewardnonface=0;//push
            else 
                rewardnonface=-1*deal;//lost
        }
        else if(s.dealerval+i<21)
        {
            if(s.dealerval+i>=17)//stands(controversial)
            {
                if(s.dealerval+i>s.playerval)//player looses
                    rewardnonface=-1*deal;
                else if(s.dealerval+i==s.playerval)//PUSH
                    rewardnonface=0;
                else//player wins
                {
                    if(s.playerval==21 && s.playercardlen==2)//player black jack
                        rewardnonface=1.5*deal;
                    else
                        rewardnonface=deal;
                }
            }
            else//dealer continues hitting
            {
                State temps;
                statecopy(temps,s);
                temps.dealerval=temps.dealerval+i;
                temps.dealercardlen=temps.dealercardlen+1;
                rewardnonface=standreward(temps,deal,p);
            }
        }
        reward=reward+((1-p)/9)*rewardnonface;
    }
    //when ace comes out
    float rewardace;
    if(s.dealerval+11>21)//soft ACE leading to busting
    {
        if(s.dealerval+1>=17)//stands(controversial)
        {
            if(s.dealerval+1>s.playerval)//player looses
                rewardace=-1*deal;
            else if(s.dealerval+1==s.playerval)//PUSH
                rewardace=0;
            else//player wins
            {
                if(s.playerval==21 && s.playercardlen==2)//player black jack
                    rewardace=1.5*deal;
                else
                    rewardace=1*deal;   
            }
        }
        else//dealer continues hitting
        {
            State temps;
            statecopy(temps,s);
            temps.dealerval=temps.dealerval+1;
            temps.dealercardlen=temps.dealercardlen+1;
            rewardace=standreward(temps,deal,p);
        }
    }
    else if(s.dealerval+11==21 && s.dealercardlen==1)//Dealer Black Jack
    {
        if(s.playerval==21 && s.playercardlen==2)//player black jack
            rewardace=0;//push
        else
            rewardace=-1*deal;//lost
    }
    else if(s.dealerval+11==21 && s.dealercardlen!=1)//Dealer Non Black Jack 21
    {
        if(s.playerval==21 && s.playercardlen==2)//player black jack
            rewardace=1.5*deal;//win
        else if(s.playerval==21 && s.playercardlen!=2)
            rewardace=0;//push
        else 
            rewardace=-1*deal;//lost
    }
    else
    {
        if(s.dealerval+11>=17)
        {
            if(s.dealerval+11>s.playerval)//player looses
                rewardace=-1*deal;
            else if(s.dealerval+11==s.playerval)//PUSH
                rewardace=0;
            else//player wins
            {
                if(s.playerval==21 && s.playercardlen==2)//player black jack
                    rewardace=1.5*deal;
                else
                    rewardace=1*deal;  
            } 
        }
        else
        {
            State temps;
            statecopy(temps,s);
            temps.dealerval=temps.dealerval+11;
            temps.dealercardlen=temps.dealercardlen+1;
            temps.dealersofta=temps.dealersofta+1;
            rewardace=standreward(temps,deal,p);
        }
    }
    reward=reward+((1-p)/9)*rewardace;
    
    return reward;
}

//reward calculation when decide to hit
float hitreward(State &s, int deal,float p)
{
    //facecard
    float reward;
    float rewardface;
    if(s.playerval+10>21 && s.playersofta==0)//player busts
        rewardface=-1*deal;
    else if(s.playerval+10>21 && s.playersofta!=0)//player uses soft A
    {
        State temps;
        statecopy(temps,s);
        temps.playersofta=temps.playersofta-1;
        temps.playercardlen=temps.playercardlen+1;
        temps.pair=false;
        rewardface=freward[statetonumber(temps)][s.dealerval];
        // rewardface=finalreward(temps,deal,p);
    }
    else if(s.playerval+10<21)
    {

        State temps;
        // cout<<"before statecopy"<<endl;
        statecopy(temps,s);
        // cout<<"after statecopy"<<endl;
        temps.playerval=temps.playerval+10;
        temps.playercardlen=temps.playercardlen+1;
        temps.pair=false;
        // cout<<"s no."<<statetonumber(temps)<<endl;
        rewardface=freward[statetonumber(temps)][s.dealerval];
        // cout<<"end after statecopy"<<endl;
    }
    else if(s.playerval+10==21)
    {
        State temps;
        // cout<<"before statecopy"<<endl;
        statecopy(temps,s);
        // cout<<"after statecopy"<<endl;
        temps.playerval=temps.playerval+10;
        temps.playercardlen=temps.playercardlen+1;
        temps.pair=false;
        rewardface=standreward(temps,deal,p);
    }
    reward=p*rewardface;
    //reward to player if non-face card appears(loop-check in sequence)
    // number cards from 2-9 (ace not included)
    for(int i=2;i<=9;i++)
    {
        float rewardnonface;
        if(s.playerval+i>21 && s.playersofta==0)//bust
            rewardnonface=-1*deal;
        else if(s.playerval+i>21 && s.playersofta!=0)//use soft A
        {
            State temps;
            statecopy(temps,s);
            temps.playersofta=temps.playersofta-1;
            temps.playercardlen=temps.playercardlen+1;
            temps.playerval=temps.playerval+i-10;
            temps.pair=false;
            rewardnonface=freward[statetonumber(temps)][s.dealerval];
        }
        else if(s.playerval+i<21)
        {
            State temps;
            statecopy(temps,s);
            temps.playerval=temps.playerval+i;
            temps.playercardlen=temps.playercardlen+1;
            temps.pair=false;
            rewardnonface=freward[statetonumber(temps)][s.dealerval];
        }
        else if(s.playerval+i==21)
        {
            State temps;
            // cout<<"before statecopy"<<endl;
            statecopy(temps,s);
            // cout<<"after statecopy"<<endl;
            temps.playerval=temps.playerval+i;
            temps.playercardlen=temps.playercardlen+1;
            temps.pair=false;
            rewardnonface=standreward(temps,deal,p);
        }
        reward=reward+((1-p)/9)*rewardnonface;
    }
    //when ace comes out
    float rewardace;
    if(s.playerval+11>21)//soft ACE leading to busting
    {
        State temps;
        statecopy(temps,s);
        temps.playerval=temps.playerval+1;
        temps.playercardlen=temps.playercardlen+1;
        temps.pair=false;
        if(s.playerval+1==21)
            rewardace=standreward(temps,deal,p);
        else
            rewardace=freward[statetonumber(temps)][s.dealerval];
    }
    else if(s.playerval+11==21)
    {
        State temps;
        statecopy(temps,s);
        temps.playerval=temps.playerval+11;
        temps.playercardlen=temps.playercardlen+1;
        temps.playersofta=temps.playersofta+1;
        temps.pair=false;
        rewardace=standreward(temps,deal,p);
    }
    else
    {
        State temps;
        statecopy(temps,s);
        temps.playerval=temps.playerval+11;
        temps.playercardlen=temps.playercardlen+1;
        temps.playersofta=temps.playersofta+1;
        temps.pair=false;
        rewardace=freward[statetonumber(temps)][s.dealerval];
    }
    reward=reward+((1-p)/9)*rewardace;
    return reward;
}

//reward calculation when decide to double
float doublereward(State &s, int deal,float p)
{
    deal=2*deal;
    //facecard
    float reward;
    float rewardface;
    if(s.playerval+10>21 && s.playersofta==0)//player busts
        rewardface=-1*deal;
    else if(s.playerval+10>21 && s.playersofta!=0)//player uses soft A
    {
        State temps;
        statecopy(temps,s);
        temps.playersofta=temps.playersofta-1;
        temps.playercardlen=temps.playercardlen+1;
        rewardface=standreward(temps,deal,p);
    }
    else if(s.playerval+10<=21)
    {
            State temps;
            statecopy(temps,s);
            temps.playerval=temps.playerval+10;
            temps.playercardlen=temps.playercardlen+1;
            rewardface=standreward(temps,deal,p);
    }
    reward=p*rewardface;
    //reward to player if non-face card appears(loop-check in sequence)
    // number cards from 2-9 (ace not included)
    for(int i=2;i<=9;i++)
    {
        float rewardnonface;
        if(s.playerval+i>21 && s.playersofta==0)//bust
            rewardnonface=-1*deal;
        else if(s.playerval+i>21 && s.playersofta!=0)//use soft A
        {
            State temps;
            statecopy(temps,s);
            temps.playersofta=temps.playersofta-1;
            temps.playercardlen=temps.playercardlen+1;
            temps.playerval=temps.playerval+i-10;
            rewardnonface=standreward(temps,deal,p);
        }
        else if(s.playerval+i<=21)
        {
            State temps;
            statecopy(temps,s);
            temps.playerval=temps.playerval+i;
            temps.playercardlen=temps.playercardlen+1;
            rewardnonface=standreward(temps,deal,p);
        }
        reward=reward+((1-p)/9)*rewardnonface;
    }
    //when ace comes out
    float rewardace;
    if(s.playerval+11>21)//soft ACE leading to busting
    {
        if(s.playerval+1>21 && s.playersofta==0)//hard ACE leading to busting
        {
            rewardace=-1*deal; 
        }
        else if(s.playerval+1>21 && s.playersofta!=0)
        {
            State temps;
            statecopy(temps,s);
            temps.playerval=temps.playerval+1-10;
            temps.playercardlen=temps.playercardlen+1;
            temps.playersofta=temps.playersofta-1;
            rewardace=standreward(temps,deal,p);
        }
        else//dealer continues hitting
        {
            State temps;
            statecopy(temps,s);
            temps.playerval=temps.playerval+1;
            temps.playercardlen=temps.playercardlen+1;
            rewardace=standreward(temps,deal,p);
        }
    }
    else
    {
        State temps;
        statecopy(temps,s);
        temps.playerval=temps.playerval+11;
        temps.playercardlen=temps.playercardlen+1;
        temps.playersofta=temps.playersofta+1;
        rewardace=standreward(temps,deal,p);
    }
    reward=reward+((1-p)/9)*rewardace;
    return reward;
}

//reward when splits


//dealing with pair=1 and pair=0
float splitreward(State &s, int deal,float p)
{
    //when pair is non-ace non-face i.e. 2-2,.........9-9
    if(statetonumber(s)>=24 && statetonumber(s)<=31)
    {
        // if a face-card comes up.. splitted hands won't have pair.
        float reward;
        float rewardface;
        State temps;
        statecopy(temps,s);
        temps.playerval=(temps.playerval/2)+10;
        temps.pair=false;
        temps.playercardlen=2;
        rewardface=freward[statetonumber(temps)][temps.dealerval];
        reward=p*rewardface;
        // if(statetonumber(s)==24)
        // {
        //     cout<<"(24,"<<s.dealerval<<")"<<endl;
        //     cout<<"rewardface="<<rewardface<<endl;
        // }
        // if a non-face card comes up.. chances of becoming a pair
        for(int i=2;i<=9;i++)
        {
            float rewardnonface;
            State temps1;
            statecopy(temps1,s);
            if(temps1.playerval/2==i)
                temps1.pair=true;
            else
                temps1.pair=false;
            temps1.playerval=(temps1.playerval/2)+i;
            temps1.playercardlen=2;
            if(temps1.pair==false)
            {
                rewardnonface=freward[statetonumber(temps1)][temps1.dealerval];
                reward=reward+((1-p)/9)*rewardnonface;
            }
            // if(statetonumber(s)==24)
            // {
            //     cout<<"(24,"<<s.dealerval<<")"<<endl;
            //     cout<<"rewardnonface="<<rewardnonface<<endl;
            // }
        }
        //if ACE comes up
        float rewardace;
        State temps2;
        statecopy(temps2,s);
        temps2.playerval=(temps2.playerval/2)+11;
        temps2.playersofta=temps2.playersofta+1;
        temps2.playercardlen=2;
        temps2.pair=false;
        rewardace=freward[statetonumber(temps2)][temps2.dealerval];
        reward=reward+((1-p)/9)*rewardace;
        // if(statetonumber(s)==24)
        // {
        //     cout<<"(24,"<<s.dealerval<<")"<<endl;
        //     cout<<"rewardace="<<rewardace<<endl;
        // }
        return 2*reward/(1-2*((1-p)/9));//because there are 2 hands like these
    }
    else if(statetonumber(s)==32)// pair 10-10
    {
        // if a face-card comes up.. splitted hands won't have pair.
        float reward;
        float rewardface;
        State temps;
        statecopy(temps,s);
        temps.playerval=10+10;
        temps.pair=true;
        temps.playercardlen=2;
        // rewardface=freward[statetonumber(temps)][temps.dealerval];
        reward=0;
        // if a non-face card comes up.. chances of becoming a pair
        for(int i=2;i<=9;i++)
        {
            float rewardnonface;
            State temps1;
            statecopy(temps1,s);
            temps1.pair=false;
            temps1.playerval=10+i;
            temps1.playercardlen=2;
            rewardnonface=freward[statetonumber(temps1)][temps1.dealerval];
            reward=reward+((1-p)/9)*rewardnonface;
        }
        //if ACE comes up------ IS THIS BLACK JACK???? YES!! THIS IS BLACK-JACK(resolved)
        float rewardace;
        State temps2;
        statecopy(temps2,s);
        temps2.playerval=10+11;
        temps2.playersofta=temps2.playersofta+1;
        temps2.pair=false;
        temps2.playercardlen=2;
        rewardace=standreward(temps2,deal,p);
        reward=reward+((1-p)/9)*rewardace;
        return 2*reward/(1-2*p);//because there are 2 hands like these
    }
    else if(statetonumber(s)==33)//KAAFI EXCEPTIONS. IN SHORT SIRF STAND KR SKTE HAI.
    {
        // if a face-card comes up
        float reward;
        float rewardface;
        State temps;
        statecopy(temps,s);
        temps.playerval=11+10;//-----IS THIS BLACK-JACK??? NO!! IT'S AN EXCEPTION(resolved)
        //but standreward will consider it as a black-jack. so, change the playercardlen.
        temps.playercardlen=3;
        rewardface=standreward(temps,deal,p);
        reward=p*rewardface;
        // if a non-face card comes up
        for(int i=2;i<=9;i++)
        {
            float rewardnonface;
            State temps1;
            statecopy(temps1,s);
            temps1.playerval=11+i;
            temps1.playercardlen=2;
            rewardnonface=standreward(temps1,deal,p);
            reward=reward+((1-p)/9)*rewardnonface;
        }
        //if ACE comes up
        float rewardace;
        State temps2;
        statecopy(temps2,s);
        temps2.playerval=11+1;
        temps2.playercardlen=2;
        rewardace=standreward(temps2,deal,p);
        reward=reward+((1-p)/9)*rewardace;
        return 2*reward;//because there are 2 hands like these
    }
} 


void finalreward(State &s, int deal,float p)
{
    float hreward,sreward,preward,dreward;
    
    if(s.pair==true)
    {
        // cout<<"pair before hit"<<endl;
        hreward=hitreward(s,deal,p);
        // hreward=-2;
        sreward=standreward(s,deal,p);
        
        dreward=doublereward(s,deal,p);
        preward=splitreward(s,deal,p);
        // cout<<"sreward="<<sreward<<endl;
        if(hreward>sreward && hreward>dreward && hreward>preward)
        {
            
            // if(statetonumber(s)==33)
            // {
            //     cout<<"hit in AA"<<endl;
            //     cout<<"33,"<<s.dealerval<<endl;
            //     cout<<"hit="<<hreward<<",stand="<<sreward<<",double="<<dreward<<",split="<<preward<<endl;
            // }
            freward[statetonumber(s)][s.dealerval]=hreward;
            faction[statetonumber(s)][s.dealerval]='H';
        }
        else if(sreward>hreward && sreward>dreward && sreward>preward)
        {
            freward[statetonumber(s)][s.dealerval]=sreward;
            faction[statetonumber(s)][s.dealerval]='S';
        }
        else if(dreward>sreward && dreward>hreward && dreward>preward)
        {
            freward[statetonumber(s)][s.dealerval]=dreward;
            faction[statetonumber(s)][s.dealerval]='D';
        }
        else if(preward>hreward && preward>sreward && preward>dreward)
        {
            
            // if(statetonumber(s)==33)
            // {
            //     cout<<"split in AA"<<endl;
            //     cout<<"33,"<<s.dealerval<<endl;
            //     cout<<"hit="<<hreward<<",stand="<<sreward<<",double="<<dreward<<",split="<<preward<<endl;
            // }
            freward[statetonumber(s)][s.dealerval]=preward;
            faction[statetonumber(s)][s.dealerval]='P';
        }
        // cout<<"freward="<<freward[statetonumber(s)][s.dealerval]<<endl;
    }
    else if(s.playercardlen==2 && s.playerval!=21)//NOT BLACK-JACK
    {
        // cout<<"not bj before hit"<<endl;
        hreward=hitreward(s,deal,p);
        // cout<<"hreward="<<hreward<<endl;
        // cout<<"not bj before stand"<<endl;
        sreward=standreward(s,deal,p);
        // cout<<"sreward="<<sreward<<endl;
        // cout<<"not bj before double"<<endl;
        dreward=doublereward(s,deal,p);
        // cout<<"dreward="<<dreward<<endl;
        if(hreward>=sreward && hreward>=dreward)
        {
            freward[statetonumber(s)][s.dealerval]=hreward;
            faction[statetonumber(s)][s.dealerval]='H';
        }
        else if(sreward>=hreward && sreward>=dreward)
        {
            freward[statetonumber(s)][s.dealerval]=sreward;
            faction[statetonumber(s)][s.dealerval]='S';
        }
        else
        {
            freward[statetonumber(s)][s.dealerval]=dreward;
            faction[statetonumber(s)][s.dealerval]='D';
        }
        // cout<<"freward="<<freward[statetonumber(s)][s.dealerval]<<endl;
    }
    else if(s.playercardlen==2 && s.playerval==21)//BLACK-JACK  //only stand applicable
    {
        // cout<<"bj before stand"<<endl;
        sreward=standreward(s,deal,p);
        // cout<<"sreward="<<sreward<<endl;
        freward[statetonumber(s)][s.dealerval]=sreward;
        faction[statetonumber(s)][s.dealerval]='S';
        // cout<<"freward="<<freward[statetonumber(s)][s.dealerval]<<endl;
    }
    else
    {
        // cout<<"else before hit"<<endl;
        hreward=hitreward(s,deal,p);
        // cout<<"hreward="<<hreward<<endl;
        // cout<<"else before stand"<<endl;
        sreward=standreward(s,deal,p);
        // cout<<"sreward="<<sreward<<endl;
        if(hreward>=sreward)
        {
            freward[statetonumber(s)][s.dealerval]=hreward;
            faction[statetonumber(s)][s.dealerval]='H';
        }
        else
        {
            freward[statetonumber(s)][s.dealerval]=sreward;
            faction[statetonumber(s)][s.dealerval]='S';
        }
        // cout<<"freward="<<freward[statetonumber(s)][s.dealerval]<<endl;
    }
}

void filldp(int deal, float p)
{
    State s;
    for(int i=2;i<=11;i++)
    {
        s=numbertostate(57,i);
        finalreward(s,deal,p);
    }
    for(int i=48;i>=39;i--)
    {
        for(int j=2;j<=11;j++)
        {
            s=numbertostate(i,j);
            finalreward(s,deal,p);
        }
    }
    for(int i=56;i>=49;i--)
    {
        for(int j=2;j<=11;j++)
        {
            s=numbertostate(i,j);
            finalreward(s,deal,p);
        }
    }
    for(int i=23;i>=16;i--)
    {
        for(int j=2;j<=11;j++)
        {
            s=numbertostate(i,j);
            finalreward(s,deal,p);
        }
    }
    for(int i=38;i>=34;i--)
    {
        for(int j=2;j<=11;j++)
        {
            s=numbertostate(i,j);
            finalreward(s,deal,p);
        }
    }
    for(int i=1;i<=15;i++)
    {
        for(int j=11;j>=2;j--)
        {
            s=numbertostate(i,j);
            finalreward(s,deal,p);
        }
    }
    // for(int i=24;i<=32;i++)
    // {
    //     for(int j=2;j<=11;j++)
    //     {
    //         s=numbertostate(i,j);
    //         finalreward(s,deal,p);
    //     }
    // }
    for(int i=33;i>=24;i--)
    {
        for(int j=2;j<=11;j++)
        {
            s=numbertostate(i,j);
            finalreward(s,deal,p);
        }
    }
}

int main()
{
    float p,freward_temp,e_temp;
    // cout<<"enter p"<<endl;
    cin>>p;float e=100.0; int k=0;
    // while(k<50)
    // {
    //     e=0;
    //     for(int i=1;i<=56;i++)
    //     {
    //         for(int j=2;j<=11;j++)
    //         {
    //             freward_temp=freward[i][j]; 
    //             // cout<<"i="<<i<<",j="<<j<<endl;
    //             State s=numbertostate(i,j);
    //             finalreward(s,1,p);
    //             if(freward_temp!=0)
    //             else
    //             e_temp=80;    //             e_temp=fabs(freward[i][j]-freward_temp)*100.0/freward_temp;

    //             if(e_temp>e)
    //             e=e_temp;

    //             // State s=numbertostate(i,j);
    //             // cout<<"details of s incoming"<<endl;
    //             // cout<<"playerval="<<s.playerval<<endl;
    //             // cout<<"playercardlen="<<s.playercardlen<<endl;
    //             // cout<<"playersofta="<<s.playersofta<<endl;
    //             // cout<<"dealerval="<<s.dealerval<<endl;
    //             // cout<<"dealercardlen="<<s.dealercardlen<<endl;
    //             // cout<<"dealersofta="<<s.dealersofta<<endl;
    //         }
    //     }
    //     cout<<e<<endl;
    //     k++;
    // }
    filldp(1,p);
    ofstream outFile;
    outFile.open("Policy.txt");
    for(int i=1;i<=33;i++)
    {
        if(i<=15)
            outFile<<i+4<<"\t";
        else if(i>15 && i<=23)
            outFile<<"A"<<i-14<<"\t";
        else if(i>=24 && i<=32)
            outFile<<i-22<<i-22<<"\t";
        else
            outFile<<"AA\t";        
        for(int j=2;j<=11;j++)
        {
            outFile<<faction[i][j]<<" ";
        }
        outFile<<endl;
    }
    outFile.close();
    // for(int i=1;i<=33;i++)
    // {
    //     for(int j=2;j<=11;j++)
    //     {
    //         cout<<"freward["<<i<<"]["<<j<<"] = "<<freward[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }
    // cout<<"about 24"<<endl;
    // cout<<"hit"<<endl;
    // for(int j=2;j<=11;j++)
    // {
    //     State s=numbertostate(24,j);
    //     cout<<hitreward(s,1,p)<<" ";

    // }
    // cout<<endl;
    // cout<<"stand"<<endl;
    // for(int j=2;j<=11;j++)
    // {
    //     State s=numbertostate(24,j);
    //     cout<<standreward(s,1,p)<<" ";

    // }
    // cout<<endl;
    // cout<<"double"<<endl;
    // for(int j=2;j<=11;j++)
    // {
    //     State s=numbertostate(24,j);
    //     cout<<doublereward(s,1,p)<<" ";

    // }
    // cout<<endl;
    // cout<<"split"<<endl;
    // for(int j=2;j<=11;j++)
    // {
    //     State s=numbertostate(24,j);
    //     cout<<splitreward(s,1,p)<<" ";

    // }
    // cout<<endl;
    // for(int i=1;i<=8;i++)
    // {
    //     cout<<i<<",2="<<freward[i][2]<<endl;
    // }
    // cout<<"16,2="<<freward[16][2]<<endl;
}