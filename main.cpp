#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;
using namespace sf;



class Card
{
public:
	int suit, value;
};

Card cards[52];
Card p[4][13];
vector<Card>deck;
vector<Card>players[4];

int num[4];
int cut[4];
char DC[4];
bool dashcall[4];
bool called[4];
int maxnum;
int maxcut;
int maxcaller;
int callChoiseNum;
int callChoiseSuit;
bool validCall=false;
int selectedNum=4;
int selectedSuit=0;
Card playedCards[4];
Card lastPlayedCards[4];
int whoTurn;
int whoStart;
int cutSuit;
int AISuitCount[4];
int callTotal=0;
int cardChoiseNum;
int suitCount[4][4];
int collected[4];
int roundCounter;
int roundsCounter;
unordered_map<int, int> score;
unordered_set<int> winners;
unordered_set<int> losers;



bool sortCompare(Card& a, Card& b)
{
    if ((a.suit==2 && b.suit==3)||(b.suit==2 && a.suit==3))
        return ((a.suit>b.suit));
    else if ((a.suit==2 && b.suit<2)||(b.suit==2 && a.suit<2))
        return ((a.suit<b.suit));
    else
        return (a.suit<b.suit || (a.suit==b.suit && a.value>b.value));
}
void deckSort();
void random();
void check();

void callStart();
void callOfCut();
void call();
void dash();

int bigWin();
void drawPlay();
void drawMain();
void drawPlayers();
void AICall(int turn);
void AIConfirm(int turn);
void AI(int turn);
void suitCounter();
void scoreCalc();


Vector2i windowDim(1280, 720);
VideoMode desktopMode = VideoMode::getDesktopMode();
Vector2i welcomeWindowPos((desktopMode.width - windowDim.x) / 2, (desktopMode.height - windowDim.y) / 2);

RenderWindow window(VideoMode(windowDim.x, windowDim.y), "2048", Style::None);
//RenderWindow window(VideoMode(desktopMode.width, desktopMode.height), "2048", Style::Fullscreen);
//View view(FloatRect(0, 0, 1280, 720));

Sprite startbg;
Sprite playbg;
Sprite deckSprite[4][15];
Sprite playerAvatar;
Sprite botAvatar;
Sprite dashcallSprite;
Sprite callSprite;
Sprite confirmSprite;
Sprite numbersSprite[14];
Sprite suitsSprite[5];
Sprite selectedSprite;


Texture startbgTexture;
Texture playbgTexture;
Texture deckTexture[4][15];
Texture playerAvatarTexture;
Texture botAvatarTexture;
Texture dashcallTexture;
Texture callTexture;
Texture confirmTexture;
Texture numbersTexture[14];
Texture suitsTexture[5];
Texture selectedTexture;


Font font;
Text roundState;



int windowSelect=1;
int windowlastSelected=0;





int main()
{
    //view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    //window.setView(view);
    srand(time(NULL));
    window.setPosition(welcomeWindowPos);
    if (!(startbgTexture.loadFromFile("resources/startbg.jpg")&&
          playbgTexture.loadFromFile("resources/playbg.jpg")&&
          playerAvatarTexture.loadFromFile("resources/HumanAvatar.png")&&
          botAvatarTexture.loadFromFile("resources/BotAvatar.png")&&
          dashcallTexture.loadFromFile("resources/dashcall.png")&&
          callTexture.loadFromFile("resources/call.png")&&
          confirmTexture.loadFromFile("resources/callconfirm.png")&&
          suitsTexture[3].loadFromFile("resources/spad.png")&&
          suitsTexture[2].loadFromFile("resources/heart.png")&&
          suitsTexture[1].loadFromFile("resources/caro.png")&&
          suitsTexture[0].loadFromFile("resources/triffle.png")&&
          suitsTexture[4].loadFromFile("resources/suns.png")&&
          selectedTexture.loadFromFile("resources/selected.png")&&
          font.loadFromFile("resources/comicbd.ttf")))
        return EXIT_FAILURE;
    for (int i=0; i<4; i++)
    {
        char c='s';
        String s;
        if (i==1) c='h';
        if (i==2) c='c';
        if (i==3) c='t';
        for (int j=2; j<15; j++)
        {
            s ="resources/" + to_string(j);
            s += c;
            s += ".png";
            if (!(deckTexture[i][j].loadFromFile(s)))
                return EXIT_FAILURE;
            deckSprite[i][j].setTexture(deckTexture[i][j]);
            deckSprite[i][j].setScale(0.08,0.08);
        }
    }
    for (int i=0; i<14; i++)
    {
        String s;
        s ="resources/" + to_string(i);
        s += ".png";
        if (!(numbersTexture[i].loadFromFile(s)))
            return EXIT_FAILURE;
        numbersSprite[i].setTexture(numbersTexture[i]);
    }
    for (int i=0; i<5; i++)
    {
            suitsSprite[i].setTexture(suitsTexture[i]);
    }
    selectedSprite.setTexture(selectedTexture);

    startbg.setTexture(startbgTexture);
    playbg.setTexture(playbgTexture);
    playerAvatar.setTexture(playerAvatarTexture);
    botAvatar.setTexture(botAvatarTexture);
    playerAvatar.setScale(0.277,0.277);
    botAvatar.setScale(0.222,0.222);
    dashcallSprite.setTexture(dashcallTexture);
    callSprite.setTexture(callTexture);
    confirmSprite.setTexture(confirmTexture);
    roundState.setFont(font);
    roundState.setCharacterSize(24);
    //roundState.setStyle(sf::Text::Bold);
    roundState.setFillColor(Color::White);
    roundState.setPosition(1200, 680);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            // Window close button or escape key is pressed: it will exit
            if ((event.type == sf::Event::Closed) ||
               ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
                window.close();
            if (event.type == sf::Event::MouseButtonPressed&&
                event.mouseButton.button == sf::Mouse::Left)
            {
                if  ((((sf::Mouse::getPosition(window).x >= 0 && sf::Mouse::getPosition(window).x <= 1280)&&
                    (sf::Mouse::getPosition(window).y >= 0 && sf::Mouse::getPosition(window).y <= 720))&&
                      windowSelect==1))
                {
                    roundsCounter=0;
                    random();
                    callStart();
                    windowSelect=3;
                    playedCards[0].value=0;
                    playedCards[1].value=0;
                    playedCards[2].value=0;
                    playedCards[3].value=0;
                    collected[0]=0;
                    collected[1]=0;
                    collected[2]=0;
                    collected[3]=0;
                    score[1] = 0;
                    score[2] = 0;
                    score[3] = 0;
                    score[4] = 0;
                    cout<<sf::Mouse::getPosition(window).x<<", "<<sf::Mouse::getPosition(window).y;
                }
                else if  (windowSelect==3)
                {
                    maxnum==3;
                    maxcut==0;
                    roundCounter=0;
                    random();
                    callStart();
                    windowSelect=3;
                    playedCards[0].value=0;
                    playedCards[1].value=0;
                    playedCards[2].value=0;
                    playedCards[3].value=0;
                    if  ((((sf::Mouse::getPosition(window).x >= 380 && sf::Mouse::getPosition(window).x <= 650)&&
                    (sf::Mouse::getPosition(window).y >= 348 && sf::Mouse::getPosition(window).y <= 508))))
                    {
                        dashcall[0]=true;
                        num[0]=0;
                        windowSelect=2;
                        called[0] = false;
                        AICall(1);
                        AICall(2);
                        AICall(3);
                        selectedSuit=maxcut;
                        //windowSelect=5;
                        whoStart=maxcaller;
                        whoTurn=maxcaller;
                        suitCounter();
                        if (maxnum==3 && maxcut==0)
                        {
                            random();
                            callStart();
                            windowSelect=3;
                            playedCards[0].value=0;
                            playedCards[1].value=0;
                            playedCards[2].value=0;
                            playedCards[3].value=0;
                        }
                        callTotal=0;
                        callTotal+=maxnum;
                        if (maxcaller!=1) AIConfirm(1);
                        if (maxcaller!=2) AIConfirm(2);
                        if (maxcaller!=3) AIConfirm(3);
                        selectedSuit=maxcut;
                        if ((maxcaller == 1 && (callTotal == 13)))
                        {
                            if ((num[3]+1) <= maxnum)
                            {
                                callTotal -= num[3];
                                callTotal += (num[3]+1);
                                num[3] = (num[3]+1);
                            }
                            else if ((num[3]-1) <= maxnum)
                            {
                                callTotal -= num[3];
                                callTotal += (num[3]-1);
                                num[3] = (num[3]-1);
                            }
                            else if (callTotal+ maxnum !=13)
                            {
                                callTotal -= num[3];
                                callTotal += maxnum;
                                num[3] = maxnum;
                            }
                            else
                            {
                                callTotal -= num[3];
                                callTotal += (maxnum-1);
                                num[3] = (maxnum-1);
                            }
                        }
                    }
                    else if  ((((sf::Mouse::getPosition(window).x >= 659 && sf::Mouse::getPosition(window).x <= 929)&&
                    (sf::Mouse::getPosition(window).y >= 348 && sf::Mouse::getPosition(window).y <= 508))))
                    {
                        dashcall[0]=false;
                        windowSelect=4;
                    }
                }
                else if  (windowSelect==4)
                {
                    validCall=false;
                    for (int i=4; i<14; i++)
                    {
                        if (numbersSprite[i].getGlobalBounds().contains(Mouse::getPosition(window).x, Mouse::getPosition(window).y))
                        {
                            callChoiseNum=i;
                        }
                    }
                    for (int i=0; i<5; i++)
                    {
                        if (suitsSprite[i].getGlobalBounds().contains(Mouse::getPosition(window).x, Mouse::getPosition(window).y))
                        {
                            callChoiseSuit=i;
                        }
                    }
                    if (callChoiseNum > maxnum || (callChoiseNum == maxnum && callChoiseSuit > maxcut))
                    {
                        validCall=true;
                        selectedNum=callChoiseNum;
                        selectedSuit=callChoiseSuit;
                    }
                    if (validCall==true &&
                        (sf::Mouse::getPosition(window).x >= 674 && sf::Mouse::getPosition(window).x <= 877)&&
                        (sf::Mouse::getPosition(window).y >= 440 && sf::Mouse::getPosition(window).y <= 515))
                    {
                        num[0] = callChoiseNum;
                        cut[0] = callChoiseSuit;
                        maxnum = num[0];
                        maxcut = cut[0];
                        maxcaller = 0;
                        called[0] = true;
                        AICall(1);
                        AICall(2);
                        AICall(3);
                        if (called[0]==true && maxcaller==0)
                        {
                            callTotal+=maxnum;
                            AIConfirm(1);
                            AIConfirm(2);
                            AIConfirm(3);
                            windowSelect=2;
                        }
                        whoStart=maxcaller;
                        whoTurn=maxcaller;
                        suitCounter();
                    }
                    if ((sf::Mouse::getPosition(window).x >= 440 && sf::Mouse::getPosition(window).x <= 643)&&
                        (sf::Mouse::getPosition(window).y >= 440 && sf::Mouse::getPosition(window).y <= 515))
                    {
                        called[0] = false;
                        AICall(1);
                        AICall(2);
                        AICall(3);
                        selectedSuit=maxcut;
                        windowSelect=5;
                        whoStart=maxcaller;
                        whoTurn=maxcaller;
                        suitCounter();
                        if (maxnum==3 && maxcut==0)
                        {
                            random();
                            callStart();
                            windowSelect=3;
                            playedCards[0].value=0;
                            playedCards[1].value=0;
                            playedCards[2].value=0;
                            playedCards[3].value=0;
                            windowSelect=3;
                        }
                    }

                }
                else if  (windowSelect==5)
                {
                    validCall=false;
                    callTotal=0;
                    callTotal+=maxnum;
                    if (maxcaller!=1) AIConfirm(1);
                    if (maxcaller!=2) AIConfirm(2);
                    if (maxcaller!=3) AIConfirm(3);
                    selectedSuit=maxcut;
                    for (int i=0; i<14; i++)
                    {
                        if (numbersSprite[i].getGlobalBounds().contains(Mouse::getPosition(window).x, Mouse::getPosition(window).y))
                        {
                            callChoiseNum=i;
                        }
                    }
                    if ((callChoiseNum <= maxnum))
                    {
                        if (!(maxcaller == 1 && (callTotal+callChoiseNum == 13)))
                        {
                            validCall=true;
                            selectedNum=callChoiseNum;
                            selectedSuit=maxcut;
                        }
                    }
                    if (validCall==true &&
                        (sf::Mouse::getPosition(window).x >= 674 && sf::Mouse::getPosition(window).x <= 877)&&
                        (sf::Mouse::getPosition(window).y >= 440 && sf::Mouse::getPosition(window).y <= 515))
                    {
                        callTotal += callChoiseNum;
                        num[0] = callChoiseNum;
                        called[0] = true;
                        windowSelect=2;
                    }

                }
                else if (windowSelect==2 && (whoTurn%4)==0)
                {
                    for (int i=0; i<13; i++)
                    {
                        if (players[0][i].value==0) continue;
                        if (deckSprite[players[0][i].suit][players[0][i].value].getGlobalBounds().contains(Mouse::getPosition(window).x, Mouse::getPosition(window).y))
                        {
                            if (whoStart==0 || (whoStart!=0 && playedCards[whoStart].suit==players[0][i].suit) || (whoStart!=0 && suitCount[0][playedCards[whoStart].suit]==0))
                            {
                                cardChoiseNum=i;
                                playedCards[0].suit=players[0][i].suit;
                                playedCards[0].value=players[0][i].value;
                                whoTurn++;
                                players[0][i].value=0;
                            }
                        }
                    }
                }
            }

        }
        if (windowSelect==1)
        {
            drawMain();
        }
        else if (windowSelect>1)
        {
            //drawPlay();
            if (windowSelect==2)
            {
                if (whoTurn%4==1 && playedCards[1].value==0) AI(1);
                if (whoTurn%4==2 && playedCards[2].value==0) AI(2);
                if (whoTurn%4==3 && playedCards[3].value==0) AI(3);

                if (playedCards[0].value!=0 && playedCards[1].value!=0 && playedCards[2].value!=0 && playedCards[3].value!=0)
                {
                    cout<<endl<<"played cards: "<<playedCards[0].value<<", "<<playedCards[1].value<<", "<<playedCards[2].value<<", "<<playedCards[3].value<<endl;
                    drawPlay();
                    /*if (whoTurn%4==0)*/sf::sleep(sf::milliseconds(3000));
                    whoStart=bigWin();
                    whoTurn=whoStart;
                    collected[whoStart]++;
                    suitCounter();
                    cout<<endl<<"win cards: "<<whoStart<<endl;
                    roundCounter++;
                    if (roundCounter==13)
                    {
                        scoreCalc();
                        roundsCounter++;
                        windowSelect=3;
                    }
                    if (roundsCounter==13)
                    {
                        windowSelect=1;
                    }
                }
            }
            drawPlay();
        }
        /*window.clear();
        window.draw(startbg);
        window.display();*/
    }
    return EXIT_SUCCESS;
}


void deckSort() {
    int j = 0;
	for (int i = 2; i < 15; i++)
	{

		cards[j].suit = 0;
		cards[j].value = i;
		deck.push_back(cards[j]);
		j++;
	}
	j = 0;
	for (int i = 2; i < 15; i++)
	{
		cards[j].suit = 1;
		cards[j].value = i;
		deck.push_back(cards[j]);
		j++;
	}
	j = 0;
	for (int i = 2; i < 15; i++)
	{
		cards[j].suit = 2;
		cards[j].value = i;
		deck.push_back(cards[j]);
		j++;
	}
	j = 0;
	for (int i = 2; i < 15; i++)
	{
		cards[j].suit = 3;
		cards[j].value = i;
		deck.push_back(cards[j]);
		j++;
	}
}

void random() {
    int x = 52;
    for (int j = 0; j < 4; j++)
    players[j].clear();
	deckSort();
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 13; i++) {

			int n = rand() % x;
			p[j][i].value = deck[n].value;
			p[j][i].suit = deck[n].suit;
			players[j].push_back(p[j][i]);
			cout << "player " << j + 1 << "    " << deck[n].value << " " << deck[n].suit << endl;
			deck.erase(deck.begin() + n);
			x--;
		}

	}
	for (int j = 0; j < 4; j++)
        sort(players[j].begin(), players[j].end(), sortCompare);
	deck.clear();
	check();
}

void check() {
	for (int j = 0; j < 4; j++)
	{
		bool suits[4] = {};
		int cnt = 0;
		for (int i = 0; i < 13; i++) {
			suits[players[j][i].suit] = 1;
		}
		for (int i = 0; i < 4; i++)
		{
			if (suits[i])
				cnt++;
		}
		if (cnt<3)
        {
            players[j].clear();
            random();
        }

	}
}

void callStart() {
	for (int i = 0; i < 4; i++) {
		num[i] = 3;
	}
	for (int i = 0; i < 4; i++) {
		dashcall[i] = false;
	}
	maxcut = 0;
	maxnum = 3;
}

void callOfCut() {
	char choise[4];
	bool cutfound = false;
	for (int i = 0; i < 4; i++) {
		if (dashcall[i] == false) {
			cout << "player " << i + 1 << " : call (C) or pass (p)" << endl;
			cin >> choise[i];
			if (choise[i] == 'C' || choise[i] == 'c') {
				cout << "now enter the number and cut {S->H->D->C}" << endl;
				cin >> num[i] >> cut[i];
				cutfound = true;
				if (num[i] > maxnum) {
					maxnum = num[i];
					maxcut = cut[i];
				}
				else if (num[i] == maxnum) {
					if (cut[i] > maxcut)
						maxcut = cut[i];

				}
				else {
					cout << "enter number greater than or equal " << maxnum << endl;
					i = i - 1;
				}

			}
			else if (choise[i] == 'p' || choise[i] == 'P') {
				continue;
			}
		}

	}
}

void call() {
	cout << "Now the cut is " << maxcut << endl;
	for (int i = 0; i < 4; i++) {
		if (dashcall[i] == false) {
			cout << "Player " << i + 1 << " :Choose number from " << maxnum << " to 13 " << endl;
			cin >> num[i];
		}
	}
}

/*void dash() {
	int count = 0;
	for (int i = 0; i < 4; i++) {
		cout << "Player " << i + 1 << " Dash Call (Yes or No)?" << endl;
		cin >> DC[i];
		if (DC[i] == 'd' || DC[i] == 'D') {
			count++;
			dashcall[i] = true;
		}

	}
	if (count >= 3) {
		system("cls");
		random();
		dash();
	}
}*/

int bigWin(){
	int winCard = 0, playSuit = playedCards[0].suit;
	bool isCut = 0;
	for (int i = 1; i < 4; i++)
	{
		if (!isCut && playedCards[i].value > playedCards[winCard].value && playedCards[i].suit == playSuit)
		{
			winCard = i;
		}
		else if (playedCards[i].suit == maxcut)
		{
			if (!isCut || (isCut && playedCards[i].value > playedCards[winCard].value))
			{
				winCard = i;
			}
		}
	}
	playedCards[0].value=0;
    playedCards[1].value=0;
    playedCards[2].value=0;
    playedCards[3].value=0;
	return winCard;
}

void drawPlay(){
    int xPos=65;
    int yPos=600;
    window.clear();
    window.draw(playbg);
    for (int i=0; i<13; i++)
    {
        xPos = deckSprite[0][14].getGlobalBounds().width * i + 65;
        deckSprite[players[0][i].suit][players[0][i].value].setPosition(xPos, yPos);
        window.draw(deckSprite[players[0][i].suit][players[0][i].value]);
    }
    deckSprite[playedCards[0].suit][playedCards[0].value].setPosition(595, 380);
    window.draw(deckSprite[playedCards[0].suit][playedCards[0].value]);
    deckSprite[playedCards[1].suit][playedCards[1].value].setPosition(690, 295);
    window.draw(deckSprite[playedCards[1].suit][playedCards[1].value]);
    deckSprite[playedCards[2].suit][playedCards[2].value].setPosition(595, 210);
    window.draw(deckSprite[playedCards[2].suit][playedCards[2].value]);
    deckSprite[playedCards[3].suit][playedCards[3].value].setPosition(500, 295);
    window.draw(deckSprite[playedCards[3].suit][playedCards[3].value]);

    drawPlayers();
    if (windowSelect==3)
        {
            window.draw(dashcallSprite);
        }
    else if (windowSelect==4)
        {
            int xPos=615;
            int yPos=180;
            window.draw(callSprite);
            for (int i=4; i<7; i++)
            {
                xPos += numbersSprite[i].getGlobalBounds().width;
                numbersSprite[i].setPosition(xPos, yPos);
                if (i == selectedNum)
                {
                    selectedSprite.setPosition(xPos, yPos);
                    window.draw(selectedSprite);
                }
                window.draw(numbersSprite[i]);
            }
            xPos=315;
            yPos=256;
            for (int i=7; i<14; i++)
            {
                xPos += numbersSprite[i].getGlobalBounds().width;
                numbersSprite[i].setPosition(xPos, yPos);
                if (i == selectedNum)
                {
                    selectedSprite.setPosition(xPos, yPos);
                    window.draw(selectedSprite);
                }
                window.draw(numbersSprite[i]);
            }
            xPos=335;
            yPos=345;
            for (int i=4; i>-1; i--)
            {
                xPos += ((suitsSprite[1].getGlobalBounds().width)+20);
                suitsSprite[i].setPosition(xPos, yPos);
                if (i == selectedSuit)
                {
                    selectedSprite.setPosition(xPos, yPos);
                    window.draw(selectedSprite);
                }
                window.draw(suitsSprite[i]);
            }
        }
    else if (windowSelect==5)
        {
            window.draw(confirmSprite);
            int xPos=315;
            int yPos=180;
            for (int i=0; i<7; i++)
            {
                xPos += numbersSprite[i].getGlobalBounds().width;
                numbersSprite[i].setPosition(xPos, yPos);
                if (i == selectedNum)
                {
                    selectedSprite.setPosition(xPos, yPos);
                    window.draw(selectedSprite);
                }
                window.draw(numbersSprite[i]);
            }
            xPos=315;
            yPos=256;
            for (int i=7; i<14; i++)
            {
                xPos += numbersSprite[i].getGlobalBounds().width;
                numbersSprite[i].setPosition(xPos, yPos);
                if (i == selectedNum)
                {
                    selectedSprite.setPosition(xPos, yPos);
                    window.draw(selectedSprite);
                }
                window.draw(numbersSprite[i]);
            }
            xPos=335;
            yPos=345;
            for (int i=4; i>-1; i--)
            {
                xPos += ((suitsSprite[1].getGlobalBounds().width)+20);
                suitsSprite[i].setPosition(xPos, yPos);
                if (i == selectedSuit)
                {
                    selectedSprite.setPosition(xPos, yPos);
                    window.draw(selectedSprite);
                }
                window.draw(suitsSprite[i]);
            }
        }
    /*else if (windowSelect==2)
    {
        if (callTotal>13) roundState.setString("+");
        else roundState.setString("+");
        roundState.setString("a");
        window.draw(roundState);
    }*/
    window.display();
}

void drawMain(){
    window.clear();
    window.draw(startbg);
    //window.draw(deckSprite[3][14]);
    window.display();
}

void drawPlayers(){
    playerAvatar.setPosition(0,0);
    window.draw(playerAvatar);
    botAvatar.setPosition(560,0);
    window.draw(botAvatar);
    botAvatar.setPosition(280,280);
    window.draw(botAvatar);
    botAvatar.setPosition(840,280);
    window.draw(botAvatar);
}

void AICall(int turn){
    int suits[4] = {};
    Card maxCall;
    maxCall.suit=0;
    maxCall.value=0;
    float maxCallValueFloat=0;
    Card bestSuit;
    int suitsPoints[4] = {};
    bestSuit.suit=0;
    bestSuit.value=0;
    int longSuit=0;
    for (int i = 0; i < 13; i++) {
        suits[players[turn][i].suit]++;
        if (suits[players[turn][i].suit]>longSuit)
        {
            longSuit=suits[players[turn][i].suit];
            maxCall.suit=players[turn][i].suit;
        }

        if (players[turn][i].value==14)
        {
            maxCallValueFloat+=1.5;
            suitsPoints[players[turn][i].suit]+=4;
        }
        else if (players[turn][i].value==13)
        {
            maxCallValueFloat+=1;
            suitsPoints[players[turn][i].suit]+=3;
        }
        else if (players[turn][i].value==12)
        {
            maxCallValueFloat+=0.5;
            suitsPoints[players[turn][i].suit]+=2;
        }
        else if (players[turn][i].value==11)
        {
            maxCallValueFloat+=0.25;
            suitsPoints[players[turn][i].suit]+=1;
        }

        if (suitsPoints[players[turn][i].suit]>bestSuit.value)
        {
            bestSuit.value=suitsPoints[players[turn][i].suit];
            bestSuit.suit=players[turn][i].suit;
        }

    }

    if (longSuit>5 && longSuit==bestSuit.suit)
    {
        maxCall.suit=bestSuit.suit;
        maxCall.value+=2;
    }

    maxCall.value=maxCallValueFloat;

    if (maxCall.value > maxnum || (maxCall.value == maxnum && maxCall.suit > maxcut))
    {
        //cout<<"fucking test"<<endl;
        num[turn] = maxCall.value;
        cut[turn] = maxCall.suit;
        maxnum = num[turn];
        maxcut = cut[turn];
        maxcaller = turn;
    }
    cout<<endl<<maxCall.value<<" , "<<maxCall.suit<<" , "<<maxnum<<" , "<<maxcut<<" , "<<maxcaller<<endl;
}

void AIConfirm(int turn){
    int suits[4] = {};
    Card maxCall;
    maxCall.suit=0;
    maxCall.value=0;
    float maxCallValueFloat=0;
    Card bestSuit;
    int suitsPoints[4] = {};
    bestSuit.suit=0;
    bestSuit.value=0;
    int longSuit=0;
    for (int i = 0; i < 13; i++) {
        suits[players[turn][i].suit]++;
        if (suits[players[turn][i].suit]>longSuit)
        {
            longSuit=suits[players[turn][i].suit];
            maxCall.suit=players[turn][i].suit;
        }

        if (players[turn][i].value==14)
        {
            maxCallValueFloat+=1.5;
            suitsPoints[players[turn][i].suit]+=4;
        }
        else if (players[turn][i].value==13)
        {
            maxCallValueFloat+=1;
            suitsPoints[players[turn][i].suit]+=3;
        }
        else if (players[turn][i].value==12)
        {
            maxCallValueFloat+=0.75;
            suitsPoints[players[turn][i].suit]+=2;
        }
        else if (players[turn][i].value==11)
        {
            maxCallValueFloat+=0.5;
            suitsPoints[players[turn][i].suit]+=1;
        }

        if (suitsPoints[players[turn][i].suit]>bestSuit.value)
        {
            bestSuit.value=suitsPoints[players[turn][i].suit];
            bestSuit.suit=players[turn][i].suit;
        }

    }

    if (longSuit>5 && longSuit==maxcut)
    {
        maxCall.value+=2;
    }

    maxCall.value=maxCallValueFloat;
    if (maxcaller!=(turn+1)%4)
    {
        if (maxCall.value <= maxnum)
        {
            callTotal += maxCall.value;
            num[turn] = maxCall.value;
        }
        else
        {
            callTotal += maxnum;
            num[turn] = maxnum;
        }
    }
    else
    {
        if (maxCall.value <= maxnum && callTotal+ maxCall.value !=13)
        {
            callTotal += maxCall.value;
            num[turn] = maxCall.value;
        }
        else if ((maxCall.value+1) <= maxnum)
        {
            callTotal += (maxCall.value+1);
            num[turn] = (maxCall.value+1);
        }
        else if ((maxCall.value-1) <= maxnum)
        {
            callTotal += (maxCall.value-1);
            num[turn] = (maxCall.value-1);
        }
        else if (callTotal+ maxnum !=13)
        {
            callTotal += maxnum;
            num[turn] = maxnum;
        }
        else
        {
            callTotal += (maxnum-1);
            num[turn] = (maxnum-1);
        }
    }
    cout<<endl<<maxCall.value<<" , "<<maxCall.suit<<" , "<<maxnum<<" , "<<maxcut<<" , "<<maxcaller<<endl;
}

void AI(int turn){
    if (true)
    {
        suitCounter();
        int i;
        while(true)
        {
            cout<<endl<<turn<<" Hit a Dead End "<<players[turn%4][i].suit<<"  "<<players[turn%4][i].value<<endl;
            i=rand()%13;
            if (players[turn%4][i].value==0) continue;
            if (whoStart==turn%4 || (whoStart!=turn%4 && playedCards[whoStart].suit==players[turn%4][i].suit) || (whoStart!=turn%4 && suitCount[turn%4][playedCards[whoStart].suit]==0))
            {
                cardChoiseNum=i;
                playedCards[turn%4].suit=players[turn%4][i].suit;
                playedCards[turn%4].value=players[turn%4][i].value;
                whoTurn++;
                players[turn%4][i].value=0;
                cout<<endl<<turn<<" played "<<players[turn%4][i].suit<<"  "<<players[turn%4][i].value<<endl;
                break;
            }
        }
    }
    suitCounter();
    /*if (callTotal>13)
        for (int i=0;i<13;i++)
        {
            cout<<endl<<turn<<" try "<<players[turn%4][i].suit<<"  "<<players[turn%4][i].value<<endl;
            if (players[turn%4][i].value==0) continue;
            cout<<endl<<(whoStart==turn%4)<<" condition 1 "<<(whoStart!=turn%4 && playedCards[whoStart].suit==players[turn%4][i].suit)<<" condition 1 "<<(whoStart!=turn%4 && suitCount[turn%4][players[turn%4][i].suit]==0)<<endl;
            cout<<endl<<whoStart<<" who Start "<<turn%4<<" turn% "<< playedCards[whoStart].suit <<" who Start suit "<<players[turn%4][i].suit<<" player suit "<<suitCount[turn%4][players[turn%4][i].suit]<<" suit count "<<endl;
            if (whoStart==turn%4 || (whoStart!=turn%4 && playedCards[whoStart].suit==players[turn%4][i].suit) || (whoStart!=turn%4 && suitCount[turn%4][players[turn%4][i].suit]==0))
            {
                cardChoiseNum=i;
                playedCards[turn%4].suit=players[turn%4][i].suit;
                playedCards[turn%4].value=players[turn%4][i].value;
                whoTurn++;
                players[turn%4][i].value=0;
                cout<<endl<<turn<<" played "<<players[turn%4][i].suit<<"  "<<players[turn%4][i].value<<endl;
                break;
            }
        }
    else
        for (int i=12;i>-1;i--)
        {
            cout<<endl<<turn<<" try "<<players[turn%4][i].suit<<"  "<<players[turn%4][i].value<<endl;
            if (players[turn%4][i].value==0) continue;
            cout<<endl<<(whoStart==turn%4)<<" condition 1 "<<(whoStart!=turn%4 && playedCards[whoStart].suit==players[turn%4][i].suit)<<" condition 1 "<<(whoStart!=turn%4 && suitCount[turn%4][players[turn%4][i].suit]==0)<<endl;
            cout<<endl<<whoStart<<" who Start "<<turn%4<<" turn% "<< playedCards[whoStart].suit <<" who Start suit "<<players[turn%4][i].suit<<" player suit "<<suitCount[turn%4][players[turn%4][i].suit]<<" suit count "<<endl;
            if (whoStart==turn%4 || (whoStart!=turn%4 && playedCards[whoStart].suit==players[turn%4][i].suit) || (whoStart!=turn%4 && suitCount[turn%4][players[turn%4][i].suit]==0))
            {
                cardChoiseNum=i;
                playedCards[turn%4].suit=players[turn%4][i].suit;
                playedCards[turn%4].value=players[turn%4][i].value;
                whoTurn++;
                players[turn%4][i].value=0;
                cout<<endl<<turn<<" played "<<players[turn%4][i].suit<<"  "<<players[turn%4][i].value<<endl;
                break;
            }
        }*/
}

void suitCounter(){
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            suitCount[i][j]=0;
    for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 13; i++) {
		    if (players[j][i].value!=0)
			suitCount[j][players[j][i].suit]++;
		}

	}
}

void scoreCalc()
{
    //unordered_map<int, int>::iterator it;
	for (int i=0;i<4;i++)
    {
        if (collected[i]==num[i])
            winners.insert(i);
        else
            losers.insert(i);
    }
	for (auto x : score)
	{
	    if (collected[x.first]==num[x.first])
        {
            x.second+=10+num[x.first];
            if (num[x.first]==maxcut)
                x.second+=10;
            if ((!(winners.find(x.first)==winners.end())) && winners.size()==1)
                x.second+=10;

            if ((dashcall[x.first]==true) && (callTotal>13))
                x.second+=25;
            else
                x.second+=33;
        }
        else
        {
            if (collected[x.first]>num[x.first])
                x.second-=collected[x.first]-num[x.first];
            else
                x.second-=num[x.first]-collected[x.first];
            if (num[x.first]==maxcut)
                x.second-=10;
            if ((!(losers.find(x.first)==losers.end())) && losers.size()==1)
                x.second-=10;

            if ((dashcall[x.first]==true) && (callTotal>13))
                x.second-=25;
            else
                x.second-=33;
        }
	}
	for (int i=0;i<4;i++)
    {
        collected[i]=0;
    }
}



