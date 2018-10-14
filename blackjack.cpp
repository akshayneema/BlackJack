#include <iostream>
#include <cstdlib>
using namespace std;
const int p;
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
    int deal;
}
//beware of soft A

State* NumberToState(int pstateno, int dealercard)//underconstruction
{
    State* s;
    s->pstateno=pstateno;
    s->dealercard=dealercard;
    if(pstateno>=1 && pstateno<=15)
    {
        s->pvalue=pstateno+4;
        s->playercards
    }
}

void statecopy(State* tostate, State* fromstate)
{
    tostate->playerval=fromstate->playerval;
    tostate->playersofta=fromstate->playersofta;
    tostate->playercardlen=fromstate->playercardlen;
    tostate->dealerval=fromstate->dealerval;
    tostate->dealersofta=fromstate->dealersofta;
    tostate->dealercardlen=fromstate->dealercardlen;
    tostate->deal=fromstate->deal;
}
//NEED TO ADD BLACK-JACK FEATURE ALSO
int afterstand(State* s)//expected reward after player stands. only dealer is making moves now.
{
    int reward;
    int rewardface;
    //reward to player face-card
    if(s->dealerval+10>21 && s->dealersofta==0)//dealer busts
        rewardface=s->deal;
    else if(s->dealerval+10>21 && s->dealersofta!=0)//dealer uses soft A
    {
        State* temps;
        statecopy(temps,s);
        temps->dealersofta=temps->dealersofta-1;
        temps->dealercardlen=temps->dealercardlen+1;
        rewardface=afterstand(temps);
    }
    else if(s->dealerval+10<=21)
    {
        if(s->dealerval+10>=17)//stands(controversial)
        {
            if(s->dealerval+10>s->playerval)//player looses
                rewardface=-1*s->deal;
            else if(s->dealerval+10==s->playerval)//PUSH
                rewardface=0;
            else//player wins
                rewardface=1*s->deal;
        }
        else//dealer continues hitting
        {
            State* temps;
            statecopy(temps,s);
            temps->dealerval=temps->dealerval+10;
            temps->dealercardlen=temps->dealercardlen+1;
            rewardface=afterstand(temps);
        }
    }
    reward=p*rewardface;
    //reward to player if non-face card appears(loop-check in sequence)
    // number cards from 2-9 (ace not included)
    for(int i=2;i<=9;i++)
    {
        int rewardnonface;
        if(s->dealerval+i>21 && s->dealersofta==0)//bust
            rewardnonface=s->deal;
        else if(s->dealerval+i>21 && s->dealersofta!=0)//use soft A
        {
            State* temps;
            statecopy(temps,s);
            temps->dealersofta=temps->dealersofta-1;
            temps->dealercardlen=temps->dealercardlen+1;
            temps->dealerval=temps->dealerval+i-10;
            rewardnonface=afterstand(temps);
        }
        else if(s->dealerval+i<=21)
        {
            if(s->dealerval+i>=17)//stands(controversial)
            {
                if(s->dealerval+i>s->playerval)//player looses
                    rewardnonface=-1*s->deal;
                else if(s->dealerval+i==s->playerval)//PUSH
                    rewardnonface=0;
                else//player wins
                    rewardnonface=1*s->deal;
            }
            else//dealer continues hitting
            {
                State* temps;
                statecopy(temps,s);
                temps->dealerval=temps->dealerval+i;
                temps->dealercardlen=temps->dealercardlen+1;
                rewardnonface=afterstand(temps);
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
                rewardace=-1*s->deal;
            else if(s->dealerval+1==s->playerval)//PUSH
                rewardace=0;
            else//player wins
                rewardace=1*s->deal;   
        }
        else//dealer continues hitting
        {
            State* temps;
            statecopy(temps,s);
            temps->dealerval=temps->dealerval+1;
            temps->dealercardlen=temps->dealercardlen+1;
            rewardace=afterstand(temps);
        }
    }
    else
    {
        if(s->dealerval+11>=17)
        {
            if(s->dealerval+11>s->playerval)//player looses
                rewardace=-1*s->deal;
            else if(s->dealerval+11==s->playerval)//PUSH
                rewardace=0;
            else//player wins
                rewardace=1*s->deal;   
        }
        else
        {
            State* temps;
            statecopy(temps,s);
            temps->dealerval=temps->dealerval+1;
            temps->dealercardlen=temps->dealercardlen+1;
            temps->dealersofta=temps->dealersofta+1;
            rewardace=afterstand(temps);
        }
    }
    reward=reward+((1-p)/9)*rewardace;
    return reward;
}