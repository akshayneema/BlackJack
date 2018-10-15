#include <iostream>
#include <cstdlib>
using namespace std;
const int p;
int freward[34][12]; //33x10 array which stores the final rewards
int faction[34][12]; //33x10 array which stores the final actions

struct State {
    // int pstateno; //ranges from 1 to 33 as per problem statement, 1-15 simple cases; 16-23 one card A another some simple card(not face); 24-33 pairs
    // int dealercard;// ranges from 2-11
    // int pvalue;// sum of value of player's card
    // vector<int> playercards; // player's cards
    // vector<int> dealercards;    // dealer's cards
    int playerval; //player's hand value
    int playersofta; // no. of soft A's in player's hand
    int playercardlen;
    int dealerval;
    int dealersofta;
    int dealercardlen;
    bool pair;
};
//beware of soft A

int statetonumber(State* s)//get your state number rn
{
    if(s->playersofta==1)
    {
        if(s->playerval==12)//pair of ace--AA 33
            return 33;
        else//one Ace and other non-face card 16 to 23
        {
            return s->playerval+3;
        }
    }
    else if(s->pair==true)
    {
        return 22+s->playerval/2;//24 to 32
    }
    else
        return s->playerval-4;//simple cases 1 to 15
}

void statecopy(State* tostate, State* fromstate)
{
    tostate->playerval=fromstate->playerval;
    tostate->playersofta=fromstate->playersofta;
    tostate->playercardlen=fromstate->playercardlen;
    tostate->dealerval=fromstate->dealerval;
    tostate->dealersofta=fromstate->dealersofta;
    tostate->dealercardlen=fromstate->dealercardlen;
}
//NEED TO ADD BLACK-JACK FEATURE ALSO
int standreward(State* s, int deal)//expected reward after player stands. only dealer is making moves now.
{
    int reward;
    int rewardface;
    //reward to player face-card
    if(s->dealerval+10>21 && s->dealersofta==0)//dealer busts
        rewardface=deal;
    else if(s->dealerval+10>21 && s->dealersofta!=0)//dealer uses soft A
    {
        State* temps;
        statecopy(temps,s);
        temps->dealersofta=temps->dealersofta-1;
        temps->dealercardlen=temps->dealercardlen+1;
        rewardface=standreward(temps,deal);
    }
    else if(s->dealerval+10<=21)
    {
        if(s->dealerval+10>=17)//stands(controversial)
        {
            if(s->dealerval+10>s->playerval)//player looses
                rewardface=-1*deal;
            else if(s->dealerval+10==s->playerval)//PUSH
                rewardface=0;
            else//player wins
                rewardface=1*deal;
        }
        else//dealer continues hitting
        {
            State* temps;
            statecopy(temps,s);
            temps->dealerval=temps->dealerval+10;
            temps->dealercardlen=temps->dealercardlen+1;
            rewardface=standreward(temps,deal);
        }
    }
    reward=p*rewardface;
    //reward to player if non-face card appears(loop-check in sequence)
    // number cards from 2-9 (ace not included)
    for(int i=2;i<=9;i++)
    {
        int rewardnonface;
        if(s->dealerval+i>21 && s->dealersofta==0)//bust
            rewardnonface=deal;
        else if(s->dealerval+i>21 && s->dealersofta!=0)//use soft A
        {
            State* temps;
            statecopy(temps,s);
            temps->dealersofta=temps->dealersofta-1;
            temps->dealercardlen=temps->dealercardlen+1;
            temps->dealerval=temps->dealerval+i-10;
            rewardnonface=standreward(temps,deal);
        }
        else if(s->dealerval+i<=21)
        {
            if(s->dealerval+i>=17)//stands(controversial)
            {
                if(s->dealerval+i>s->playerval)//player looses
                    rewardnonface=-1*deal;
                else if(s->dealerval+i==s->playerval)//PUSH
                    rewardnonface=0;
                else//player wins
                    rewardnonface=1*deal;
            }
            else//dealer continues hitting
            {
                State* temps;
                statecopy(temps,s);
                temps->dealerval=temps->dealerval+i;
                temps->dealercardlen=temps->dealercardlen+1;
                rewardnonface=standreward(temps,deal);
            }
        }
        reward=reward+((1-p)/9)*rewardnonface;
    }
    //when ace comes out
    int rewardace;
    if(s->dealerval+11>21)//soft ACE leading to busting
    {
        if(s->dealerval+1>=17)//stands(controversial)
        {
            if(s->dealerval+1>s->playerval)//player looses
                rewardace=-1*deal;
            else if(s->dealerval+1==s->playerval)//PUSH
                rewardace=0;
            else//player wins
                rewardace=1*deal;   
        }
        else//dealer continues hitting
        {
            State* temps;
            statecopy(temps,s);
            temps->dealerval=temps->dealerval+1;
            temps->dealercardlen=temps->dealercardlen+1;
            rewardace=standreward(temps,deal);
        }
    }
    else
    {
        if(s->dealerval+11>=17)
        {
            if(s->dealerval+11>s->playerval)//player looses
                rewardace=-1*deal;
            else if(s->dealerval+11==s->playerval)//PUSH
                rewardace=0;
            else//player wins
                rewardace=1*deal;   
        }
        else
        {
            State* temps;
            statecopy(temps,s);
            temps->dealerval=temps->dealerval+11;
            temps->dealercardlen=temps->dealercardlen+1;
            temps->dealersofta=temps->dealersofta+1;
            rewardace=standreward(temps,deal);
        }
    }
    reward=reward+((1-p)/9)*rewardace;
    return reward;
}

//reward calculation when decide to hit
int hitreward(State* s, int deal)
{
    //facecard
    int reward;
    int rewardface;
    if(s->playerval+10>21 && s->playersofta==0)//player busts
        rewardface=-1*deal;
    else if(s->playerval+10>21 && s->playersofta!=0)//player uses soft A
    {
        State* temps;
        statecopy(temps,s);
        temps->playersofta=temps->playersofta-1;
        temps->playercardlen=temps->playercardlen+1;
        rewardface=freward[statetonumber(temps)][s->dealerval];
    }
    else if(s->playerval+10<=21)
    {
            State* temps;
            statecopy(temps,s);
            temps->playerval=temps->playerval+10;
            temps->playercardlen=temps->playercardlen+1;
            rewardface=freward[statetonumber(temps)][s->dealerval];
    }
    reward=p*rewardface;
    //reward to player if non-face card appears(loop-check in sequence)
    // number cards from 2-9 (ace not included)
    for(int i=2;i<=9;i++)
    {
        int rewardnonface;
        if(s->playerval+i>21 && s->playersofta==0)//bust
            rewardnonface=-1*deal;
        else if(s->playerval+i>21 && s->playersofta!=0)//use soft A
        {
            State* temps;
            statecopy(temps,s);
            temps->playersofta=temps->playersofta-1;
            temps->playercardlen=temps->playercardlen+1;
            temps->playerval=temps->playerval+i-10;
            rewardnonface=freward[statetonumber(temps)][s->dealerval];
        }
        else if(s->playerval+i<=21)
        {
            State* temps;
            statecopy(temps,s);
            temps->playerval=temps->playerval+i;
            temps->playercardlen=temps->playercardlen+1;
            rewardnonface=freward[statetonumber(temps)][s->dealerval];
        }
        reward=reward+((1-p)/9)*rewardnonface;
    }
    //when ace comes out
    int rewardace;
    if(s->playerval+11>21)//soft ACE leading to busting
    {
        if(s->playerval+1>21 && s->playersofta==0)//hard ACE leading to busting
        {
            rewardace=-1*deal; 
        }
        else if(s->playerval+1>21 && s->playersofta!=0)
        {
            State* temps;
            statecopy(temps,s);
            temps->playerval=temps->playerval+1-10;
            temps->playercardlen=temps->playercardlen+1;
            temps->playersofta=temps->playersofta-1;
            rewardace=freward[statetonumber(temps)][s->dealerval];
        }
        else//dealer continues hitting
        {
            State* temps;
            statecopy(temps,s);
            temps->playerval=temps->playerval+1;
            temps->playercardlen=temps->playercardlen+1;
            rewardace=freward[statetonumber(temps)][s->dealerval];
        }
    }
    else
    {
        State* temps;
        statecopy(temps,s);
        temps->playerval=temps->playerval+1;
        temps->playercardlen=temps->playercardlen+1;
        temps->playersofta=temps->playersofta+1;
        rewardace=freward[statetonumber(temps)][s->dealerval];
    }
    reward=reward+((1-p)/9)*rewardace;
    return reward;
}

//reward calculation when decide to double
int doublereward(State* s, int deal)
{
    deal=2*deal;
    //facecard
    int reward;
    int rewardface;
    if(s->playerval+10>21 && s->playersofta==0)//player busts
        rewardface=-1*deal;
    else if(s->playerval+10>21 && s->playersofta!=0)//player uses soft A
    {
        State* temps;
        statecopy(temps,s);
        temps->playersofta=temps->playersofta-1;
        temps->playercardlen=temps->playercardlen+1;
        rewardface=standreward(temps,deal);
    }
    else if(s->playerval+10<=21)
    {
            State* temps;
            statecopy(temps,s);
            temps->playerval=temps->playerval+10;
            temps->playercardlen=temps->playercardlen+1;
            rewardface=standreward(temps,deal);
    }
    reward=p*rewardface;
    //reward to player if non-face card appears(loop-check in sequence)
    // number cards from 2-9 (ace not included)
    for(int i=2;i<=9;i++)
    {
        int rewardnonface;
        if(s->playerval+i>21 && s->playersofta==0)//bust
            rewardnonface=-1*deal;
        else if(s->playerval+i>21 && s->playersofta!=0)//use soft A
        {
            State* temps;
            statecopy(temps,s);
            temps->playersofta=temps->playersofta-1;
            temps->playercardlen=temps->playercardlen+1;
            temps->playerval=temps->playerval+i-10;
            rewardnonface=standreward(temps,deal);
        }
        else if(s->playerval+i<=21)
        {
            State* temps;
            statecopy(temps,s);
            temps->playerval=temps->playerval+i;
            temps->playercardlen=temps->playercardlen+1;
            rewardnonface=standreward(temps,deal);
        }
        reward=reward+((1-p)/9)*rewardnonface;
    }
    //when ace comes out
    int rewardace;
    if(s->playerval+11>21)//soft ACE leading to busting
    {
        if(s->playerval+1>21 && s->playersofta==0)//hard ACE leading to busting
        {
            rewardace=-1*deal; 
        }
        else if(s->playerval+1>21 && s->playersofta!=0)
        {
            State* temps;
            statecopy(temps,s);
            temps->playerval=temps->playerval+1-10;
            temps->playercardlen=temps->playercardlen+1;
            temps->playersofta=temps->playersofta-1;
            rewardace=standreward(temps,deal);
        }
        else//dealer continues hitting
        {
            State* temps;
            statecopy(temps,s);
            temps->playerval=temps->playerval+1;
            temps->playercardlen=temps->playercardlen+1;
            rewardace=standreward(temps,deal);
        }
    }
    else
    {
        State* temps;
        statecopy(temps,s);
        temps->playerval=temps->playerval+1;
        temps->playercardlen=temps->playercardlen+1;
        temps->playersofta=temps->playersofta+1;
        rewardace=standreward(temps,deal);
    }
    reward=reward+((1-p)/9)*rewardace;
    return reward;
}

//reward when splits
int splitreward(State* s, int deal)
{
    //when pair is non-ace i.e. 2-2,.........10-10
    if(statetonumber(s)>=24 && statetonumber(s)<=32)
    {
        // if a face-card comes up
        int reward;
        int rewardface;
        State* temps;
        statecopy(temps,s);
        temps->playerval=temps->playerval/2+10;
        rewardface=freward[statetonumber(temps)][temps->dealerval];
        reward=p*rewardface;
        // if a non-face card comes up
        for(int i=2;i<=9;i++)
        {
            int rewardnonface;
            statecopy(temps,s);
            temps->playerval=temps->playerval/2+i;
            rewardnonface=freward[statetonumber(temps)][temps->dealerval];
            reward=reward+((1-p)/9)*rewardnonface;
        }
        //if ACE comes up
        int rewardace;
        statecopy(temps,s);
        temps->playerval=temps->playerval/2+11;
        temps->playersofta=temps->playersofta+1;
        rewardace=freward[statetonumber(temps)][temps->dealerval];
        reward=reward+((1-p)/9)*rewardace;
        return 2*reward;//because there are 2 hands like these
    }
    else//KAAFI EXCEPTIONS. IN SHORT SIRF STAND KR SKTE HAI.
    {
        // if a face-card comes up
        int reward;
        int rewardface;
        State* temps;
        statecopy(temps,s);
        temps->playerval=11+10;//-----IS THIS BLACK-JACK??? NO!! IT'S AN EXCEPTION.
        rewardface=standreward(temps,deal);
        reward=p*rewardface;
        // if a non-face card comes up
        for(int i=2;i<=9;i++)
        {
            int rewardnonface;
            statecopy(temps,s);
            temps->playerval=11+i;
            rewardnonface=standreward(temps,deal);
            reward=reward+((1-p)/9)*rewardnonface;
        }
        //if ACE comes up
        int rewardace;
        statecopy(temps,s);
        temps->playerval=11+1;
        rewardace=standreward(temps,deal);
        reward=reward+((1-p)/9)*rewardace;
        return 2*reward;//because there are 2 hands like these
    }
} 