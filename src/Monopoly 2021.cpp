//============================================================================
// Name        : Monopoly.cpp
// Author      : MATT
// Version     :
// Copyright   :
// Description : Game in C, why
//============================================================================

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <time.h>
#include <math.h>
#include <string.h>

using namespace sf;

//RenderWindow window(VideoMode(1400,934), "Monopoly Gen 4");

sf::RenderWindow window(sf::VideoMode(1200,819),"Monopoly 2021");
//sf::RenderWindow windowProperty(sf::VideoMode(400,400),"Buy Property");
//sf::RenderWindow Win2(sf::VideoMode(200,200),"Window 2");

sf::Font Courior;
sf::Font Calibri;

void StartUpPropertyName(void);		//Get and Set Name for Properties
void StartUpPropertyData(void);		//Get and Set Property Number Data
void StartUpPropertySpec(void);		//Set Draw Details for Property
void StartUpGameBoard(void);		//Set Draw Details for other bit of the GameBoard
void StartUpPropertyDeed(void);		//Set up Property Deeds, might try into other StartUp's for some details
void DrawGameBoard(void);			//SFML Draw Calls for Main Game Board
void DrawBuildBoard(void);			//SFML Draw Calls for Building Menu
void DrawTradeBoard(void);			//SFML Draw Calls for Trading Menu
void TileNameToTextBox(void);		//WIP for cosmetic upgrade to Property Name Data
void StartUpPlayer(void);			//Setup for Player Number&Drawing Data
void PlayerMovement(void);		//Animate Players moving around gameboard

int ButtonHandler(void);			//IN:Global Variables	::start of code clean up
									//OUT:Returns button number clicked, no button default to '-1' output

void PlayerActionHandler(int);		//IN:Global Variables, local passed in button clicked
									//OUT:Update multiple Global Variables based on States, Player, Frames

void PropertyHandler(void);			//IN:Global Variables
									//OUT:Update NoticeBoardState

void TradeOfferBuilder(int);		//IN:Button Pressed
									//OUT:Build Trade offer, edits global variables

int RollDice(void);					//IN:Active Player Global Variable
									//OUT:DiceTotal, Update Both Global Dice Values, Player Rolled Double Bool

int AI_TURN(void);					//IN:Global Variables
									//OUT:Returns button number clicked, no button default to '-1' output

void SaveGame(void);				//Save Game to TXT File
void LoadGame(void);				//Load Game from TXT File

void CheckMonopolyStatus(int);		//IN:Player being evaluated
									//OUT:ActivePlayer MonopoliesOwned

void HouseBuildBalancer(int);		//IN:Property Player is wanting to build on (ActiveProperty)
									//OUT:Draw calls and updated house count / player funds / keep house count with +/- 1

char* IntToString(int,int);			//Convert Number data to String Data
char StringBuffer[20];				//cheap woke around for local buffer within function, produces garbage text without

void PropertyExchange(int);			//IN:AI Response, Offer is Global, but is feed into function
									//OUT: Update property and money values

int AI_Trade(void);					//IN:Offer Struct (Global var)	//AI trade evaluation, same for all AI players so far
									//OUT:Yes/No to trade offer

void BoardEvaluation(int,int);		//IN:Deal Maker, Deal Target Player ID
									//OUT:Fill in data for boardState Struct, used for trade evaluation

void MainBoardAction(int);			//IN:Local ButtonClicked value from root ActionHandler
									//OUT:Global vars about MainGameBoard

void BuildAction(int);				//IN:Local ButtonClicked value from root ActionHandler
									//OUT:Glboal vars about BuildBoard

void TradeAction(int);				//IN:Loval ButtonClicked value from root ActionHandler
									//OUT:Global vars about TradeBoard

void MortgageAction(int);			//IN:Local ButtonClicked value from root ActionHandler
									//OUT:Global varsa about MortgageBoard

void StartUp_CC(void);				//IN:Global
									//OUTGlobal, setup, initize Chest/Chest card data and draw order

void ChanceFunc(void);				//IN:Global vars
									//OUT:Update global vars

void ChestFunc(void);				//IN:Global vars
									//OUT:Update global vars

int CardAction(int[4]);				//IN:Card parameters from chance/chest
									//OUT:Error bool, update global vars

void SetPlayerPosition(int);		//IN:Global vars, player pos / Jail / DB State
									//OUT:SFML Draw calls



void AI_MakeTrade(void);			//AI will create trade offers, build, future proof to add in mortgage property

void AI_MakeBuild(void);

void AI_MakeMortgage(void);



//Offset Data for drawing Players on GameBoard
//int POSXOffset[4]={10,10,35,35};	//10,10,35,35	normal(0-3) , corner(4-7)
//int POSYOffset[4]={55,80,55,80};	//10,35,10,35

//int POSXOffsetCorner[16]={75,75,50,25,05,05,32,56,06,06,32,56,75,75,50,25};	//Corner offsets, 0-3,4-7,8-11,12-15
//int POSYOffsetCorner[16]={25,50,75,75,25,50,75,75,56,31,06,06,56,31,05,05};

int MovementX[12]={10,35,10,35,60,80,30,80,5,35,5,35};
int MovementY[12]={25,25,55,55,80,60,80,30,5,5,35,35};

struct BoardTile
{
	char TileName[30];
	int ID;
	int BlockID;
	int Type;		//0-Normal,1-Util,2-RR,3-taxes,4-chest,5-chance,6-go,7-jail/visting,8-parking,9-goto jail
	int Owner;		//-2 -1 0 1 2 3 		-2:can not be owned, -1 not owned 0-3 player owner
	int BuyCost;
	int HouseCost;
	int HouseCount;
	int Rent[6];
	int MortgageValue;
	int isMortgage;

	sf::Font TileNameFont;
	sf::Text TileNameText;	//top
	sf::Text TileNameTextMid;
	sf::Text TileNameTextBot;

	sf::Font TileCostFont;
	sf::Text TileCostText;
	sf::Text TileCostTextBackUp;

	sf::RectangleShape TileOutline;
	sf::RectangleShape TileBlockColor;
	sf::RectangleShape TileImage;
	sf::Texture TileImageTexture;

	sf::RectangleShape TileHouse[4];
	sf::RectangleShape TileHotel;
	sf::RectangleShape TileOwnerColor;

	int RGB[3];
	int hasColor;

	int PosX;
	int PosY;
};

struct Agents
{
	int ID;
	int Pos;
	int Money;
	int DB_roll;
	int DB_count;
	int inJail;
	int isBankrupt;
	int hasRolled;
	int hasAI;
	float MonopoliesOwned[8]={0};	//dirty trick to track Completed Monopolies, sub 1 to get to LookUpTable array index

	int GetOutJail[2];	//0,1,2 since both chance/chest have one, may have more since chest/chance can be modify

	int PosX;
	int PosY;

	int VelX;
	int VelY;

	int Rot;

	int BuildDesireBlock[8]={0};		//Property Block Build desire list
	int TradeDesireBlock[8]={0};		//Property BLock Trade desire list

	sf::RectangleShape AgentSprite;
	sf::Texture AgentTexture;
	sf::Font AgentFont;
	sf::Text AgentText;
};

struct Element
{
	char ElementString[20];
	sf::Font ElementFont;
	sf::Text ElementText;
	sf::RectangleShape ElementShape;
	sf::Texture ElementTexture;
	int PosX;
	int PosY;
	int Width;
	int Height;
	int isClicked;		//old handle for button click
	int isValid;		//help set/change element back color
	int isVisible;		//same as isActive, added to render queue
	int value;
};

struct TradeStruct
{
	int target_GetProp[30]={0};
	int target_GetFunds;
	int player_GetProp[30]={0};
	int player_GetFunds;
	int T_GP_Count;		//index # of props in array
	int P_GP_Count;
};

struct BoardState
{
	float MonopolyState[2][10];	//0-Player making deal	1-player deal is targeting
	int Funds[2];				//0-(HUMAN)	1-(AI)
};

//May not use for chest / chance, tie into original idea of using NoticeBoard
struct GameCard
{
	int Num;
	char Text[50];
	int Parm[4];		//Parm[0]	Card Action Type: 0move player, 1collect, 2pay, 3collect others, 4pay others, 5property upkeep, 6getout jail, 7goto jail
						//Parm[1 ... 3]		more details to take action

						//Parm 0 [] [] []		moved fixed number of title, move to property id, move to property type
						//Parm 5 [] []			house cost, hotel cost

	sf::Text CardText;
	sf::RectangleShape CardShape;
	sf::Font TextFont;
	sf::Texture CardTexture;
};

/* Frames (Menus):
 *
 * Main Game Board
 * Trading
 * Building
 * (un)Mortgage
 * get Trade Offer
 */

BoardTile Property[40];
Agents Player[4];

// Structs used for trade menu and trade evaluation
BoardTile PropertyDeed[40];
BoardTile FurtureProperty[40];
Agents FuturePlayer[4];
TradeStruct Offer;
BoardState Board[2];

int ChanceSeq[16];	//card sequence for drawing Chance / Chest
int ChestSeq[16];

int ChancePT=0;	//pointers for card stack
int ChestPT=0;

int Aceleration=1;
int MovementActive=0;

const int ButtonCount=110;	//Sets how many buttons, goes across many functions
const int TextBoxCount=20;	//TextBox count
Element Button[ButtonCount];	//BuToNS to click on

Element Line[2];		//Draw a line, used for trading menu
Element Dice[2];		//game dice
Element PlayerCount;	//displaces current ActivePlayer
Element Debugger[10];	//debug printout in game window for testing
Element TextBox[TextBoxCount];	//replace old debugger

Element Funds[4];		//displace player funds
Element Cards[2];		//chest / chance cards
Element NoticeBoard;	//use for main game board interactions
						//chance, chest, buying property, paying dues
						//can change colors/size based on situation used in

GameCard CommunityChest[16];
GameCard Chance[16];


/*	Game Board Buttons:		3rd rebuild
 *	0: End Turn
 *	1: Main Game Frame
 *	2: Save Game
 *	3: Build Frame
 *	4: Load Game
 *	5: Trade Frame
 *	6: Quite Game
 *	7: Mortgage Frame
 *	8: Roll Dice
 *	9: reserved	submit offer to AI, exit trade by deselect everything and go back to other menu with buttons above
 *	10: Target Player 1 ... 13: Target Player 4, used in trading menu
 *
 *	14: Accept		Universal across different states and frames
 *	15: Decline
 *
 *	16: '0'		these are now abandon
 *	17: '1'
 *	18:	'2'
 *	19:	'3'			7 8 9
 *	20:	'4'			4 5 6
 *	21:	'5'			1 2 3
 *	22	'6'			0 D C
 *	23	'7'
 *	24	'8
 *	25	'9'
 *	26	'CE'
 *	27	'DEL'
 *
 *	28	reserved	used for Active Player give cash offer
 *  29  reserved	used for Active Player get cash offer
 *
 *  30  add / sub house count to all properties, only draw those that matter
 *  ..
 *  110
 *
 *	for trading menu, will draw property tiles / trading cards
 *
 *===========================
 * TextBox Use
 * 0: ActivePlayer, MainBoard
 * 1: Player_GetFunds, Trade
 * 2: Target_GetFunds, Trade
 * 3: Target Current Funds, Trade
 * 4: Player Current Funds, Trade
 *
 *
 *
 */


//int Frame=0;
//int State=0;
//int PropertyHandlerState=0;
int ActiveWindow=0;		//active toggle for buy property notice
//int ButtonHandlerState=0;

int ActivePlayer=0;			//could be local, but for end turn values gets updated, easier as gobal
int TargetPlayer=5;			//Player that ActivePlayer is wanting to trade with
int CashOfferMode=0;			//0-not in cash offer, 1-player get, 2-player give
int ActiveFrame=0;			//Main Game Board, Build, Trade, Mortgage Menus
int NoticeBoardState=0;		//Notice Board version / isActive	(ActiveWindow replacement)
int PlayerActionState=0;	//State of player action, might not need global, but as backup
int PropertyHandlerState=0;	//Current Property landed on, could be made part of player struct
							//combine ButtonClickState with ActivePlayer to track player button selection
int MonopolyBlockLookUp[8][3]={0};	//Cheat, quick lookup for monopoly block properties max of 3 properties per monopoly block
int PropertyPerBlock[8]={0};		//Stop having to figure out how many properties are in block
int DiceRollCount=0;		//AI will cheat and know the entire RND number sequence, this is the pointer on that list
int GameLoad=0;

int main() {
	int i=0;
	int RNDSeed=0;
	StartUpPropertyName();
	StartUpPropertyData();
	StartUpPropertySpec();
	StartUpPropertyDeed();
	StartUpGameBoard();
	StartUpPlayer();
	StartUp_CC();
	DrawGameBoard();
		//	srand(time(NULL));

	FILE *fp;
	fp=fopen("bits/ini_values/Seed.txt","r");
	fscanf(fp,"%d",&RNDSeed);
	if(RNDSeed==NULL)
	{
		printf("File Empty\n");
		RNDSeed=time(NULL);
	}
	else
		srand(RNDSeed);
	printf("Seed: %d\n",RNDSeed);
	fclose(fp);

	//window.setVerticalSyncEnabled(true);

	//windowProperty.close();
	//windowProperty.setVisible(false);
	int RNDTick=0;
	while(window.isOpen())
	{
		TextBox[0].ElementText.setString(IntToString(ActivePlayer+1,3));	//was setting 2, now 3
		sf::Event event;
		if(Player[ActivePlayer].hasAI==0)
		{
			//Human Player, the joke the player has no AI
			//Handles Human Interaction
			while(window.pollEvent(event))
			{
				switch(event.type)
				{
				case Event::Closed:
					window.close();
					break;

				case Event::MouseButtonPressed:
					PlayerActionHandler(ButtonHandler());
					printf("AP:%d AF:%d NBS:%d DB:%d Jail:%d\n",ActivePlayer,ActiveFrame,NoticeBoardState,Player[ActivePlayer].DB_count,Player[ActivePlayer].inJail);

					break;

				case Event::KeyPressed:
					//printf("key code is: %d\n",event.key.code);
					//used for making a cash offer in trade, more fun then typeing keyboard on screen, easier to make, maybe
					if(event.key.code==58||event.key.code==36)	//ENTER or ESC to exit number entery
					{
						CashOfferMode=0;
					}

					if(event.key.code==Keyboard::Q)
					{
						for(i=0;i<40;i++)
						{
							Property[i].TileOutline.rotate(10);
						}
					}

					if(((event.key.code>74)&&(event.key.code<85))||((event.key.code>25)&&(event.key.code<36)))
						if(event.key.code>74)
							i=event.key.code-75;
						else
							i=event.key.code-26;
					switch(CashOfferMode)
					{
					case 1:
						//Active Player get cash
						if(event.key.code==59)
						{
							Offer.player_GetFunds/=10;
						}
						else
						{
							Offer.player_GetFunds*=10;
							Offer.player_GetFunds+=i;
							if(Offer.player_GetFunds>Player[TargetPlayer].Money)
							{
								Offer.player_GetFunds/=10;
							}
						}
						break;
					case 2:
						//Active Player give cash
						if(event.key.code==59)
						{
							Offer.target_GetFunds/=10;
						}
						else
						{
							Offer.target_GetFunds*=10;
							Offer.target_GetFunds+=i;
							if(Offer.target_GetFunds>Player[ActivePlayer].Money)
							{
								Offer.target_GetFunds/=10;
							}
						}
						break;
					default:
						break;
					}


					break;

				case Event::KeyReleased:
					break;

				default:
					break;
				}
			}
		//END of Human Interaction Handling
		}
		else
		{
			//AI Player, simulate human button presses
			//RNDTick=rand()%10;
			window.pollEvent(event);
			if(RNDTick==0)
			{
				//ButtonClickedState=-1;
				PlayerActionHandler(AI_TURN());
				//printf("Player %d is Active, Landed on Property %d, Clicked Button %d\n",ActivePlayer,Player[ActivePlayer].Pos,ButtonClickedState);
				//printf("NoticeState %d ; PropHandlerState %d ; ClickState %d ; PlayerActionState %d\n",NoticeBoardState,PropertyHandlerState,ButtonClickedState,PlayerActionState);
			}

			/*
			if(event.type==sf::Event::MouseButtonPressed)
			{
				Player[ActivePlayer].hasAI=0;
			}
			*/
			//system("PAUSE");
		}

		//render game board / everything

		switch(ActiveFrame)
		{
		case 0:
			//Game Board (Main)
			for(i=0;i<9;i++)
			{
				if(i%2==0)
					Button[i].isVisible=1;
				if(Player[ActivePlayer].hasRolled==1)
				{
					Button[8].isVisible=0;
					Button[0].isVisible=1;
				}
				else
				{
					Button[8].isVisible=1;
					Button[0].isVisible=0;
				}
			}
			DrawGameBoard();
			break;
		case 1:
			//Build
			for(i=0;i<9;i++)
			{
				if(i%2==0)
					Button[i].isVisible=0;
			}
			DrawBuildBoard();
			break;
		case 2:
			//Trade
			for(i=0;i<9;i++)
			{
				if(i%2==0)
					Button[i].isVisible=0;
			}
			DrawTradeBoard();
			break;
		case 3:
			//Mortgage
			break;
		case 4:
			//Receive Trade Offer
			break;
		default:
			break;
		}



	}

	return 0;
}

void StartUpPropertyName(void)
{
	//read in 40 lines of property names from text file, max of 29 charactors for property name

	char Buffer[600]={0};
	int i=0;
	int k=0;
	int j=0;
	int l=0;
	int m=0;
	int n=0;
	int BufferLen[40]={0};
	int BufferOffSet=0;
	FILE *fp;

	//open file to get names, read in charactors to long buffer unit end of file
	fp=fopen("bits/ini_values/Names.txt","r");

	for(i=0;i<600;i++)
	{
		if(feof(fp))
		{
			break;
		}
		fscanf(fp,"%c",&Buffer[i]);
		j++;
	}
	fclose(fp);
	j--;//sub 1 to drop EOF

	//get string lengths for each property
	i=0;
	m=0;
	while(m<40)
	{
		if(Buffer[k]=='\n')
		{
			BufferLen[m]=l;
			m++;
			l=0;
		}
		else
		{
			l++;
		}
		k++;
	}

	//copy length of property tile from buffer to property name in property struct
	for(i=0;i<40;i++)
	{
		char Buffer2[BufferLen[i]+1];
		for(n=0;n<(BufferLen[i]);n++)
		{
			Buffer2[n]=Buffer[n+BufferOffSet];
			if(Buffer2[n]==' ')
				Buffer2[n]='\n';
		}
		Buffer2[BufferLen[i]]='\0';
		strcpy(Property[i].TileName,Buffer2);
		BufferOffSet+=(BufferLen[i]+1);
	}

}

void StartUpPropertyData(void)
{
	FILE *fp;
	int i=0;
	int j=0;
	char buffer[10] ;
	fp=fopen("bits/ini_values/BlockID.txt","r");
	for(i=0;i<40;i++)
	{
		fscanf(fp,"%d",&Property[i].BlockID);
		Property[i].ID=i;
		Property[i].HouseCount=0;
		Property[i].isMortgage=0;
		Property[i].Owner=-1;
	}
	fclose(fp);

	fp=fopen("bits/ini_values/HouseCost.txt","r");
	for(i=0;i<40;i++)
	{
		fscanf(fp,"%d",&Property[i].HouseCost);
	}
	fclose(fp);

	fp=fopen("bits/ini_values/PropPrice.txt","r");
	for(i=0;i<40;i++)
	{
		fscanf(fp,"%d",&Property[i].BuyCost);
		Property[i].MortgageValue=Property[i].BuyCost/2;
	}
	fclose(fp);

	fopen("bits/ini_values/PropPrice.txt","r");
	for(i=0;i<40;i++)
	{
		fscanf(fp,"%s",&buffer[0]);
		Property[i].TileCostText.setString(buffer);
		Property[i].TileCostTextBackUp.setString(Property[i].TileCostText.getString());
	}
	fclose(fp);

	fp=fopen("bits/ini_values/PropType.txt","r");
	for(i=0;i<40;i++)
	{
		fscanf(fp,"%d",&Property[i].Type);
	}
	fclose(fp);

	fp=fopen("bits/ini_values/RentValue.txt","r");
	for(i=0;i<40;i++)
	{
		for(j=0;j<6;j++)
		{
			fscanf(fp,"%d",&Property[i].Rent[j]);
		}
	}
	fclose(fp);

	system("cls");

	fp=fopen("bits/ini_values/PropColor.txt","r");
	for(i=0;i<40;i++)
	{
		int PropColorBuff[4]={0};
		for(j=0;j<4;j++)
		{
			fscanf(fp,"%d",&PropColorBuff[j]);
		}

		Property[i].hasColor=PropColorBuff[0];
		Property[i].RGB[0]=PropColorBuff[1];
		Property[i].RGB[1]=PropColorBuff[2];
		Property[i].RGB[2]=PropColorBuff[3];

		//printf("%d\t%d\t%d\t%d\n",PropColorBuff[0],PropColorBuff[1],PropColorBuff[2],PropColorBuff[3]);

	}//	rectangle[i].setFillColor(sf::Color(204,227,199));

	Courior.loadFromFile("bits/fonts/cour.ttf");
	Calibri.loadFromFile("bits/fonts/calibri.ttf");

	//Setup for Monopoly Block Look Up, look at Propert Type & Block Color



	int PropertyStart=0;	//first build-able property, sets indexes
	while(Property[PropertyStart].hasColor==0)
	{
		PropertyStart++;
	}

	int MonopolyIndexX=0;	//	[10][4]	lookUp Array
	int MonopolyIndexY=0;	//quick indexes for filling in Monopoly Look Up Table, set table to 0,0	(Block#, Property#)

	int ColorCurrent[3]={Property[PropertyStart].RGB[0],Property[PropertyStart].RGB[1],Property[PropertyStart].RGB[2]};	//Current property colors
	int BlockChange=0;

	for(i=0;i<40;i++)
	{
		if(Property[i].Type==0)
		{
			//Normal will have block color data
			for(j=0;j<3;j++)
			{
				if(Property[i].RGB[j]!=ColorCurrent[j])
				{
					//different RGB Value from current
					BlockChange=1;
				}
			}
			if(BlockChange==0)
			{
				//no change keep adding to current block
				MonopolyBlockLookUp[MonopolyIndexY][MonopolyIndexX]=i;
				PropertyPerBlock[MonopolyIndexY]=MonopolyIndexX;
				MonopolyIndexX++;
			}
			else
			{
				//block change, first check if in a previous block, if not inc indexY and add to list as new
				//framework is in to allow for property blocks nested with in other blocks, but not used
				//blocks MUST be sequential or things will break
				MonopolyIndexX=0;
				MonopolyIndexY++;
				MonopolyBlockLookUp[MonopolyIndexY][MonopolyIndexX]=i;
				BlockChange=0;
				for(j=0;j<3;j++)
				{
					ColorCurrent[j]=Property[i].RGB[j];
				}
				MonopolyIndexX++;
			}
		}
	}

	for(i=0;i<8;i++)
	{
		for(j=0;j<3;j++)
		{
			printf("%d ",MonopolyBlockLookUp[i][j]);
		}
		printf("\n");
	}

	for(i=0;i<8;i++)
	{
		printf("Property in Block %d is %d\n",i+1,PropertyPerBlock[i]+1);
	}

	//system("PAUSE");


}	//Property[i].TileBlockSetColor(sf::Color(23,42,23))

void StartUpPropertySpec(void)
{
	int i=0;
	int j=0;

	int PropX=65;	//normal 210x210 & 210x130	total 1600x1600 px		65
	int PropY=105;	//scale down 1/2 to 800x800 px						105
	int PropJail=71;//set size for jail area

	for(i=0;i<40;i++)
	{
		Property[i].TileOutline.setSize(sf::Vector2f(PropX,PropY));
		Property[i].TileBlockColor.setSize(sf::Vector2f(PropX,PropY/5));
	}
	Property[0].TileOutline.setSize(sf::Vector2f(PropY,PropY));
	Property[10].TileOutline.setSize(sf::Vector2f(PropY,PropY));
	Property[20].TileOutline.setSize(sf::Vector2f(PropY,PropY));
	Property[30].TileOutline.setSize(sf::Vector2f(PropY,PropY));

	for(i=0;i<40;i++)
	{
		Property[i].TileOutline.setFillColor(sf::Color(204,227,199));
		Property[i].TileOutline.setOutlineThickness(2);
		Property[i].TileOutline.setOutlineColor(sf::Color::Black);
		Property[i].TileBlockColor.setFillColor(sf::Color(Property[i].RGB[0],Property[i].RGB[1],Property[i].RGB[2]));
		Property[i].TileBlockColor.setOutlineThickness(2);
		Property[i].TileBlockColor.setOutlineColor(sf::Color::Black);

		Property[i].TileNameText.setFont(Calibri);
		Property[i].TileNameTextMid.setFont(Calibri);
		Property[i].TileNameTextBot.setFont(Calibri);
		Property[i].TileCostText.setFont(Calibri);

		Property[i].TileNameText.setCharacterSize(10);
		Property[i].TileNameTextMid.setCharacterSize(10);
		Property[i].TileNameTextBot.setCharacterSize(10);

		Property[i].TileNameText.setColor(sf::Color::Black);
		Property[i].TileNameTextMid.setColor(sf::Color::Black);
		Property[i].TileNameTextBot.setColor(sf::Color::Black);
		Property[i].TileNameText.setString(Property[i].TileName);
		//break apart TileName into multiply Text Blocks

		for(j=0;j<4;j++)
		{
			Property[i].TileHouse[j].setSize(sf::Vector2f(15,15));
			Property[i].TileHouse[j].setFillColor(sf::Color::Green);
			Property[i].TileHouse[j].setOutlineThickness(1);
			Property[i].TileHouse[j].setOutlineColor(sf::Color::Black);
		}

		Property[i].TileHotel.setSize(sf::Vector2f(31,15));
		Property[i].TileHotel.setFillColor(sf::Color::Red);
		Property[i].TileHouse[j].setOutlineThickness(1);
		Property[i].TileHouse[j].setOutlineColor(sf::Color::Black);

	}

	//TileNameToTextBox();

	//break apart the TileName to at most 3 seperate text blocks, break at each space


	Property[10].TileBlockColor.setSize(sf::Vector2f(PropJail,PropJail));
	Property[10].TileBlockColor.setPosition(Property[10].TileOutline.getPosition().x,
			Property[10].TileOutline.getPosition().y);

	//PropX+=Property[1].TileOutline.getOutlineThickness();
	//PropY+=Property[1].TileOutline.getOutlineThickness();

	PropX+=2;
	PropY+=2;

	for(i=0;i<10;i++)
	{
		Property[i].TileOutline.setPosition(PropY+(9*PropX)-((PropX)*(i))+2,(9*PropX)+PropY+2);
		Property[i].TileBlockColor.setPosition(PropY+(9*PropX)-(PropX*(i))+2,(9*PropX)+PropY+2);
		if(Property[i].hasColor==1)
		{
			Property[i].TileNameText.setPosition(Property[i].TileBlockColor.getPosition().x,
				Property[i].TileBlockColor.getPosition().y+Property[i].TileBlockColor.getSize().y);
		}
		else
		{
			Property[i].TileNameText.setPosition(Property[i].TileBlockColor.getPosition().x,
				Property[i].TileBlockColor.getPosition().y);
		}

		for(j=0;j<4;j++)
		{
			Property[i].TileHouse[j].setPosition(Property[i].TileBlockColor.getPosition().x+1+(j*16),
				Property[i].TileBlockColor.getPosition().y+2);

		}
		Property[i].TileHotel.setPosition(Property[i].TileBlockColor.getPosition().x+16,
				Property[i].TileBlockColor.getPosition().y+2);

		if(Property[i].Type<3)
		{

		}

	}
	for(i=10;i<20;i++)
	{
		Property[i].TileOutline.setPosition(PropY,(PropY+((9*PropX))-(PropX*(i-10)))+2);
		Property[i].TileBlockColor.setPosition(PropY,(PropY+((9*PropX))-(PropX*(i-10)))+2);
		Property[i].TileOutline.setRotation(90);
		Property[i].TileBlockColor.setRotation(90);
		Property[i].TileNameText.setRotation(90);
		if(Property[i].hasColor==1)
		{
			Property[i].TileNameText.setPosition(Property[i].TileBlockColor.getPosition().x-
					Property[i].TileBlockColor.getSize().y,Property[i].TileBlockColor.getPosition().y);
		}
		else
		{
			Property[i].TileNameText.setPosition(Property[i].TileBlockColor.getPosition().x,
					Property[i].TileBlockColor.getPosition().y);
		}

		for(j=0;j<4;j++)
		{
			Property[i].TileHouse[j].setRotation(90);
			Property[i].TileHouse[j].setPosition(Property[i].TileBlockColor.getPosition().x-2,
				Property[i].TileBlockColor.getPosition().y+1+(j*16));

		}
		Property[i].TileHotel.setRotation(90);
		Property[i].TileHotel.setPosition(Property[i].TileBlockColor.getPosition().x-2,
				Property[i].TileBlockColor.getPosition().y+16);
	}
	for(i=20;i<30;i++)
	{
		Property[i].TileOutline.setPosition(PropY+(PropX*(i-20)),PropY);
		Property[i].TileBlockColor.setPosition(PropY+(PropX*(i-20)),PropY);
		Property[i].TileOutline.setRotation(180);
		Property[i].TileBlockColor.setRotation(180);
		Property[i].TileNameText.setRotation(180);
		if(Property[i].hasColor==1)
		{
			Property[i].TileNameText.setPosition(Property[i].TileBlockColor.getPosition().x,
				Property[i].TileBlockColor.getPosition().y-Property[i].TileBlockColor.getSize().y);
		}
		else
		{
			Property[i].TileNameText.setPosition(Property[i].TileBlockColor.getPosition().x,
				Property[i].TileBlockColor.getPosition().y);
		}

		for(j=0;j<4;j++)
		{
			Property[i].TileHouse[j].setRotation(180);
			Property[i].TileHouse[j].setPosition(Property[i].TileBlockColor.getPosition().x-1-(j*16),
				Property[i].TileBlockColor.getPosition().y-2);

		}
		Property[i].TileHouse[j].setRotation(180);
		Property[i].TileHotel.setPosition(Property[i].TileBlockColor.getPosition().x-16,
				Property[i].TileBlockColor.getPosition().y-2);
	}
	for(i=30;i<40;i++)
	{
		Property[i].TileOutline.setPosition((9*PropX)+PropY+2,PropY+(PropX*(i-30)));
		Property[i].TileBlockColor.setPosition((9*PropX)+PropY+2,PropY+(PropX*(i-30)));
		Property[i].TileOutline.setRotation(270);
		Property[i].TileBlockColor.setRotation(270);
		Property[i].TileNameText.setRotation(270);
		if(Property[i].hasColor==1)
		{
			Property[i].TileNameText.setPosition(Property[i].TileBlockColor.getPosition().x+
				Property[i].TileBlockColor.getSize().y,Property[i].TileBlockColor.getPosition().y);
		}
		else
		{
			Property[i].TileNameText.setPosition(Property[i].TileBlockColor.getPosition().x,
				Property[i].TileBlockColor.getPosition().y);
		}

		for(j=0;j<4;j++)
		{
			Property[i].TileHouse[j].setRotation(270);
			Property[i].TileHouse[j].setPosition(Property[i].TileBlockColor.getPosition().x+2,
				Property[i].TileBlockColor.getPosition().y-1-(j*16));

		}
		Property[i].TileHotel.setRotation(270);
		Property[i].TileHotel.setPosition(Property[i].TileBlockColor.getPosition().x+2,
				Property[i].TileBlockColor.getPosition().y-16);
	}





}

void StartUpGameBoard(void)
{
	//setup player funds elements
	//setup chance/community chest tiles
	//setup option buttons
	//setup dice/roll dice

	int i=0;
	int j=0;

	for(i=0;i<4;i++)
	{
		//SetUp Funds drawing detail
		Funds[i].ElementShape.setSize(sf::Vector2f(80,40));
		Funds[i].ElementShape.setPosition(835+90*i,20);
		Funds[i].ElementShape.setFillColor(sf::Color::Green);
		Funds[i].ElementShape.setOutlineColor(sf::Color::Black);
		Funds[i].ElementShape.setOutlineThickness(2);
		Funds[i].ElementText.setPosition(Funds[i].ElementShape.getPosition().x,
				Funds[i].ElementShape.getPosition().y);
		Funds[i].ElementText.setCharacterSize(20);
		Funds[i].ElementText.setColor(sf::Color::Black);
		Funds[i].ElementText.setFont(Calibri);
	}

	for(i=0;i<2;i++)
	{
		//Setup Chance / Community Chest
		if(i%2==0)
		{
			Cards[i].ElementShape.setFillColor(sf::Color::Cyan);
			Cards[i].ElementShape.setPosition(308,222);
			Cards[i].ElementShape.setRotation(135);
		}
		else
		{
			Cards[i].ElementShape.setFillColor(sf::Color(255,106,0));
			Cards[i].ElementShape.setPosition(511,598);
			Cards[i].ElementShape.setRotation(315);
		}
		Cards[i].ElementShape.setSize(sf::Vector2f(120,80));
		Cards[i].ElementShape.setOutlineColor(sf::Color::Black);
		Cards[i].ElementShape.setOutlineThickness(2);
	}

	Button[0].ElementText.setString("End Turn");	//button 0
	Button[1].ElementText.setString("Main");		//button 1
	Button[2].ElementText.setString("Save");		//button 2
	Button[3].ElementText.setString("Build");		//button 3
	Button[4].ElementText.setString("Load");		//button 4
	Button[5].ElementText.setString("Trade");		//button 5
	Button[6].ElementText.setString("Quit");		//button 6
	Button[7].ElementText.setString("Mortgage");	//button 7
	Button[14].ElementText.setString("Accept");		//button 14
	Button[15].ElementText.setString("Decline");	//button 15

	int odd=0;
	int even=0;
	for(i=0;i<9;i++)
	{
		//Setup Menu Buttons
		if(i==8)
		{
			//Button 8, Roll Dice SetUp
			Button[i].ElementShape.setPosition(972,400);
			Button[i].ElementText.setString("Roll Dice");

		}
		else
		{
			if(i%2==0)
			{
				//odd
				Button[i].ElementShape.setPosition(835+90*odd,100+600);
				odd++;
			}
			else
			{
				//even
				Button[i].ElementShape.setPosition(835+90*even,150+600);
				even++;
			}
		}

		Button[i].ElementText.setPosition(
			Button[i].ElementShape.getPosition().x,
			Button[i].ElementShape.getPosition().y);
		Button[i].ElementText.setColor(sf::Color::Black);
		Button[i].ElementText.setCharacterSize(18);
		Button[i].ElementText.setFont(Calibri);
		Button[i].ElementShape.setFillColor(sf::Color(160,189,216));
		Button[i].ElementShape.setSize(sf::Vector2f(80,40));
		Button[i].ElementShape.setOutlineColor(sf::Color::Black);
		Button[i].ElementShape.setOutlineThickness(2);
		Button[i].isVisible=1;
	}

	//setup for dice/roll dice button
	//dice 1, dice 2, roll dice button (button 9)
	for(i=0;i<2;i++)
	{
		//draw dice
		Dice[i].ElementShape.setSize(sf::Vector2f(80,80));
		Dice[i].ElementShape.setOutlineColor(sf::Color::Black);
		Dice[i].ElementShape.setOutlineThickness(2);
		Dice[i].ElementShape.setPosition(925+100*i,300);	//890,290
		Dice[i].ElementShape.setFillColor(sf::Color::White);
	}

	//setup NoticeBoard - Tells player about buying/pay rent for property than landed on
	NoticeBoard.ElementShape.setSize(sf::Vector2f(200,200));
	NoticeBoard.ElementShape.setOutlineColor(sf::Color::Black);
	NoticeBoard.ElementShape.setOutlineThickness(2);
	NoticeBoard.ElementShape.setFillColor(sf::Color::White);
	NoticeBoard.ElementShape.setPosition(200,200);
	NoticeBoard.ElementText.setFont(Calibri);
	NoticeBoard.ElementText.setCharacterSize(20);
	NoticeBoard.ElementText.setColor(sf::Color::Black);


	//setup button 14,15 as accept and decline universal buttons

	for(i=14;i<16;i++)
	{
		Button[i].ElementShape.setSize(sf::Vector2f(100,50));
		Button[i].ElementShape.setOutlineColor(sf::Color::Black);
		Button[i].ElementShape.setOutlineThickness(2);
		if(i==14)
			Button[i].ElementShape.setFillColor(sf::Color::Green);
		else
			Button[i].ElementShape.setFillColor(sf::Color::Red);
		Button[i].ElementShape.setPosition(0,0);
		Button[i].ElementText.setColor(sf::Color::Black);
		Button[i].ElementText.setCharacterSize(18);
		Button[i].ElementText.setFont(Calibri);
		Button[i].isVisible=0;
	}

	//Setup for NoticeBoard interface
	Button[14].ElementShape.setPosition(200,350);	//Green
	Button[15].ElementShape.setPosition(300,350);	//Red
	Button[14].ElementText.setPosition(Button[14].ElementShape.getPosition().x,Button[14].ElementShape.getPosition().y);
	Button[15].ElementText.setPosition(Button[15].ElementShape.getPosition().x,Button[15].ElementShape.getPosition().y);
	NoticeBoard.ElementText.setPosition(NoticeBoard.ElementShape.getPosition().x,NoticeBoard.ElementShape.getPosition().y);

	//setup for TextBox Elements (active player, AP funds, AP get cash, TP funds, TP get cash)
	for(i=0;i<TextBoxCount;i++)
	{
		TextBox[i].ElementText.setPosition(1000,100);
		TextBox[i].ElementText.setCharacterSize(20);
		TextBox[i].ElementText.setColor(sf::Color::Black);
		TextBox[i].ElementText.setFont(Calibri);
	}
	//Might move to new function StartUpBuildBoard---------------------------------------------------------------



	//setup for building menu buttons
	for(i=30;i<ButtonCount;i++)
	{
		Button[i].ElementShape.setSize(sf::Vector2f(25,25));
		Button[i].ElementShape.setOutlineColor(sf::Color::Black);
		Button[i].ElementShape.setOutlineThickness(2);
		Button[i].ElementShape.setFillColor(sf::Color::White);

		if(i%2==0)
		{
			//Button[i].ElementShape.setPosition();
			Button[i].ElementTexture.loadFromFile("bits/sprits/add.png");
		}
		else
		{
			Button[i].ElementTexture.loadFromFile("bits/sprits/sub.png");
		}

		Button[i].ElementShape.setTexture(&Button[i].ElementTexture);
		Button[i].isVisible=0;
	}

	for(i=0;i<40;i++)
	{
		int PropPosX=Property[i].TileOutline.getPosition().x;
		int PropPosY=Property[i].TileOutline.getPosition().y;
		int PropRotate=Property[i].TileOutline.getRotation();
		switch(PropRotate)
		{
		case 0:
			Button[(i*2)+30].ElementShape.setPosition(PropPosX+40,PropPosY+80);
			Button[(i*2)+31].ElementShape.setPosition(PropPosX,PropPosY+80);
			break;
		case 90:
			Button[(i*2)+30].ElementShape.setPosition(PropPosX-105,PropPosY+40);
			Button[(i*2)+31].ElementShape.setPosition(PropPosX-105,PropPosY);
			break;
		case 180:
			Button[(i*2)+30].ElementShape.setPosition(PropPosX-65,PropPosY-105);
			Button[(i*2)+31].ElementShape.setPosition(PropPosX-25,PropPosY-105);
			break;
		case 270:
			Button[(i*2)+30].ElementShape.setPosition(PropPosX+80,PropPosY-65);
			Button[(i*2)+31].ElementShape.setPosition(PropPosX+80,PropPosY-25);
			break;
		default:
			printf("Something wrong in housebuilding buttons\n");
			break;
		}

	}

	//Setup TragetPlayer buttons for trading menu, might trade menu setup function later
	for(i=0;i<4;i++)
	{
		//Activeplyer button will move to left of screen but set offset, will be visable, but not clickable

		Button[10+i].ElementShape.setPosition(835+90*i,20);	//same spot at debug
		Button[10+i].PosX=Button[10+i].ElementShape.getPosition().x;
		Button[10+i].ElementShape.setSize(sf::Vector2f(80,40));
		Button[10+i].ElementShape.setOutlineColor(sf::Color::Black);
		Button[10+i].isVisible=0;
		Button[10+i].ElementShape.setOutlineThickness(2);
		Button[10+i].ElementShape.setFillColor(sf::Color::White);
		Button[10+i].ElementText.setString(IntToString(i+1,2));
		Button[10+i].ElementText.setPosition(Button[10+i].ElementShape.getPosition().x,Button[10+i].ElementShape.getPosition().y);
		Button[10+i].ElementText.setCharacterSize(20);
		Button[10+i].ElementText.setColor(sf::Color::Black);
		Button[10+i].ElementText.setFont(Calibri);

	}

	//Draw 2 lines for trading menu design
	for(i=0;i<2;i++)
	{
		Line[i].ElementShape.setPosition(0,60);
		Line[i].ElementShape.setSize(sf::Vector2f(1400,5));
		Line[i].ElementShape.setOutlineColor(sf::Color::Black);
		Line[i].ElementShape.setOutlineThickness(2);
		Line[i].ElementShape.setFillColor(sf::Color::Black);
	}
	Line[1].ElementShape.setPosition(600,0);
	Line[1].ElementShape.setRotation(90);

	//Setup Cash Offers and Submit Offer Buttons
	for(i=27;i<30;i++)
	{
		j=i;
		if((i-18)==9)
		{
			//27 -> 9 setup for button 9,28,29
			j=9;
		}
		Button[j].ElementShape.setPosition(20+(110*(i-27)),750);
		Button[j].PosX=Button[j].ElementShape.getPosition().x;
		Button[j].ElementShape.setSize(sf::Vector2f(100,50));
		Button[j].ElementShape.setOutlineColor(sf::Color::Black);
		Button[j].isVisible=1;
		Button[j].ElementShape.setOutlineThickness(2);
		Button[j].ElementShape.setFillColor(sf::Color::White);
		Button[j].ElementText.setPosition(Button[j].ElementShape.getPosition().x,Button[j].ElementShape.getPosition().y);
		Button[j].ElementText.setCharacterSize(20);
		Button[j].ElementText.setColor(sf::Color::Black);
		Button[j].ElementText.setFont(Calibri);
	}
	Button[9].ElementText.setString("Submit\nOffer");
	Button[28].ElementText.setString("Give\nCash Offer");
	Button[29].ElementText.setString("Get\nCash Offer");

	Button[0].isVisible=0;
	Button[8].isVisible=1;

}

void StartUpPropertyDeed(void)
{
	//lot of copy/paste from property struct to deeds, but this is done for position protection
	int i=0;
	int j=0;

	int PropX=65;
	int PropY=105;

	for(i=0;i<40;i++)
	{
		strcpy(PropertyDeed[i].TileName,Property[i].TileName);
		PropertyDeed[i].hasColor=Property[i].hasColor;
		for(j=0;j<3;j++)
		{
			PropertyDeed[i].RGB[j]=Property[i].RGB[j];
		}

		PropertyDeed[i].TileOutline.setFillColor(sf::Color::White);
		PropertyDeed[i].TileOutline.setOutlineColor(sf::Color::Black);
		PropertyDeed[i].TileOutline.setOutlineThickness(2);
		PropertyDeed[i].TileOutline.setSize(sf::Vector2f(PropX,PropY));
		PropertyDeed[i].TileBlockColor.setSize(sf::Vector2f(PropX,PropY/5));
		PropertyDeed[i].TileBlockColor.setFillColor(sf::Color(Property[i].RGB[0],Property[i].RGB[1],Property[i].RGB[2]));

		PropertyDeed[i].TileNameText.setFont(Calibri);
		PropertyDeed[i].TileNameText.setCharacterSize(10);
		PropertyDeed[i].TileNameText.setColor(sf::Color::Black);
		PropertyDeed[i].TileNameText.setString(PropertyDeed[i].TileName);
	}

	int OffsetY=110;
	int OffsetX=70;

	//set base position for porperty deeds
	for(i=0;i<4;i++)
	{
		for(j=0;j<3;j++)
		{
			PropertyDeed[MonopolyBlockLookUp[i][j]].TileOutline.setPosition(j*70+OffsetX,i*110+OffsetY);			//( x , y) >>   \/
			PropertyDeed[MonopolyBlockLookUp[i][j]].TileBlockColor.setPosition(j*70+OffsetX,i*110+OffsetY);
		}
	}

	for(i=0;i<4;i++)
	{
		for(j=0;j<3;j++)
		{
			PropertyDeed[MonopolyBlockLookUp[i+4][j]].TileOutline.setPosition(j*70+210+OffsetX,i*110+OffsetY);
			PropertyDeed[MonopolyBlockLookUp[i+4][j]].TileBlockColor.setPosition(j*70+210+OffsetX,i*110+OffsetY);
		}
	}

	int RR_Count=0;
	int Util_Count=0;

	for(i=0;i<40;i++)
	{
		//Railroad & Utility
		if(Property[i].Type==2)
		{
			//Prop is Railroad
			PropertyDeed[i].TileOutline.setPosition(70*RR_Count+OffsetX,440+OffsetY);
			PropertyDeed[i].TileBlockColor.setPosition(70*RR_Count+OffsetX,440+OffsetY);
			RR_Count++;
		}
		if(Property[i].Type==1)
		{
			//Prop is Utility
			PropertyDeed[i].TileOutline.setPosition(70*Util_Count+280+OffsetX,440+OffsetY);
			PropertyDeed[i].TileBlockColor.setPosition(70*Util_Count+280+OffsetX,440+OffsetY);
			PropertyDeed[i].TileBlockColor.setFillColor(sf::Color(127,127,127));
			Util_Count++;
		}

	}

	for(i=0;i<40;i++)
	{
		PropertyDeed[i].TileNameText.setPosition(PropertyDeed[i].TileBlockColor.getPosition().x,
			PropertyDeed[i].TileBlockColor.getPosition().y+PropertyDeed[i].TileBlockColor.getSize().y);
	}

	Offer.P_GP_Count=0;		//zeros count before game start, sanitize index
	Offer.T_GP_Count=0;

	//Backup Deed positon interal to property struct
	for(i=0;i<40;i++)
	{
		PropertyDeed[i].PosX=PropertyDeed[i].TileOutline.getPosition().x;
		PropertyDeed[i].PosY=PropertyDeed[i].TileOutline.getPosition().y;
	}


}

void StartUp_CC(void)
{
	//Chance / Chest StartUp and Initiation
	FILE *fp;
	int i=0;
	int j=0;
	int k=0;
	int m=0;
	char Buffer[800];

	//Load in Data for Chance
	fp=fopen("bits/ini_values/Chance.txt","r");

	for(i=0;i<800;i++)
	{
		if(feof(fp))
		{
			break;
		}
		fscanf(fp,"%c",&Buffer[i]);
		j++;
	}
	fclose(fp);
	j--;//sub 1 to drop EOF

	//break down single string buffer to 16 buffers per card
	for(i=0;i<32;i++)
	{
		m=0;
		while(Buffer[k]!='\n')
		{
			if(i%2==0)
			{
				Chance[i/2].Text[m]=Buffer[k];
				m++;
			}
			k++;
		}
		if(i%2==0)
			Chance[i/2].Text[m]='\0';
		k++;
	}

	for(i=0;i<16;i++)
	{
		printf("%s\n",Chance[i].Text);
	}

	//Load in Data for Community Chest
	i=0;
	j=0;
	k=0;
	m=0;
	fp=fopen("bits/ini_values/Chest.txt","r");

	for(i=0;i<800;i++)
	{
		if(feof(fp))
		{
			break;
		}
		fscanf(fp,"%c",&Buffer[i]);
		j++;
	}
	fclose(fp);
	j--;//sub 1 to drop EOF

	//break down single string buffer to 16 buffers per card
	for(i=0;i<32;i++)
	{
		m=0;
		while(Buffer[k]!='\n')
		{
			if(i%2==0)
			{
				CommunityChest[i/2].Text[m]=Buffer[k];
				m++;
			}
			k++;
		}
		if(i%2==0)
			CommunityChest[i/2].Text[m]='\0';
		k++;
	}

	for(i=0;i<16;i++)
	{
		printf("%s\n",CommunityChest[i].Text);
	}

	//Load in Numeric data for chest and chance
	fp=fopen("bits/ini_values/ChanceData.txt","r");
	for(i=0;i<16;i++)
	{
		for(j=0;j<4;j++)
		{
			fscanf(fp,"%d",&Chance[i].Parm[j]);
		}
	}
	fclose(fp);

	fp=fopen("bits/ini_values/ChestData.txt","r");
	for(i=0;i<16;i++)
	{
		for(j=0;j<4;j++)
		{
			fscanf(fp,"%d",&CommunityChest[i].Parm[j]);
		}
	}
	fclose(fp);

	//Generate card draw order for chest / chance
	//will rnd later, can also be rand by change order in read-in file
	for(i=0;i<16;i++)
	{
		ChestSeq[i]=i;
		ChanceSeq[i]=i;
	}

	//Randomize Seq order
	int num1=0;
	int num2=0;
	int buf=0;

	for(i=0;i<200;i++)
	{
		num1=rand()%15;
		num2=rand()%15;
		buf=ChestSeq[num1];
		ChestSeq[num1]=ChestSeq[num2];
		ChestSeq[num2]=buf;

		num1=rand()%15;
		num2=rand()%15;
		buf=ChanceSeq[num1];
		ChanceSeq[num1]=ChanceSeq[num2];
		ChanceSeq[num2]=buf;
	}



		//system("PAUSE");
}

void DrawGameBoard(void)
{
	window.clear(sf::Color(92,163,99));
	int i=0;
	int j=0;
	int PropRotate;
	int CostTextX=0;
	int CostTextY=0;

	for(i=0;i<40;i++)
	{
		//draw Properties
		window.draw(Property[i].TileOutline);
		if(Property[i].hasColor==1)
			window.draw(Property[i].TileBlockColor);
		window.draw(Property[i].TileNameText);
	}

	for(i=0;i<40;i++)
	{
		//draw property price tag/owner
		if(Property[i].Type<3)
		{
			Property[i].TileCostText.setColor(sf::Color::Black);
			Property[i].TileCostText.setCharacterSize(10);
			PropRotate=Property[i].TileOutline.getRotation();
			if(Property[i].Owner!=-1)
			{
				//Property is owned, show owner
				Property[i].TileCostText.setString(IntToString(Property[i].Owner+1,2));
			}
			else
			{
				//Property is not owned, show price
				Property[i].TileCostText.setString(Property[i].TileCostTextBackUp.getString());
			}

			switch(PropRotate)
			{
			case 0:
				//Property[i].TileCostText.setString("$120");

				CostTextX=Property[i].TileOutline.getPosition().x+
					Property[i].TileOutline.getLocalBounds().width/2-
					Property[i].TileCostText.getLocalBounds().width/2;
				CostTextY=Property[i].TileOutline.getPosition().y+90;

				Property[i].TileCostText.setPosition(CostTextX,CostTextY);
				break;
			case 90:
				Property[i].TileCostText.setRotation(90);
				//Property[i].TileCostText.setString("$120");

				CostTextX=Property[i].TileOutline.getPosition().x-90;
				CostTextY=Property[i].TileOutline.getPosition().y+
					Property[i].TileOutline.getLocalBounds().width/2-
					Property[i].TileCostText.getLocalBounds().width/2;

				Property[i].TileCostText.setPosition(CostTextX,CostTextY);
				break;
			case 180:
				Property[i].TileCostText.setRotation(180);
				//Property[i].TileCostText.setString("$120");

				CostTextX=Property[i].TileOutline.getPosition().x-
					Property[i].TileOutline.getLocalBounds().width/2+
					Property[i].TileCostText.getLocalBounds().width/2+4;
				CostTextY=Property[i].TileOutline.getPosition().y-90;

				Property[i].TileCostText.setPosition(CostTextX,CostTextY);
				break;
			case 270:
				Property[i].TileCostText.setRotation(270);
				//Property[i].TileCostText.setString("$120");

				CostTextX=Property[i].TileOutline.getPosition().x+90;
				CostTextY=Property[i].TileOutline.getPosition().y-
					Property[i].TileOutline.getLocalBounds().width/2+
					Property[i].TileCostText.getLocalBounds().width/2+4;

				Property[i].TileCostText.setPosition(CostTextX,CostTextY);
				break;
			default:

				break;
			}

			window.draw(Property[i].TileCostText);
		}

	}

	for(i=0;i<40;i++)
	{
		//draw houses/hotel
		if(Property[i].HouseCount>4)
		{
			window.draw(Property[i].TileHotel);
		}
		else
		{
			for(j=0;j<Property[i].HouseCount;j++)
			{
				window.draw(Property[i].TileHouse[j]);
			}
		}

	}

	for(i=0;i<4;i++)
	{
		//draw Player funds in upper right of window
		Funds[i].ElementText.setString(IntToString(Player[i].Money,0));
		window.draw(Funds[i].ElementShape);
		Funds[i].ElementText.setPosition(Funds[i].ElementText.getPosition().x,Funds[i].ElementText.getPosition().y);
		window.draw(Funds[i].ElementText);
	}

	//draw Dice/Roll Dice
	for(i=0;i<2;i++)
	{
		window.draw(Dice[i].ElementShape);
	}

	//Draw menu option buttons
	for(i=0;i<9;i++)
	{
		if(Button[i].isVisible==1)
		{
			window.draw(Button[i].ElementShape);
			window.draw(Button[i].ElementText);
		}
	}

	//Draw Community Chest/Change card spaceHolder
	for(i=0;i<2;i++)
	{
		window.draw(Cards[i].ElementShape);
	}

	//Draw NoticeBoard with updated text

	if(NoticeBoardState==0)
	{
		NoticeBoard.ElementShape.setSize(sf::Vector2f(200,200));
		NoticeBoard.ElementShape.setFillColor(sf::Color::White);
	}


	if(NoticeBoard.isVisible==1&&NoticeBoardState!=0)
	{
		window.draw(NoticeBoard.ElementShape);
		window.draw(NoticeBoard.ElementText);
	}

	for(i=14;i<16;i++)
	{
		if(Button[i].isVisible==1)
		{
			window.draw(Button[i].ElementShape);
			window.draw(Button[i].ElementText);
		}
	}

	//Draw ActivePlayer TextBox
	window.draw(TextBox[0].ElementText);

	//Draw Player Sprites
	for(i=0;i<4;i++)
	{
		window.draw(Player[i].AgentSprite);
	}

	//MAIN DRAW Call
	window.display();
	window.setFramerateLimit(60);

}

void DrawBuildBoard(void)
{
	window.clear(sf::Color(92,163,99));
	int i=0;
	int j=0;

	//draw menu selection buttons, very, VERY important
	for(i=0;i<9;i++)
	{
		if(Button[i].isVisible==1)
		{
			window.draw(Button[i].ElementShape);
			window.draw(Button[i].ElementText);
		}
	}

	//draw Player funds in upper right of window
	Funds[ActivePlayer].ElementText.setString(IntToString(Player[ActivePlayer].Money,0));
	window.draw(Funds[ActivePlayer].ElementShape);
	//Funds[i].ElementText.setPosition(Funds[i].ElementText.getPosition().x,Funds[i].ElementText.getPosition().y);
	window.draw(Funds[ActivePlayer].ElementText);

	for(i=0;i<40;i++)
	{
		if(Property[i].Type==0&&Property[i].Owner==ActivePlayer)
		{
			window.draw(Property[i].TileOutline);
			window.draw(Property[i].TileBlockColor);
			window.draw(Property[i].TileNameText);
		}
	}

	//check monopoly blocks for complete ownership
	// if true, draw -/+ buttons, else do nothing

	for(i=30;i<ButtonCount;i++)
	{
		if(Button[i].isVisible==1)
			window.draw(Button[i].ElementShape);
	}

	for(i=0;i<40;i++)
	{
		//draw houses/hotel
		if(Property[i].HouseCount>4)
		{
			window.draw(Property[i].TileHotel);
		}
		else
		{
			for(j=0;j<Property[i].HouseCount;j++)
			{
				window.draw(Property[i].TileHouse[j]);
			}
		}

	}


	window.display();
	window.setFramerateLimit(60);
}

void DrawTradeBoard(void)
{
	window.clear(sf::Color(92,163,99));
	int i=0;
	int j=0;

	//draw menu selection buttons, very, VERY important
	for(i=0;i<9;i++)
	{
		if(Button[i].isVisible==1)
		{
			window.draw(Button[i].ElementShape);
			window.draw(Button[i].ElementText);
		}
	}

	//only render tradable properties, Normal, Util, Railroad,	Owned only by the two players interacting in the trade
	for(i=0;i<40;i++)
	{
		if((Property[i].Type==0||Property[i].Type==1||Property[i].Type==2)&&Property[i].HouseCount==0)	//URGENT, props with houses can be partially traded
		{																								//temp fix if prop has house, but not whole block
			if(Property[i].Owner==TargetPlayer || Property[i].Owner==ActivePlayer)
			{
				if(PropertyDeed[i].TileOutline.getPosition().x!=PropertyDeed[i].PosX)
				{
					PropertyDeed[i].TileOutline.setPosition(PropertyDeed[i].PosX,PropertyDeed[i].PosY);
					PropertyDeed[i].TileBlockColor.setPosition(PropertyDeed[i].PosX,PropertyDeed[i].PosY);
					PropertyDeed[i].TileNameText.setPosition(PropertyDeed[i].TileBlockColor.getPosition().x,
						PropertyDeed[i].TileBlockColor.getPosition().y+PropertyDeed[i].TileBlockColor.getSize().y);
				}

				if(Property[i].Owner==TargetPlayer)
				{
					//shift Target's Properties to the right, but leave the base position alone
					PropertyDeed[i].TileOutline.setPosition(PropertyDeed[i].PosX+600,PropertyDeed[i].PosY);
					PropertyDeed[i].TileBlockColor.setPosition(PropertyDeed[i].PosX+600,PropertyDeed[i].PosY);
					PropertyDeed[i].TileNameText.setPosition(PropertyDeed[i].PosX+600,PropertyDeed[i].PosY);
					PropertyDeed[i].TileNameText.setPosition(PropertyDeed[i].TileBlockColor.getPosition().x,
						PropertyDeed[i].TileBlockColor.getPosition().y+PropertyDeed[i].TileBlockColor.getSize().y);
				}

				window.draw(PropertyDeed[i].TileOutline);
				window.draw(PropertyDeed[i].TileBlockColor);
				window.draw(PropertyDeed[i].TileNameText);
			}
		}
	}

	//Render Target player switch buttons
	for(i=0;i<4;i++)
	{
		if(Button[i+10].isVisible==1)
		{
			if(i==ActivePlayer)
			{
				Button[10+i].ElementShape.setPosition(20,Button[10+i].ElementShape.getPosition().y);
			}
			else
			{
				Button[10+i].ElementShape.setPosition(Button[10+i].PosX,Button[10+i].ElementShape.getPosition().y);
			}
			Button[10+i].ElementText.setPosition(Button[10+i].ElementShape.getPosition().x,Button[10+i].ElementShape.getPosition().y);

			window.draw(Button[10+i].ElementShape);
			window.draw(Button[10+i].ElementText);
		}
	}

	for(i=0;i<2;i++)
	{
		window.draw(Line[i].ElementShape);
	}

	for(i=27;i<30;i++)
	{
		if(i==27)
		{
			window.draw(Button[9].ElementShape);
			window.draw(Button[9].ElementText);
		}
		else
		{
			switch(CashOfferMode)
			{
			case 1:
				Button[29].ElementShape.setFillColor(sf::Color::Green);
				Button[28].ElementShape.setFillColor(sf::Color::White);
				break;
			case 2:
				Button[29].ElementShape.setFillColor(sf::Color::White);
				Button[28].ElementShape.setFillColor(sf::Color::Green);
				break;
			default:
				Button[29].ElementShape.setFillColor(sf::Color::White);
				Button[28].ElementShape.setFillColor(sf::Color::White);
				break;
			}
			window.draw(Button[i].ElementShape);
			window.draw(Button[i].ElementText);
		}
	}

	TextBox[1].ElementText.setPosition(Button[29].ElementShape.getPosition().x,Button[29].ElementShape.getPosition().y-30);
	TextBox[1].ElementText.setString(IntToString(Offer.player_GetFunds,0));

	TextBox[2].ElementText.setPosition(Button[28].ElementShape.getPosition().x,Button[28].ElementShape.getPosition().y-30);
	TextBox[2].ElementText.setString(IntToString(Offer.target_GetFunds,0));

	TextBox[3].ElementText.setPosition(Button[29].ElementShape.getPosition().x,Button[29].ElementShape.getPosition().y-60);
	TextBox[3].ElementText.setString(IntToString(Player[TargetPlayer].Money,0));

	TextBox[4].ElementText.setPosition(Button[28].ElementShape.getPosition().x,Button[28].ElementShape.getPosition().y-60);
	TextBox[4].ElementText.setString(IntToString(Player[ActivePlayer].Money,0));

	if(TargetPlayer!=5)
	{
		for(i=1;i<5;i++)
		{
		window.draw(TextBox[i].ElementText);
		}
	}

	window.display();
	window.setFramerateLimit(60);
}

void DrawTradeOfferAI(void)
{
	//Draw board similar to Trade, but only show, onnly two button matter, Yes/No


}

void TileNameToTextBox(void)
{
	//kind of unnecessary for now, no functionally lost, just cosmetic
	int i=0;
	int j=0;
	//	int k=0;
	//	char Buffer[30]={0};
	//	int BuffLen[3]={0};
	int TextBlock=1;
	int STRLEN=0;

	//Property[i].TileName;  Text string of entire Property Name
	//Property[i].TileNameText;	Text Drawable Element, top, mid, bottom
	//Property[i].TileNameTextMid
	//Property[i].TileNameTextBot

	//Property[i].TileNameText.setString(Property[i].TileName);
	system("cls");
	for(i=0;i<40;i++)
	{
		//if i==10 then do different action, jail is special


		TextBlock=1;
		STRLEN=strlen(Property[i].TileName);
		for(j=0;j<STRLEN;j++)
		{
			if(Property[i].TileName[j]==' ')
				TextBlock++;
		}

		printf("%d,%s\n",TextBlock,Property[i].TileName);
	}
	system("PAUSE");






}

void StartUpPlayer(void)
{
	int i=0;
	Player[0].AgentTexture.loadFromFile("bits/sprits/P1.png");
	Player[1].AgentTexture.loadFromFile("bits/sprits/P2.png");
	Player[2].AgentTexture.loadFromFile("bits/sprits/P3.png");
	Player[3].AgentTexture.loadFromFile("bits/sprits/P4.png");

	//int StartX=Property[0].TileOutline.getPosition().x;
	//int StartY=Property[0].TileOutline.getPosition().y
	//		+Property[0].TileOutline.getSize().y-20;

	for(i=0;i<4;i++)
	{
		Player[i].AgentSprite.setTexture(&Player[i].AgentTexture);
		Player[i].AgentSprite.setPosition(0,0);
		Player[i].AgentSprite.setSize(sf::Vector2f(20,20));
		Player[i].Money=1500;
		Player[i].Pos=0;
		Player[i].Rot=0;
		Player[i].inJail=0;
		Player[i].isBankrupt=0;
		Player[i].DB_roll=0;
		Player[i].DB_count=0;
		Player[i].ID=i;
		Player[i].GetOutJail[0]=0;	//chest GetOut
		Player[i].GetOutJail[1]=0;	//chance GetOut
	}

	//StartX=Property[0].TileOutline.getPosition().y+Property[0].TileOutline.getSize().y-Player[0].AgentSprite.getSize().y*2;
	//StartY=Property[0].TileOutline.getPosition().x+Property[0].TileOutline.getSize().x-Player[0].AgentSprite.getSize().x*2;

	for(i=0;i<4;i++)
	{
		Player[i].AgentSprite.setPosition(Property[0].TileOutline.getPosition().x+MovementX[i+4],Property[0].TileOutline.getPosition().y+MovementY[i+4]);
	}

	FILE *fp;

	fp=fopen("bits/ini_values/PlayerAI.txt","r");
	for(i=0;i<4;i++)
	{
		fscanf(fp,"%d",&Player[i].hasAI);
	}
	fclose(fp);

		//	BG_img[0].loadFromFile("bits/Property/Prop1.png");
		//	MenuButton[10].setTexture(&BG_img[19]);
}

void PlayerMovement(void)
{

	MovementActive=1;
	while(Player[ActivePlayer].AgentSprite.getPosition().x!=Player[ActivePlayer].PosX||
			Player[ActivePlayer].AgentSprite.getPosition().y!=Player[ActivePlayer].PosY||
			Player[ActivePlayer].AgentSprite.getRotation()!=Property[Player[ActivePlayer].Pos].TileOutline.getRotation())
	{
		//Either X or Y is not aligned with new position, keep moving
		//if with 5 px of target, just jump to taget
		if(Player[ActivePlayer].AgentSprite.getPosition().x<Player[ActivePlayer].PosX+5&&Player[ActivePlayer].AgentSprite.getPosition().x>Player[ActivePlayer].PosX-5)
		{
			Player[ActivePlayer].AgentSprite.setPosition(Player[ActivePlayer].PosX,Player[ActivePlayer].AgentSprite.getPosition().y);
		}
		else
		{
			if(Player[ActivePlayer].AgentSprite.getPosition().x>Player[ActivePlayer].PosX)
				Player[ActivePlayer].AgentSprite.move(sf::Vector2f(-9,0));
			else
				Player[ActivePlayer].AgentSprite.move(sf::Vector2f(9,0));
		}

		if(Player[ActivePlayer].AgentSprite.getPosition().y<Player[ActivePlayer].PosY+5&&Player[ActivePlayer].AgentSprite.getPosition().y>Player[ActivePlayer].PosY-5)
			Player[ActivePlayer].AgentSprite.setPosition(Player[ActivePlayer].AgentSprite.getPosition().x,Player[ActivePlayer].PosY);
		else
		{
			if(Player[ActivePlayer].AgentSprite.getPosition().y>Player[ActivePlayer].PosY)
				Player[ActivePlayer].AgentSprite.move(sf::Vector2f(0,-9));
			else
				Player[ActivePlayer].AgentSprite.move(sf::Vector2f(0,9));
		}
		//sf::sleep(sf::milliseconds(500));		//wait 1 sec

		if(Player[ActivePlayer].AgentSprite.getRotation()<Player[ActivePlayer].Rot)
			Player[ActivePlayer].AgentSprite.rotate(10);
		if(Player[ActivePlayer].AgentSprite.getRotation()>Player[ActivePlayer].Rot)
		{
			if(Player[ActivePlayer].Rot==0&&Player[ActivePlayer].AgentSprite.getRotation()>180)
			{
				Player[ActivePlayer].AgentSprite.rotate(30);
			}
			else
				Player[ActivePlayer].AgentSprite.rotate(-30);
		}


		DrawGameBoard();
	}
	MovementActive=0;

	/*		OLD player movement around board
	int i=0;
	for(i=0;i<DiceTotal;i++)
	{
		window.clear();

		Player[ActivePlayer].Pos++;
		if(Player[ActivePlayer].Pos>39)
		{
			Player[ActivePlayer].Pos-=40;
			Player[ActivePlayer].Money+=200;
		}
		DrawGameBoard();
	}
	*/
	if(GameLoad==0)
		PropertyHandler();

}

void SaveGame(void)
{
	//Save game state of:
	//Player 0 - 3 Position
	//Player 0 - 3 Money
		//Player 0 - 3 Owned Properties ::Redundant
	//Player 0 - 3 Flags (inJail, isBankrupt, DBcount)
	//Property 0 - 39 House Count
	//Property 0 - 39 Owner
	//	-1:Not owned
	//	 0-3:Player Owner

	int i=0;

	FILE *fp;

	fp=fopen("bits/SaveGame.txt","w");

	for(i=0;i<4;i++)
	{
		fprintf(fp,"%d %d %d %d %d %d %d\n",
			Player[i].Money,Player[i].Pos,Player[i].isBankrupt,
			Player[i].inJail,Player[i].DB_roll,Player[i].ID,Player[i].hasAI);
	}
	for(i=0;i<40;i++)
	{
		fprintf(fp,"%d %d %d %d\n",
			Property[i].Owner,Property[i].HouseCount,
			Property[i].isMortgage,Property[i].ID);
	}

	fclose(fp);
	system("cls");
	printf("Game Saved");
}

void LoadGame(void)
{
	//	fscanf(fp,"%d",&PropColorBuff[j]);
	int i=0;

	FILE *fp;
	fp=fopen("bits/SaveGame.txt","r");

	for(i=0;i<4;i++)
	{
		fscanf(fp,"%d %d %d %d %d %d %d",
			&Player[i].Money,&Player[i].Pos,&Player[i].isBankrupt,
			&Player[i].inJail,&Player[i].DB_roll,&Player[i].ID,&Player[i].hasAI);
	}
	for(i=0;i<40;i++)
	{
		fscanf(fp,"%d %d %d %d",
			&Property[i].Owner,&Property[i].HouseCount,
			&Property[i].isMortgage,&Property[i].ID);
	}
	fclose(fp);

	system("cls");
	for(i=0;i<4;i++)
	{
		printf("%d %d %d %d %d %d\n",
			Player[i].Money,Player[i].Pos,Player[i].isBankrupt,
			Player[i].inJail,Player[i].DB_roll,Player[i].ID);
	}
	for(i=0;i<40;i++)
	{
		printf("%d %d %d %d\n",
			Property[i].Owner,Property[i].HouseCount,
			Property[i].isMortgage,Property[i].ID);
	}

	printf("Game Loaded");

	GameLoad=1;
	for(i=0;i<4;i++)
	{
		ActivePlayer=i;
		SetPlayerPosition(0);
		PlayerMovement();

	}
	GameLoad=0;
	//StartUpPropertySpec();
}

int ButtonHandler(void)
{
	int i=0;
	int j=0;
	int MouseX=sf::Mouse::getPosition(window).x;
	int MouseY=sf::Mouse::getPosition(window).y;

	int ButtonClicked=-1;

	int ButtonWidth=0;
	int ButtonHeight=0;
	int ButtonPosX=0;
	int ButtonPosY=0;
	//runs through all button elements and check is any were clicked
	//return value of button that is clicked, -1 for none, 0-button0, 1-button1  ....
	//Handles normal buttons for frame

	if(MovementActive==0)	//save guard from clicking while players are still moveing around board
	{
		for(i=0;i<ButtonCount;i++)
		{
			ButtonWidth=Button[i].ElementShape.getSize().x;
			ButtonHeight=Button[i].ElementShape.getSize().y;
			ButtonPosX=Button[i].ElementShape.getPosition().x;
			ButtonPosY=Button[i].ElementShape.getPosition().y;

			if((MouseX>ButtonPosX&&MouseX<ButtonPosX+ButtonWidth)&&(MouseY>ButtonPosY&&MouseY<ButtonPosY+ButtonHeight)&&Button[i].isVisible==1)
			{
				ButtonClicked=i;
				printf("Button %d Pressed\n",i);
				if(ActiveFrame==2)
				{
					if((ButtonClicked>9&&ButtonClicked<14)&&(ButtonClicked-10!=ActivePlayer))
						TargetPlayer=i-10;
				}

			}
		}

		if(ActiveFrame==2)
		{
			//Add support for clickable property deeds
			for(i=0;i<40;i++)
			{
				ButtonWidth=PropertyDeed[i].TileOutline.getSize().x;
				ButtonHeight=PropertyDeed[i].TileOutline.getSize().y;
				ButtonPosX=PropertyDeed[i].TileOutline.getPosition().x;
				ButtonPosY=PropertyDeed[i].TileOutline.getPosition().y;

				if((MouseX>ButtonPosX&&MouseX<ButtonPosX+ButtonWidth)&&(MouseY>ButtonPosY&&MouseY<ButtonPosY+ButtonHeight)&&Property[i].Type<3)
				{
					ButtonClicked=i+200;
					printf("Button %d Pressed\n",i+200);
				}
			}
		}

		if(ButtonClicked==9)
			printf("Submit Trade was clicked\n");
	}

	return ButtonClicked;
}

void PlayerActionHandler(int ButtonClicked)
{
	//Handles player action based on game states, frames, button clicked		MAIN HEART OF GAME PLAY

	//current active player	(0 - 3)
	//main board, trade, build, mortgage (0 - 3)
	//is NoticeBoard Action and in what state, buy, rent, rr, util, tax, ( chance, chest different notice board, still under same tent)
		//get number range from PropertyHandler Function, WIP

	switch(ActiveFrame)
	{
	case 0:
		//Main Game Board
		MainBoardAction(ButtonClicked);
		break;
	case 1:
		//Build
		BuildAction(ButtonClicked);
		break;
	case 2:
		//Trade
		TradeAction(ButtonClicked);
		break;
	case 3:
		//Mortgage
		//MortgageAction(ButtonClicked);
		break;
	default:
		printf("ActiveFrame Out of Range: (%d)\n",ActiveFrame);//error check
		break;
	}


}

char* IntToString(int BaseNum, int setting)
{
	//since built in string library are not working, have to do a lot of manual work
	//also acts as a fun data processing challenge

	//!!! while Buffer Length is 20 minus 1 for /0 still limit loop for 1 Mil - 1  !!!
	//can go beyond limit with recursive loop

	//setting is toggle to add "$" to front text string
	//setting 1 - no addition to number string
	//setting 0 - addition of "$" to number string
	//setting 2 - addition of "Player " to number string

	//BaseNum=12345;	//debug override
	//setting=1;

	//char Buffer[10];	//dont have to append /0 at the end when zero entire buffer from start

	int i=0;	//index count
	int j=10;	//decimal index
	int DecimalCount=0;

	switch(setting)
	{
	case 0:
		strcpy(StringBuffer,"$");
		i++;
		break;
	case 2 ... 3:
		strcpy(StringBuffer,"Player ");
		i+=7;
		break;
	default:
		break;
	}

	if(BaseNum<0)
	{
		//ABS values, add ( - ) to string,	fixed long running bug of neg numbers breaking this
		BaseNum=abs(BaseNum);
		StringBuffer[i]='-';
		i++;
	}

	while(BaseNum%j!=BaseNum)
	{
		//if the number is 9 or less then this will get skipped
		j*=10;
		DecimalCount++;
	}

	while(DecimalCount>0)
	{
		//Decimal is greater than 0, 10's and above
		//	( Num % ( T * 10 ) - Num % T ) / T = H		T= Ten's Factor, H=output, Num=input	T=(10^DecimalCount)
		// Int to ASCII Nun+48

		int T=DecimalCount;
		int Num=BaseNum;
		double Power=pow(10,T);
		int Tens=Power;			//convert double to int for modulus(%) to work
		int ResultTop=Num%(Tens*10);
		int ResultBot=Num%(Tens);

		int result=(ResultTop-ResultBot)/(Tens);

		//printf("DC:%d : %d %d %d \n",DecimalCount,ResultTop,ResultBot,result);

		StringBuffer[i]=result+48;
		DecimalCount--;
		i++;

	}
	//handle 1's
	StringBuffer[i]=(BaseNum%10)+48;
	i++;
	StringBuffer[i]='\0';

	//printf("Function:%s\n",Buffer);

	if(setting==3)
	{
		strcat(StringBuffer,"'s Turn");
	}



/*
	Funds[0].ElementText.setString

	fscanf(fp,"%s",&buffer[0]);
	Property[i].TileCostText.setString(buffer);
	Property[i].TileCostTextBackUp.setString(Property[i].TileCostText.getString());
*/
	return StringBuffer;
}

void PropertyHandler(void)
{
	//handles player landing on property, buy, pay rent/cost
	//input of ActivePlayer Number

	//get property type
	//0-Normal,1-Util,2-RR,3-taxes,4-chest,5-chance,6-go,7-jail/visting,8-parking,9-goto jail

	int ActiveProperty=Player[ActivePlayer].Pos;
	NoticeBoard.isVisible=0;
	Button[14].isVisible=0;
	Button[15].isVisible=0;
	NoticeBoardState=Property[ActiveProperty].Type+1;

	switch(Property[ActiveProperty].Type)
	{
	case 0 ... 2:
	//Property Normal, Utility, RailRoad
		NoticeBoard.isVisible=1;
		NoticeBoard.ElementShape.setFillColor(sf::Color::White);
		//Enable/set bits for NoticeBoardStatus Rendering
		if(Property[ActiveProperty].Owner<0)
		{
			switch(Property[ActiveProperty].Type)
			{
			case 0:
				NoticeBoard.ElementText.setString("Buy Vacant Property");
				break;
			case 1:
				NoticeBoard.ElementText.setString("Buy Vacant Utility");
				break;
			case 2:
				NoticeBoard.ElementText.setString("Buy Vacant RailRoad");
				break;
			default:
				break;
			}

			//Property NOT Owned, does player have funds to buy
			if(Player[ActivePlayer].Money>Property[ActiveProperty].BuyCost)
			{
				Button[14].isVisible=1;
				Button[15].isVisible=1;
			}
			else
				Button[15].isVisible=1;

		}
		else
		{
			//Property IS Owned, add check for bankrupt, builder menu / mortgage menu / remove player
			if(Property[ActiveProperty].Owner==ActivePlayer)
			{
				NoticeBoardState=0;
				NoticeBoard.isVisible=0;
			}
			else
			{
				NoticeBoard.ElementText.setString("Pay Rent");
				Button[14].isVisible=1;
			}
		}
		break;
	case 3:
	//Property Taxes, Tax value is property buy values, allow for different tax values
		Player[ActivePlayer].Money-=Property[ActiveProperty].BuyCost;
		NoticeBoardState=0;
		break;
	case 4:
	//Community Chest
		Button[14].isVisible=1;
		NoticeBoard.isVisible=1;
		NoticeBoardState=5;
		ChestFunc();
		break;
	case 5:
	//Chance
		Button[14].isVisible=1;
		NoticeBoard.isVisible=1;
		NoticeBoardState=6;
		ChanceFunc();
		break;
	case 6:
	//GO
		Player[ActivePlayer].Money+=200;
		NoticeBoardState=0;
		break;
	case 7:
	//Just Visting / Jail
		//Still need to work out how jail works
		NoticeBoardState=0;
		break;
	case 8:
	//Free Parking
		Player[ActivePlayer].Money+=500;
		NoticeBoardState=0;
		break;
	case 9:
	//GO TO JAIL
		Player[ActivePlayer].inJail=1;
		Player[ActivePlayer].Pos=10;
		SetPlayerPosition(0);

		NoticeBoardState=0;
		break;
	default:
		printf("PropertyHandler Type LookUp Failed\n");
		break;
	}





}

int RollDice(void)
{
	//Rolls dice
	//RND number for two dice, set Rolled Double flag for active player if needed
	int i=0;
	int DiceTotal=0;
	for(i=0;i<2;i++)
	{
		Dice[i].value=((rand()%6)+1);
		//if(ActivePlayer==0)
		//	scanf("%d",&Dice[i].value);		//Debug force in dice roll values for human player
		switch(Dice[i].value)
		{
		case 1:
			Dice[i].ElementTexture.loadFromFile("bits/sprits/Die1.png");
			break;
		case 2:
			Dice[i].ElementTexture.loadFromFile("bits/sprits/Die2.png");
			break;
		case 3:
			Dice[i].ElementTexture.loadFromFile("bits/sprits/Die3.png");
			break;
		case 4:
			Dice[i].ElementTexture.loadFromFile("bits/sprits/Die4.png");
			break;
		case 5:
			Dice[i].ElementTexture.loadFromFile("bits/sprits/Die5.png");
			break;
		case 6:
			Dice[i].ElementTexture.loadFromFile("bits/sprits/Die6.png");
			break;
		default:
			printf("Dice Roll Texture Broke\n");
			break;
		}
		Dice[i].ElementShape.setTexture(&Dice[i].ElementTexture);
		DiceTotal+=Dice[i].value;
	}

	if(Dice[0].value==Dice[1].value)
		Player[ActivePlayer].DB_roll=1;
	else
		Player[ActivePlayer].DB_roll=0;

	DiceRollCount++;
	return DiceTotal;
}

int AI_TURN(void)
{
	//AI simulates button pressed, more advance than a random number generator

	//ButtonClickedState, simulated button clicked based of NoticeBoard, ActiveFrame, PropertyHandlerState
	//default to 0, end turn

	//0-Normal,1-Util,2-RR,3-taxes,4-chest,5-chance,6-go,7-jail/visting,8-parking,9-goto jail

	//button numbers;	 14: Accept	| 15: Decline

	//AI rolls dice, buys property, pays rent

	int ActiveProperty=Player[ActivePlayer].Pos;
	int isOwned=Property[ActiveProperty].Owner;
	int AI_ButtonClicked=-1;;


	switch(NoticeBoardState)
	{
	case 0:
		//No NoticeBoard, AI can roll dice, end turn, build, trade
		if(Player[ActivePlayer].hasRolled==0)
			AI_ButtonClicked=8;
		else
		{
			//AI has rolled Dice and done everything else required from the property, build, trade, mortgage, end turn


			AI_ButtonClicked=0;
		}
		break;
	case 1:
		//NoticeBoard is up for Normal Property
		if(Player[ActivePlayer].Money<Property[ActiveProperty].BuyCost&&isOwned==-1)
		{
			//AI doesnt have enough cash
			AI_ButtonClicked=15;
		}
		else
			AI_ButtonClicked=14;
		break;
	case 2:
		//NoticeBoard is up for Utility
		if(Player[ActivePlayer].Money<Property[ActiveProperty].BuyCost&&isOwned==-1)
		{
			//AI doesnt have enough cash
			AI_ButtonClicked=15;
		}
		else
			AI_ButtonClicked=14;
		break;
	case 3:
		//NoticeBoard is up for RailRoad
		if(Player[ActivePlayer].Money<Property[ActiveProperty].BuyCost&&isOwned==-1)
		{
			//AI doesnt have enough cash
			AI_ButtonClicked=15;
		}
		else
			AI_ButtonClicked=14;
		break;
	case 4:
		//Tax
		//ButtonClickedState=14;
		break;
	case 5:
		//Chest
		AI_ButtonClicked=14;
		break;
	case 6:
		//Chance
		AI_ButtonClicked=14;
		break;
	case 7:
		//GO
		break;
	case 8:
		//Just Visting
		//ButtonClickedState=14;
		break;
	case 9:
		//Parking
		break;
	case 10:
		//Go To Jail
		//ButtonClickedState=14;
		break;
	default:
		//Error catching
		break;
	}

		//sf::sleep(sf::milliseconds(500));		//wait 1 sec
	printf("AI Clicked:%d\tNBS:%d\n",AI_ButtonClicked,NoticeBoardState);
	return AI_ButtonClicked;
}

void CheckMonopolyStatus(int PlayerID)
{
	//ActivePlayer.MonopoliesOwned
	//MonopolyBlockLookUp[8][3]

	int i=0;
	int j=0;						//# of owned properties
	int l=0;
	int m=0;
	int k=0;
	int ActiveProperty=0;
	float BlockCount[8]={0};			//completion status of monopoly
	float BlockCountMax[8]={0};
	int PropertiesOwned[40]={0};	//0=GO, shouldn't be modable, isn't yet, but shold never be


	for(i=0;i<40;i++)
	{
		if(Property[i].Owner==PlayerID&&Property[i].Type==0)
		{
			//only build list of properties that support houses
			PropertiesOwned[j]=i;
			j++;
		}
	}

	for(i=0;i<j;i++)
	{
		printf("%d\n",PropertiesOwned[i]);
	}

	//3D array check player owned properties to properties within monopoly lookup table
	for(m=0;m<3;m++)
	{
		for(l=0;l<8;l++)
		{
			for(i=0;i<j;i++)
			{
				if(PropertiesOwned[i]==MonopolyBlockLookUp[l][m])
				{
					BlockCount[l]++;
				}
			}
			if(MonopolyBlockLookUp[l][m]!=0)
			{
				BlockCountMax[l]++;
			}
		}
	}

	//update player data to show completed monopoly block owned
	for(i=0;i<8;i++)
	{
		Player[PlayerID].MonopoliesOwned[i]=BlockCount[i]/BlockCountMax[i];
		//printf("%d / %d || ",BlockCount[i],BlockCountMax[i]);
		printf("%f ||",Player[PlayerID].MonopoliesOwned[i]);
	}
	printf("\n");

	//update build buttons visible (add / sub houses)
	for(i=0;i<40;i++)
	{
		if(Property[i].hasColor==1&&Property[i].Owner==PlayerID&&Property[i].Type==0)
		{
			if(Property[i].HouseCount!=0)
				Button[(i*2)+31].isVisible=1;
			else
				Button[(i*2)+31].isVisible=0;

			if(Property[i].HouseCount!=5)
				Button[(i*2)+30].isVisible=1;
			else
				Button[(i*2)+30].isVisible=0;

		}

	}

	//go through monopolies owned, disable (+/-) buttons
	for(i=0;i<8;i++)
	{
		if(Player[PlayerID].MonopoliesOwned[i]<0.8)		//change to decimal value to show not only if complete of how much	MonopoliesOwned[i]<.8, avoid FP error
		{
			//hide buttons that player does not own full monopolies
			for(k=0;k<BlockCountMax[i];k++)
			{
				ActiveProperty=MonopolyBlockLookUp[i][k];
					Button[(ActiveProperty*2)+30].isVisible=0;
					Button[(ActiveProperty*2)+31].isVisible=0;
			}
		}
	}




}

void HouseBuildBalancer(int ActiveProperty)
{
	//keeps house count built balanced
	int i=0;
	int j=0;
	int k=0;
	int ActiveBlock=0;
	int BlockMaxCount[8]={0};
	int ActiveExtraProperties[3]={0};	//Property ID for other properties within ActiveBlock
	int AdditonHouseBill=0;	//add extra houses to keep balance, button clicked from ActionHandler still builds on active property

	//enable / disable -/+ buttons based on min/max house count
	if(Property[ActiveProperty].HouseCount!=0)
		Button[(ActiveProperty*2)+31].isVisible=1;
	else
		Button[(ActiveProperty*2)+31].isVisible=0;

	if(Property[ActiveProperty].HouseCount!=5)
		Button[(ActiveProperty*2)+30].isVisible=1;
	else
		Button[(ActiveProperty*2)+30].isVisible=0;

	//find monopoly block that ActiveProperty is part of
	for(j=0;j<8;j++)
	{
		for(i=0;i<3;i++)
		{
			if(ActiveProperty==MonopolyBlockLookUp[j][i])
			{
				ActiveBlock=j;
			}
			if(MonopolyBlockLookUp[j][i]!=0)
			{
				BlockMaxCount[j]++;
			}
		}
	}

	//WILL resume work on balancer later, until then up to player trust

	//blocks are either 2/3 properties, 1 makes no sence, 4 isn't supported
	if(BlockMaxCount[ActiveBlock]==2)
	{
		//2 prop in block
		ActiveExtraProperties[0]=MonopolyBlockLookUp[ActiveBlock][0];
		ActiveExtraProperties[1]=MonopolyBlockLookUp[ActiveBlock][1];



	}
	else
	{
		//3 prop in block
		ActiveExtraProperties[0]=MonopolyBlockLookUp[ActiveBlock][0];
		ActiveExtraProperties[1]=MonopolyBlockLookUp[ActiveBlock][1];
		ActiveExtraProperties[2]=MonopolyBlockLookUp[ActiveBlock][2];

	}


}

void TradeOfferBuilder(int DeedClicked)
{
	//if button clicked is owned by player, add to give
		//Change Deed Boarder to Red
	//if button clicked is owned by target, add to get
		//Change Deed Board to Blue
	//if clicked again, back to black, remove from offer

	//Calcultors will be added later
	//exit, done		> Reset everything		|	send offer to AI

//
	DeedClicked-=200;

	int i=0;
	int removeVal=DeedClicked;
	int removeIndex=0;
	printf("PROPERTY DEED CLICKED: %d\n",DeedClicked);

	if(PropertyDeed[DeedClicked].TileOutline.getOutlineColor()==sf::Color::Black)
	{
		//Add property to list
		if(Property[DeedClicked].Owner==ActivePlayer&&PropertyDeed[DeedClicked].TileOutline.getOutlineColor()==sf::Color::Black)
		{
			Offer.target_GetProp[Offer.T_GP_Count]=DeedClicked;
			Offer.T_GP_Count++;
			PropertyDeed[DeedClicked].TileOutline.setOutlineColor(sf::Color::Red);
			printf("Prop add to offer\n");
		}

		if(Property[DeedClicked].Owner==TargetPlayer&&PropertyDeed[DeedClicked].TileOutline.getOutlineColor()==sf::Color::Black)
		{
			Offer.player_GetProp[Offer.P_GP_Count]=DeedClicked;
			Offer.P_GP_Count++;
			PropertyDeed[DeedClicked].TileOutline.setOutlineColor(sf::Color::Blue);
			printf("Prop add to offer\n");
		}
	}
	else
	{
		//Remove property from list
		if(Property[DeedClicked].Owner==ActivePlayer&&PropertyDeed[DeedClicked].TileOutline.getOutlineColor()==sf::Color::Red)
		{
			PropertyDeed[DeedClicked].TileOutline.setOutlineColor(sf::Color::Black);
			printf("Prop removed to offer\n");

			while(Offer.target_GetProp[removeIndex]!=removeVal)
			{
				removeIndex++;
			}

			while(removeIndex<Offer.T_GP_Count)
			{
				Offer.target_GetProp[removeIndex]=Offer.target_GetProp[removeIndex+1];
				removeIndex++;
			}
			Offer.T_GP_Count--;
		}

		if(Property[DeedClicked].Owner==TargetPlayer&&PropertyDeed[DeedClicked].TileOutline.getOutlineColor()==sf::Color::Blue)
		{
			PropertyDeed[DeedClicked].TileOutline.setOutlineColor(sf::Color::Black);
			printf("Prop removed to offer\n");

			while(Offer.player_GetProp[removeIndex]!=removeVal)
			{
				removeIndex++;
			}

			while(removeIndex<Offer.P_GP_Count)
			{
				Offer.player_GetProp[removeIndex]=Offer.player_GetProp[removeIndex+1];
				removeIndex++;
			}
			Offer.P_GP_Count--;
		}
	}

	//debug read out for lists
	printf("Target Get List:");
	for(i=0;i<Offer.T_GP_Count;i++)
	{
		printf(" %d",Offer.target_GetProp[i]);
	}
	printf("\n");

	printf("Player Get List:");
	for(i=0;i<Offer.P_GP_Count;i++)
	{
		printf(" %d",Offer.player_GetProp[i]);
	}
	printf("\n");

	//check to see if there are any properties within the deal structure
	//if true, hide other players that could be selected for trading target
	//else, render the other players so ActivePlayer can show around
	if(Offer.T_GP_Count!=0 || Offer.P_GP_Count!=0)
	{
		//There are properties in trade offer, could be give or get
		for(i=0;i<4;i++)
		{
			if(i!=ActivePlayer)
				if(i!=TargetPlayer)
					Button[10+i].isVisible=0;
		}
		//Hide the other navigation buttons as well if there is a live trade
		for(i=1;i<8;i+=2)
		{
			Button[i].isVisible=0;
		}
	}
	else
	{
		for(i=0;i<4;i++)
		{
			Button[i+10].isVisible=1;
		}
		for(i=1;i<8;i+=2)
		{
			Button[i].isVisible=1;
		}
	}




}

void PropertyExchange(int Result)
{
	int i=0;
	int j=0;
	//if AI declines, exit and do nothing
	if(Result!=1)
		return;
	//AI accepted, trade money and properties

	Player[TargetPlayer].Money+=Offer.target_GetFunds;
	Player[TargetPlayer].Money-=Offer.player_GetFunds;

	Player[ActivePlayer].Money+=Offer.player_GetFunds;
	Player[ActivePlayer].Money-=Offer.target_GetFunds;

	for(i=0;i<Offer.P_GP_Count;i++)
	{
		//Active Player get property
		Property[Offer.player_GetProp[i]].Owner=ActivePlayer;
	}

	for(i=0;i<Offer.T_GP_Count;i++)
	{
		//Target Player get property
		Property[Offer.target_GetProp[i]].Owner=TargetPlayer;
	}

	for(i=0;i<40;i++)
	{
		PropertyDeed[i].TileOutline.setOutlineColor(sf::Color::Black);
		if(i<30)
		{
			Offer.player_GetProp[i]=0;
			Offer.target_GetProp[i]=0;
		}
	}

	Offer.P_GP_Count=0;
	Offer.T_GP_Count=0;
	Offer.player_GetFunds=0;
	Offer.target_GetFunds=0;


}

int AI_Trade(void)
{
	//AI for evaluating trade deals
	int i=0;
	int j=0;
	int Accept=0;

	int TargetGain=0;
	int TargetLoss=0;

	//very simple ai for now, adds up buy cost for each property plus cash offer
	//if Target > Active, make deal

	for(i=0;i<Offer.T_GP_Count;i++)
	{
		//build Target Property Gain
		TargetGain+=Property[Offer.target_GetProp[i]].BuyCost;
	}

	for(i=0;i<Offer.T_GP_Count;i++)
	{
		//build Target Property Loss
		TargetLoss+=Property[Offer.player_GetProp[i]].BuyCost;
	}

	//add in Cash Offer
	TargetGain+=Offer.target_GetFunds;
	TargetLoss-=Offer.player_GetFunds;

	if(TargetGain>TargetLoss)
		Accept=1;

	//	More Advance AI Dev Below, look at monoply completness status


	return Accept;
}

void BoardEvaluation(int DealMaker, int DealTarget)
{
	//Part of advance Trade AI, On Hold while added rest of gameboard features

	// Generate Monpoly state struct / /array for trade evaluation part

	//Board[0]- current board, before trade
	//Board[1]- furture board, is trade goes though

	//MonopolyState[0][]- Monopoly Completion State for deal maker as a float for each block 0 - 7 , 8-Util, 9-RR
	//MonopolyState[1][]- Monopoly Completion State for deal target as a float for each block 0 - 7 , 8-Util, 9-RR

	/*

	int i=0;
	int j=0;
	int k=0;
	int p=0;
	int UtilCount[3]={0};	//0-Total, 1-Maker, 2-Target
	int RRCount[3]={0};
	int PropertyIndex[2]={0};	//Count of Properties owned by Maker,Target
	int PropertyOwnedList[2][40]={0};
	int PropBlockOwned[2][8]={0};
	int PropBlockTotal[8]={0};

	//BUILDING LIST FOR CURENT GAME BOARD

	//build list, again, for property owned by maker and target only cares about buildable property
	for(i=0;i<40;i++)
	{
		if(Property[i].Owner==DealMaker&&Property[i].Type==0)
		{
			PropertyOwnedList[0][PropertyIndex[0]]=i;
			PropertyIndex[0]++;
		}
		if(Property[i].Owner==DealTarget&&Property[i].Type==0)
		{
			PropertyOwnedList[1][PropertyIndex[1]]=i;
			PropertyIndex[1]++;
		}
	}
	// Util // RR evaluation first, its easier, scalable for >2 Utils or >4 RR's, NOT recommend though

	//Build list of all RR's and Util's and how many are owned by both Deal Maker and Deal Target
	for(i=0;i<40;i++)
	{
		if(Property[i].Type==1)
		{
			UtilCount[0]++;
			if(Property[i].Owner==DealMaker)
				UtilCount[1]++;
			if(Property[i].Owner==DealTarget)
				UtilCount[2]++;
		}
		if(Property[i].Type==2)
		{
			RRCount[0]++;
			if(Property[i].Owner==DealMaker)
				RRCount[1]++;
			if(Property[i].Owner==DealTarget)
				RRCount[2]++;
		}
	}

	//Build list property in the blocks own by each player and total props in that block
	for(p=0;p<2;p++)
	{
		for(i=0;i<7;i++)
		{
			for(j=0;j<3;j++)
			{
				for(k=0;k<PropertyIndex[p];k++)
				{
					if(PropertyOwnedList[k]==MonopolyBlockLookUp[i][j])
					{
						//Prop in block is owned by player
						PropBlockOwned[p][i]++;
					}
					if(MonopolyBlockLookUp[i][j]!=0&&p<1)
						PropBlockTotal[i]++;
				}
			}
		}
	}

	//Load in all the values into pre deal board state
	for(i=0;i<2;i++)
	{
		for(j=0;j<8;j++)
		{
			Board[0].MonopolyState[i][j]=PropBlockOwned[j][i]/PropBlockTotal[j][i];
		}
		Board[0].MonopolyState[i][8]=UtilCount[0]/UtilCount[i+1];
		Board[0].MonopolyState[i][9]=RRCount[0]/RRCount[i+1];
	}
	//BUILDING LIST FOR FUTURE GAME BOARD

	//Copy Current BoardTile to Future BoardTile
	// Feed that new data into above mess

	//load future with present
	for(i=0;i<40;i++)
	{
		FurtureProperty[i].Owner=Property[i].Owner;
	}

	//switch property
	for(i=0;i<Offer.P_GP_Count;i++)
	{
		//Active Player get property
		FurtureProperty[Offer.player_GetProp[i]].Owner=ActivePlayer;
	}

	for(i=0;i<Offer.T_GP_Count;i++)
	{
		//Target Player get property
		FurtureProperty[Offer.target_GetProp[i]].Owner=TargetPlayer;
	}


*/









}

void MainBoardAction(int ButtonClicked)
{
	int ActiveProperty=Player[ActivePlayer].Pos;
	int DiceTotalHold=0;
	int i=0;
	int RR_Count=0;

	switch(NoticeBoardState)
	{
	case 0:
	//NoticeBoard is not displayed, allow access to GameAction Buttons
		switch(ButtonClicked)
		{
		case 0:
			//End Turn, can't end until rolled
			if(Player[ActivePlayer].hasRolled==1)
			{
				Player[ActivePlayer].hasRolled=0;
				Player[ActivePlayer].DB_roll=0;
				ActivePlayer++;
				if(ActivePlayer>3)
					ActivePlayer=0;

				Button[0].isVisible=0;
				Button[8].isVisible=1;
			}

			break;
		case 1:
			//Return to Main Game Board
			ActiveFrame=0;
			break;
		case 2:
			//Save Game
			SaveGame();
			break;
		case 3:
			//Return to Build Menu
			CheckMonopolyStatus(ActivePlayer);
			ActiveFrame=1;
			break;
		case 4:
			//Load Game
			LoadGame();
			break;
		case 5:
			//Return to Trade Menu
			CashOfferMode=0;
			for(i=10;i<14;i++)
			{
				Button[i].isVisible=1;
			}
			TargetPlayer=5;
			ActiveFrame=2;
			break;
		case 6:
			//Quit Game
			window.close();
			break;
		case 7:
			//Return to Mortgage Menu
			ActiveFrame=3;
			break;
		case 8:
			//Roll Dice
				//Dice Roll Rebuild, with Jail / Double Rolls / Normal Action
			//in Jila roll 5 times, get out on 5th non-double roll, only when press end turn
			if(Player[ActivePlayer].hasRolled==0)
			{
				DiceTotalHold=RollDice();
				if(Player[ActivePlayer].inJail==0)
				{
					//SetPlayerPosition(RollDice());		//moves player around board, updates pos data
					if(Player[ActivePlayer].DB_count<2)
					{
						SetPlayerPosition(DiceTotalHold);
						if(Player[ActivePlayer].DB_roll==0)
						{
							Player[ActivePlayer].hasRolled=1;
							Player[ActivePlayer].DB_count=0;
							Button[0].isVisible=1;
							Button[8].isVisible=0;
						}
						else
						{
							Player[ActivePlayer].hasRolled=0;
							Player[ActivePlayer].DB_count++;
						}
					}
					else
					{
						//player has rolled 3 doubles in a roll, on 3rd do right to jail, no action
						Player[ActivePlayer].inJail=1;
						Player[ActivePlayer].DB_count=0;
						Player[ActivePlayer].Pos=10;		//Hard Code 10 as jail, could change to look for first tile that has jail type
						SetPlayerPosition(0);
						Player[ActivePlayer].hasRolled=1;
						Button[0].isVisible=1;
						Button[8].isVisible=0;
					}
				}
				else
				{
					//JAIL, rolled 3 doubles or got sent to JAIL
					if(Player[ActivePlayer].DB_roll==1||Player[ActivePlayer].DB_count>1)
					{
						//Player has rolled Double, free to leave
						Player[ActivePlayer].inJail=0;
						Player[ActivePlayer].DB_count=0;
						Player[ActivePlayer].DB_roll=0;
						SetPlayerPosition(DiceTotalHold);
						Player[ActivePlayer].hasRolled=1;
						Button[0].isVisible=1;
						Button[8].isVisible=0;
					}
					else
					{
						//Player in jail, did not roll double, stay in jail for max three rolls, 4 move out auto
						Player[ActivePlayer].DB_count++;
						Player[ActivePlayer].hasRolled=1;
					}
				}
			}
			break;
		default:
			//catch when ButtonClicked is (-1)
			break;
		}
		break;
	case 1 ... 3:
	//Property may have owner, pay rent / buy property
		if(ButtonClicked==15||ButtonClicked==14)
		{
			if(ButtonClicked==14)
			{
				if(Property[ActiveProperty].Owner<0)
				{
					Property[ActiveProperty].Owner=ActivePlayer;
					Player[ActivePlayer].Money-=Property[ActiveProperty].BuyCost;
				}
				else
				{
					switch(Property[ActiveProperty].Type)
					{
					case 0:
						//Pay Rent Normal Property
						Player[Property[ActiveProperty].Owner].Money+=Property[ActiveProperty].Rent[Property[ActiveProperty].HouseCount];
						Player[ActivePlayer].Money-=Property[ActiveProperty].Rent[Property[ActiveProperty].HouseCount];
						break;
					case 1:
						//Pay Rent Utility
						if(Property[12].Owner==Property[28].Owner)	//bit hard coded. will come back later
						{
							// dice roll x10
							//DiceTotal();
							Player[Property[ActiveProperty].Owner].Money+=(RollDice()*10);
							Player[ActivePlayer].Money-=(RollDice()*10);
						}
						else
						{
							//dice roll x4
							Player[Property[ActiveProperty].Owner].Money+=(RollDice()*4);
							Player[ActivePlayer].Money-=(RollDice()*4);
						}
						break;
					case 2:
						//Pay Rent Railroad
						for(i=0;i<40;i++)
						{
							if(Property[i].Type==2&&Property[i].Owner==Property[ActiveProperty].Owner)
								RR_Count++;
						}

						Player[ActivePlayer].Money-=Property[ActiveProperty].Rent[RR_Count];
						Player[Property[ActiveProperty].Owner].Money+=Property[ActiveProperty].Rent[RR_Count];

						break;
					default:
						break;
					}

				}
			}
			NoticeBoardState=0;
			Button[14].isVisible=0;
			Button[15].isVisible=0;
			NoticeBoard.isVisible=0;
		}
		break;
	case 4:
		//tax, no action needed
		break;
	case 5:
		//Chest, Click OK
		NoticeBoardState=0;
		Button[14].isVisible=0;
		NoticeBoard.isVisible=0;
		if(ButtonClicked==14)
		{
			int error=CardAction(CommunityChest[ChestSeq[ChestPT]].Parm);
			if(error==1)
			{
				ChestPT++;
				if(ChestPT>15)
					ChestPT=0;
				error=CardAction(CommunityChest[ChestSeq[ChestPT]].Parm);
			}

		}
		ChestPT++;
		if(ChestPT>15)
			ChestPT=0;

		break;
	case 6:
		//Chance. Click OK
		NoticeBoardState=0;
		Button[14].isVisible=0;
		NoticeBoard.isVisible=0;
		if(ButtonClicked==14)
		{
			int error=CardAction(Chance[ChanceSeq[ChancePT]].Parm);
			if(error==1)
			{
				ChancePT++;
				if(ChancePT>15)
					ChancePT=0;
				error=CardAction(Chance[ChanceSeq[ChancePT]].Parm);
			}

		}
		ChancePT++;
		if(ChancePT>15)
			ChancePT=0;
		break;
	case 7:
		//GO, no action neeeded
		break;
	case 8:
		//Jail / Visting, use in part of jail roll
		break;
	case 9:
		//Parking, no action needed
		break;
	case 10:
		//GO to JAIL, click OK
		break;
	default:
		printf("Main Board Action Handler Type ERROR\n");
		break;
	}
}

void BuildAction(int ButtonClicked)
{
	int i=0;
	int ActiveProperty=Player[ActivePlayer].Pos;

	switch(ButtonClicked)
	{
	case 1:
		ActiveFrame=0;
		break;
	case 5:
		//Return to Trade Menu
		for(i=10;i<14;i++)
		{
			Button[i].isVisible=1;
		}
		TargetPlayer=5;
		CashOfferMode=0;
		ActiveFrame=2;
		break;
	case 32 ... 109:
		if(ButtonClicked%2==1)
		{
			//remove house
			ActiveProperty=(ButtonClicked-31)/2;
			if(Property[ActiveProperty].HouseCount!=0)
			{
				Property[ActiveProperty].HouseCount--;
				Player[ActivePlayer].Money+=Property[ActiveProperty].HouseCost;
				HouseBuildBalancer(ActiveProperty);
			}
		}
		else
		{
			//add house
			ActiveProperty=(ButtonClicked-30)/2;
			if(Property[ActiveProperty].HouseCount!=5&&Player[ActivePlayer].Money>Property[ActiveProperty].HouseCost)
			{
				Property[ActiveProperty].HouseCount++;
				Player[ActivePlayer].Money-=Property[ActiveProperty].HouseCost;
				HouseBuildBalancer(ActiveProperty);
			}
		}
	break;
	}
}

void TradeAction(int ButtonClicked)
{
	int i=0;

	switch(ButtonClicked)
	{
	case 1:
		//Return to Main Board
		Offer.player_GetFunds=0;
		Offer.target_GetFunds=0;
		ActiveFrame=0;
	break;

	case 9:
		//Player is done with building trade, send offer to AI
		printf("Trade Offer Submitted\n");
		PropertyExchange(AI_Trade());
		ActiveFrame=0;

		for(i=10;i<14;i++)
		{
			Button[i].isVisible=0;
		}

		for(i=0;i<10;i++)
		{
			Button[i].isVisible=1;
		}
	break;

	case 3:
		//Return to Build Menu
		Offer.player_GetFunds=0;
		Offer.target_GetFunds=0;
		CheckMonopolyStatus(ActivePlayer);
		ActiveFrame=1;
	break;

	case 200 ... 239:
		//handles building trade offer
		TradeOfferBuilder(ButtonClicked);
	break;

	case 28:
		//Player Give Cash
		if(CashOfferMode==2)
			CashOfferMode=0;
		else
			CashOfferMode=2;
	break;

	case 29:
		//Player Get Cash
		if(CashOfferMode==1)
			CashOfferMode=0;
		else
			CashOfferMode=1;
	break;

	}
}

void MortgageAction(int ButtonClicked)
{
	switch(ButtonClicked)
	{
	case 1:
		ActiveFrame=0;
		break;
	}
}

void ChanceFunc(void)
{
	NoticeBoard.ElementShape.setFillColor(sf::Color(255,161,0));
	NoticeBoard.ElementShape.setSize(sf::Vector2f(350,200));
	NoticeBoard.ElementText.setString(Chance[ChanceSeq[ChancePT]].Text);
}

void ChestFunc(void)
{
	NoticeBoard.ElementShape.setFillColor(sf::Color::Cyan);
	NoticeBoard.ElementShape.setSize(sf::Vector2f(350,200));
	NoticeBoard.ElementText.setString(CommunityChest[ChestSeq[ChestPT]].Text);
}

int CardAction(int Parm[4])
{
	int i=0;
	int j=0;
	int HouseTotel[2]={0};

	for(i=0;i<4;i++)
	{
		if(Player[i].isBankrupt==0)
			j++;
	}

	printf("Parm: %d, %d, %d, %d\n",Parm[0],Parm[1],Parm[2],Parm[3]);

	switch(Parm[0])
	{
	case 0:
		//move player around board, fixed amount, to property, to type
		if(Parm[1]==-1)
		{
			if(Parm[2]==-1)
			{
				//Move to Type
				i=Player[ActivePlayer].Pos;
				while(Property[i].Type!=Parm[3])
				{
					i++;
					if(i>39)
						i=0;
				}
				Player[ActivePlayer].Pos=i;
			}
			else
			{
				//Move to Prop
				Player[ActivePlayer].Pos=Parm[2];
			}
		}
		else
		{
			//Move Fixed, always a positive number, will loop around if moving back 3 spaces
			Player[ActivePlayer].Pos+=Parm[1];
			if(Player[ActivePlayer].Pos>39)
				Player[ActivePlayer].Pos-=39;
		}
		SetPlayerPosition(0);
		PlayerMovement();
		NoticeBoard.ElementShape.setSize(sf::Vector2f(200,200));
		NoticeBoard.ElementShape.setFillColor(sf::Color::White);
		NoticeBoardState=0;
		NoticeBoard.isVisible=0;
		PropertyHandler();
		break;
	case 1:
		//player collect fixed $
		Player[ActivePlayer].Money+=Parm[1];
		break;
	case 2:
		//player pay fixed $
		Player[ActivePlayer].Money-=Parm[1];
		break;
	case 3:
		//player collect, rest pay
		Player[ActivePlayer].Money+=Parm[1]*j;
		for(i=0;i<4;i++)
		{
			Player[i].Money-=Parm[1];
		}
		break;
	case 4:
		//player pays, rest collect
		Player[ActivePlayer].Money-=Parm[1]*j;
		for(i=0;i<4;i++)
		{
			Player[i].Money+=Parm[1];
		}
		break;
	case 5:
		//property upkeep
		for(i=0;i<40;i++)
		{
			if(Property[i].Owner==ActivePlayer)
			{
				if(Property[i].HouseCount>4)
					HouseTotel[1]++;
				else
					HouseTotel[0]+=Property[i].HouseCount;

				Player[ActivePlayer].Money-=(HouseTotel[0]*Parm[1]+HouseTotel[1]*Parm[2]);
			}
		}
		break;
	case 6:
		//GOJF card, come back to later
		break;
	case 7:
		//GTJ, come back to later
		Player[ActivePlayer].inJail=1;
		Player[ActivePlayer].Pos=10;
		SetPlayerPosition(0);
		break;
	default:
		printf("Chest / Chance Action Type Lookup Failed\n");
		break;
	}

	return 0;
}

void SetPlayerPosition(int DiceTotal)
{
	//So what of a rebuild of PlayerTavelAnimate
	//feed in how many space to move, property 14 -> 20 (Rolled toal of 6)
	//calculate new position for ALL players, may need to move players when 2/3/4 players are at same property
	int i=0;
	int j=0;
	int playerCount=0;
	int PropRotate=0;
	int OffOffSet=0;
	//int CornerX[16]={};
	//int CornerY[16]={};


	Player[ActivePlayer].Pos+=DiceTotal;
	if(Player[ActivePlayer].Pos>39)
	{
		Player[ActivePlayer].Pos-=40;
		Player[ActivePlayer].Money+=200;
	}

	Player[ActivePlayer].Rot=Property[Player[ActivePlayer].Pos].TileOutline.getRotation();

	printf("Player %d , Pos: %d\n",ActivePlayer,Player[ActivePlayer].Pos);

	Player[ActivePlayer].PosX=Property[Player[ActivePlayer].Pos].TileOutline.getPosition().x;
	Player[ActivePlayer].PosY=Property[Player[ActivePlayer].Pos].TileOutline.getPosition().y;

	PropRotate=Property[Player[ActivePlayer].Pos].TileOutline.getRotation();

	for(i=0;i<4;i++)
	{
		if(Player[i].Pos==Player[ActivePlayer].Pos)
			playerCount++;
	}
	playerCount--;

	if(Player[ActivePlayer].Pos%10==0)
		OffOffSet=4;

	if(Player[ActivePlayer].inJail==1)
		OffOffSet=8;

	if(GameLoad==1)
	{
		OffOffSet-=ActivePlayer;
	}

	switch(PropRotate)
	{
	case 0:
		Player[ActivePlayer].PosX+=MovementX[playerCount+OffOffSet];
		Player[ActivePlayer].PosY+=MovementY[playerCount+OffOffSet];
		break;
	case 90:
		Player[ActivePlayer].PosY+=MovementX[playerCount+OffOffSet];
		Player[ActivePlayer].PosX-=MovementY[playerCount+OffOffSet];
		break;
	case 180:
		Player[ActivePlayer].PosX-=MovementX[playerCount+OffOffSet];
		Player[ActivePlayer].PosY-=MovementY[playerCount+OffOffSet];
		break;
	case 270:
		Player[ActivePlayer].PosY-=MovementX[playerCount+OffOffSet];
		Player[ActivePlayer].PosX+=MovementY[playerCount+OffOffSet];
		break;
	}


	PlayerMovement();

}

/*	AI Section
 *
 * Design Notes:
 *
 * Accepting Trade Offer from SomeOne:
 * AI only accept trade if it comes out better, gains > losses
 * AI will add weight to loss or gain based on Monopoly status 0->1 | 0->2 | 0->3 | 0->4
 * AI will value cash more if its short below set $ and relative to others
 *
 * Making Trade Offer to SomeOne:
 * AI will value cheaper properties first, have more at lesser value than fewer at greater value
 * AI will value properties to complete its current collection
 * AI will stay within current Property Limits
 * AI will stay within current Money Limits
 *
 * Seq of Actions:
 * -Start of Turn
 * Roll Dice > Move > Property Action > Loop Until Done. (Board does this automatically)
 * Remove Houses / Hotel if short on funds
 * Try to UnMortgage Properties (If have any and enough Funds)
 * Try to Build
 * Trade
 * Try to Build
 * End Turn
 * -End of AI TURN
 *
 */


void AI_MakeTrade(void)
{
	int i=0;

	//Sanitize structure from other deals
	Offer.P_GP_Count=0;
	Offer.T_GP_Count=0;
	Offer.player_GetFunds=0;
	Offer.target_GetFunds=0;
	for(i=0;i<40;i++)
	{
		Offer.player_GetProp[i]=0;
		Offer.target_GetProp[i]=0;
	}

	//Gen 1 AI, AI picks 1 to 3 rnd properties that it owns and trades with 1 to 3 rnd properties that rnd target owns



}

void AI_MakeBuild(void)
{
	//Looks at monopoly stutus, choose cheap properties are priroites, different from trade that values higher proces proprty
	int i=0;
	int TargetBlock=-1;

	int FullBuiltBlock[8]={0};	//checks for block will max houses build








	CheckMonopolyStatus(ActivePlayer);

	for(i=0;i<8;i++)
	{
		if(Player[ActivePlayer].MonopoliesOwned[i]>0.8)
		{
			//	3/3 ownership

		}
		else
		{
			//AI doesn't own complete block, check to see if it own 2/3 or 1/3 to add to wish list to trade for later
			//AI does not care about RR and Utilities
			if(Player[ActivePlayer].MonopoliesOwned[i]>0.5)
			{
				// 2/3 ownership

			}
			else
			{
				if(Player[ActivePlayer].MonopoliesOwned[i]>0.25)
				{
					//	1/3 ownership

				}
				else
				{
					//	0 ownership

				}
			}
		}
	}



}

void AI_MakeMortgage(void)
{

}
