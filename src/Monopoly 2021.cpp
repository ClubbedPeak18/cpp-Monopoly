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
void PlayerSetUp(void);				//Setup for Player Number&Drawing Data
void PlayerTravelAnimate(int);		//Animate Players moving around gameboard

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

void CheckMonopolyStatus(void);		//IN:Glogal Variables
									//OUT:ActivePlayer MonopoliesOwned

void HouseBuildBalancer(int);		//IN:Property Player is wanting to build on (ActiveProperty)
									//OUT:Draw calls and updated house count / player funds / keep house count with +/- 1

char* IntToString(int,int);			//Convert Number data to String Data
char StringBuffer[10];				//cheap woke around for local buffer within function, produces garbage text without

int TradeAI(void);					//IN:Offer Struct (Global var)
									//OUT:Yes/No to trade offer

//Offset Data for drawing Players on GameBoard
int POSXOffset[4]={10,10,35,35};	//10,10,35,35	normal(0-3) , corner(4-7)
int POSYOffset[4]={55,80,55,80};	//10,35,10,35

int POSXOffsetCorner[16]={75,75,50,25,05,05,32,56,06,06,32,56,75,75,50,25};	//Corner offsets, 0-3,4-7,8-11,12-15
int POSYOffsetCorner[16]={25,50,75,75,25,50,75,75,56,31,06,06,56,31,05,05};

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
	int inJail;
	int isBankrupt;
	int hasRolled;
	int hasAI;
	int MonopoliesOwned[8]={0};	//dirty trick to trace Completed Monopolies, sub 1 to get to LookUpTable array index

	int PosX;
	int PosY;

	sf::RectangleShape AgentSprite;
	sf::Texture AgentTexture;
	sf::Font AgentFont;
	sf::Text AgentText;
};

struct Element
{
	char ElementString[10];
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
	int player_Getfunds;
	int T_GP_Count;		//index # of props in array
	int P_GP_Count;
};

/* Frames (Menus):
 *
 * Main Game Board
 * Trading
 * Building
 * (un)Mortgage
 */

BoardTile Property[40];
BoardTile PropertyDeed[40];
Agents Player[4];

TradeStruct Offer;

const int ButtonCount=110;	// old 30

Element Button[ButtonCount];	//0-9 for numbers 0 to 9;	 10,11 for yes/no;	12,13,14,15 main,build,trade,mortgage
					//Rework for button numbers, see below

Element Line[2];		//Draw a line, used for trading menu

Element Dice[2];		//game dice
Element PlayerCount;	//displaces current ActivePlayer
Element Debugger[10];	//debug printout in game window for testing
Element Funds[4];		//displace player funds
Element Cards[2];		//chest / chance cards
Element NoticeBoard;	//use for main game board interactions
						//chance, chest, buying property, paying dues
						//can change colors/size based on situation used in
/*	Buttons used:
		Main:
			0-3;	Roll Dice/End Turn,Build,Trade,Mortgage Menus		-mostly done
			0-39;	BoardTiles
		Trade:
			0-11;	Num 0-9,Yes,No
			0-39	BoardTiles
		Build:
			0-7;	(+/-)1,(+/-)2,(+/-)3,Accept,Cancel
			0-39	BoardTiles
		Mortgage:
			0-39:	BoardTiles
		Save Game:

		Load Game:

		Quite Game:

*/


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
 *	9: reserved
 *	10: Target Player 1 ... 13: Target Player 4, used in trading menu
 *
 *	14: Accept		Universal across different states and frames
 *	15: Decline
 *
 *	16: '0'		Buttons for number pad, used in trading menu, may use keyboard numpad
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
 *	28	reserved
 *  29  reserved
 *
 *  30  add / sub house count to all properties, only draw those that matter
 *  ..
 *  110
 *
 *	for trading menu, will draw property tiles / trading cards
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
int ActiveFrame=0;			//Main Game Board, Build, Trade, Mortgage Menus
int NoticeBoardState=0;		//Notice Board version / isActive	(ActiveWindow replacement)
int PlayerActionState=0;	//State of player action, might not need global, but as backup
int PropertyHandlerState=0;	//Current Property landed on, could be made part of player struct
							//combine ButtonClickState with ActivePlayer to track player button selection
int MonopolyBlockLookUp[8][3]={0};	//Cheat, quick lookup for monopoly block properties max of 3 properties per monopoly block


int main() {
	int i=0;
	int RNDSeed=0;
	StartUpPropertyName();
	StartUpPropertyData();
	StartUpPropertySpec();
	StartUpPropertyDeed();
	StartUpGameBoard();
	PlayerSetUp();
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
		Debugger[0].ElementText.setString(IntToString(ActivePlayer+1,1));
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
						printf("AP %d AF %d NBS %d PAS %d PHS %d\n",ActivePlayer,ActiveFrame,NoticeBoardState,PlayerActionState,PropertyHandlerState);

						break;

					case Event::KeyPressed:
						if(event.key.code==Keyboard::Numpad8)
						{
							for(i=0;i<40;i++)
							{
								if(Property[i].HouseCount!=5&&Property[i].Type==0)
									Property[i].HouseCount++;
							}
						}
						if(event.key.code==Keyboard::Numpad2)
						{
							for(i=0;i<40;i++)
							{
								if(Property[i].HouseCount!=0&&Property[i].Type==0)
									Property[i].HouseCount--;
							}
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
			for(i=0;i<9;i++)
			{
				if(i%2==0)
					Button[i].isVisible=1;
			}
			DrawGameBoard();
			break;
		case 1:
			for(i=0;i<9;i++)
			{
				if(i%2==0)
					Button[i].isVisible=0;
			}
			DrawBuildBoard();
			break;
		case 2:
			for(i=0;i<9;i++)
			{
				if(i%2==0)
					Button[i].isVisible=0;
			}
			DrawTradeBoard();
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
		Button[i].isVisible=1;
	}

	//Setup for NoticeBoard interface
	Button[14].ElementShape.setPosition(200,350);	//Green
	Button[15].ElementShape.setPosition(300,350);	//Red
	Button[14].ElementText.setPosition(Button[14].ElementShape.getPosition().x,Button[14].ElementShape.getPosition().y);
	Button[15].ElementText.setPosition(Button[15].ElementShape.getPosition().x,Button[15].ElementShape.getPosition().y);
	NoticeBoard.ElementText.setPosition(NoticeBoard.ElementShape.getPosition().x,NoticeBoard.ElementShape.getPosition().y);

	//setup for Debug / UI Elemenents (Active Player Indicator)
	Debugger[0].ElementText.setPosition(1000,100);
	Debugger[0].ElementText.setCharacterSize(20);
	Debugger[0].ElementText.setColor(sf::Color::Black);
	Debugger[0].ElementText.setFont(Calibri);

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

	for(i=0;i<2;i++)
	{
		//Draw a line
		Line[i].ElementShape.setPosition(0,60);
		Line[i].ElementShape.setSize(sf::Vector2f(1400,5));
		Line[i].ElementShape.setOutlineColor(sf::Color::Black);
		Line[i].ElementShape.setOutlineThickness(2);
		Line[i].ElementShape.setFillColor(sf::Color::Black);
	}
	Line[1].ElementShape.setPosition(600,0);
	Line[1].ElementShape.setRotation(90);

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


	for(i=0;i<4;i++)
	{
		//draw players
		PropRotate=Property[Player[i].Pos].TileOutline.getRotation();
		switch(PropRotate)
		{
		case 90:
			if(Player[i].Pos==10)
			{
				//Jail/Just Visting
				if(Player[i].inJail==1)
				{
					//use position of normal property but with different x/y offsets
					Player[i].AgentSprite.setPosition(
						Property[Player[i].Pos].TileOutline.getPosition().x+POSXOffset[i+1]+50,
						Property[Player[i].Pos].TileOutline.getPosition().y+POSYOffset[i+1]+50);
				}
				else
				{
					Player[i].AgentSprite.setPosition(
						Property[Player[i].Pos].TileOutline.getPosition().x+POSXOffsetCorner[i+4]-100,
						Property[Player[i].Pos].TileOutline.getPosition().y+POSYOffsetCorner[i+4]);
				}
			}
			else
			{
				Player[i].AgentSprite.setPosition(
					Property[Player[i].Pos].TileOutline.getPosition().x+POSXOffset[i]-110,
					Property[Player[i].Pos].TileOutline.getPosition().y+POSYOffset[i]-45);
			}
			break;
		case 180:
			if(Player[i].Pos==20)
			{
				//Free Parking
				Player[i].AgentSprite.setPosition(
					Property[Player[i].Pos].TileOutline.getPosition().x+POSXOffsetCorner[i+8]-100,
					Property[Player[i].Pos].TileOutline.getPosition().y+POSYOffsetCorner[i+8]-100);
			}
			else
			{
				Player[i].AgentSprite.setPosition(
					Property[Player[i].Pos].TileOutline.getPosition().x+POSXOffset[i]-65,
					Property[Player[i].Pos].TileOutline.getPosition().y+POSYOffset[i]-155);
			}
			break;
		case 270:
			if(Player[i].Pos==30)
			{
				//Go to Jail
				Player[i].AgentSprite.setPosition(
					Property[Player[i].Pos].TileOutline.getPosition().x+POSXOffsetCorner[i+12],
					Property[Player[i].Pos].TileOutline.getPosition().y+POSYOffsetCorner[i+12]-100);
			}
			else
			{
				Player[i].AgentSprite.setPosition(
					Property[Player[i].Pos].TileOutline.getPosition().x+POSXOffset[i]+45,
					Property[Player[i].Pos].TileOutline.getPosition().y+POSYOffset[i]-110);
			}
			break;
		default:
			if(Player[i].Pos==0)
			{
				//GO
				Player[i].AgentSprite.setPosition(
					Property[Player[i].Pos].TileOutline.getPosition().x+POSXOffsetCorner[i],
					Property[Player[i].Pos].TileOutline.getPosition().y+POSYOffsetCorner[i]);
			}
			else
			{
				Player[i].AgentSprite.setPosition(
					Property[Player[i].Pos].TileOutline.getPosition().x+POSXOffset[i],
					Property[Player[i].Pos].TileOutline.getPosition().y+POSYOffset[i]);
			}
			break;
		}



		window.draw(Player[i].AgentSprite);
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
		window.draw(Button[i].ElementShape);
		window.draw(Button[i].ElementText);
	}

	//Draw Community Chest/Change card spaceHolder
	for(i=0;i<2;i++)
	{
		window.draw(Cards[i].ElementShape);
	}

	//Draw Additive Game Elements form Property Handler State
	int ActiveProperty=Player[ActivePlayer].Pos;

	switch(NoticeBoardState)
	{
	case 0:
		break;
	case 1:
		//Normal
		if(Property[ActiveProperty].Owner==-1)
			//Property is not owned
			NoticeBoard.ElementText.setString("Buy Vacant Property");
		else
		{
			//Property is owned
			if(Property[ActiveProperty].Owner!=ActivePlayer)
				NoticeBoard.ElementText.setString("Pay Rent");
			else
				NoticeBoard.ElementText.setString("Player Own Property");
		}
		break;
	case 2:
		//Utility
		if(Property[ActiveProperty].Owner==-1)
		{
			//not own
			NoticeBoard.ElementText.setString("Buy Vacant Utility");
		}
		else
		{
			//owned, but by who
			if(Property[ActiveProperty].Owner!=ActivePlayer)
				NoticeBoard.ElementText.setString("Pay Rent");
			else
				NoticeBoard.ElementText.setString("Player Own Property");
		}
		break;
	case 3:
		//RailRoad
		if(Property[ActiveProperty].Owner==-1)
		{
			//not own
			NoticeBoard.ElementText.setString("Buy Vacant RailRoad");
		}
		else
		{
			//owned, but by who
			if(Property[ActiveProperty].Owner!=ActivePlayer)
				NoticeBoard.ElementText.setString("Pay Rent");
			else
				NoticeBoard.ElementText.setString("Player Own Property");
		}
		break;
	case 4:
		//Chest
		//NoticeBoardState=0;
		break;
	case 5:
		//Chance
		//NoticeBoardState=0;
		break;
		//6-GO, 7-Visting, 8-Parking, 9-JAIL
	default:
		//NoticeBoardState=0;
		break;
	}

	//Draw NoticeBoard with updated text
	if(NoticeBoardState>0&&NoticeBoardState<4)
	{
		window.draw(NoticeBoard.ElementShape);
		window.draw(NoticeBoard.ElementText);
		window.draw(Button[14].ElementShape);	//green
		window.draw(Button[14].ElementText);
		if(Property[Player[ActivePlayer].Pos].Owner==-1)
		{
			window.draw(Button[15].ElementShape);	//red
			window.draw(Button[15].ElementText);
			Button[15].isVisible=1;
		}
		Button[14].isVisible=1;
	}
	else
	{
		Button[14].isVisible=0;
		Button[15].isVisible=0;
	}

	//Draw ActivePlayer using debugger placeholder elements
	window.draw(Debugger[0].ElementText);

	//MAIN DRAW Call
	window.display();
	window.setFramerateLimit(30);

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
	window.setFramerateLimit(30);
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

	window.display();
	window.setFramerateLimit(30);
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

void PlayerSetUp(void)
{
	int i=0;
	Player[0].AgentTexture.loadFromFile("bits/sprits/P1.png");
	Player[1].AgentTexture.loadFromFile("bits/sprits/P2.png");
	Player[2].AgentTexture.loadFromFile("bits/sprits/P3.png");
	Player[3].AgentTexture.loadFromFile("bits/sprits/P4.png");

	int StartX=Property[0].TileOutline.getPosition().x;
	int StartY=Property[0].TileOutline.getPosition().y
			+Property[0].TileOutline.getSize().y-20;

	for(i=0;i<4;i++)
	{
		Player[i].AgentSprite.setTexture(&Player[i].AgentTexture);
		Player[i].AgentSprite.setPosition(POSXOffset[i],POSYOffset[i]);
		Player[i].AgentSprite.setSize(sf::Vector2f(20,20));
		Player[i].Money=1500;
		Player[i].Pos=0;
		Player[i].inJail=0;
		Player[i].isBankrupt=0;
		Player[i].DB_roll=0;
		Player[i].ID=i;
	}

	StartX=Property[0].TileOutline.getPosition().y+Property[0].TileOutline.getSize().y-Player[0].AgentSprite.getSize().y*2;
	StartY=Property[0].TileOutline.getPosition().x+Property[0].TileOutline.getSize().x-Player[0].AgentSprite.getSize().x*2;

	for(i=0;i<4;i++)
	{
		Player[i].AgentSprite.setPosition(StartX-POSXOffset[i],StartY-POSYOffset[i]);
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

void PlayerTravelAnimate(int DiceTotal)
{
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
				printf("Button %d Pressed\n",i);
			}
		}
		for(i=10;i<14;i++)
		{
			ButtonWidth=Button[i].ElementShape.getSize().x;
			ButtonHeight=Button[i].ElementShape.getSize().y;
			ButtonPosX=Button[i].ElementShape.getPosition().x;
			ButtonPosY=Button[i].ElementShape.getPosition().y;

			if((MouseX>ButtonPosX&&MouseX<ButtonPosX+ButtonWidth)&&(MouseY>ButtonPosY&&MouseY<ButtonPosY+ButtonHeight)&&Button[i].isVisible==1)
			{
				ButtonClicked=i;
				printf("Button %d Pressed\n",i);
				TargetPlayer=i-10;
				for(j=0;j<4;j++)
				{
					if(j!=ActivePlayer)
						if(j!=TargetPlayer)
							Button[10+j].isVisible=0;
				}
			}
		}

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

	//button pressed (0 - 29)
	int i=0;
	int ActiveProperty=Player[ActivePlayer].Pos;
	switch(ActiveFrame)
	{
	case 0:
		//Main Game Board
		switch(NoticeBoardState)	//may make NoticeBoardState a part of player struct
		{
		case 0:
			//no notice board is drawn, carry on as normal for button 0 - 8 for main game board options
			switch(ButtonClicked)
			{
			case 0:
				//End Turn, can't end until rolled
				if(Player[ActivePlayer].hasRolled==1)
				{
					Player[ActivePlayer].hasRolled=0;
					ActivePlayer++;
					if(ActivePlayer>3)
						ActivePlayer=0;
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
				CheckMonopolyStatus();
				ActiveFrame=1;
				break;
			case 4:
				//Load Game
				LoadGame();
				break;
			case 5:
				//Return to Trade Menu
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
				if(Player[ActivePlayer].hasRolled==0)
				{
					PlayerTravelAnimate(RollDice());
					Player[ActivePlayer].hasRolled=1;
				}
				break;
			default:
				//catch when ButtonClicked is (-1)
				break;
			}
			break;
		case 1:
			//Player landed on Property
				//diable all buttons but "accept"(14) and "decline"(15)
			//check if Property is OWNED
			if(Property[ActiveProperty].Owner==-1)
			{
				//property is NOT OWNED
				//accept/decline, update property values, player funds
				if(ButtonClicked==14)
				{
					//buy property purchase
					Property[ActiveProperty].Owner=ActivePlayer;
					Player[ActivePlayer].Money-=Property[ActiveProperty].BuyCost;
					NoticeBoardState=0;
				}
				if(ButtonClicked==15)
				{
					//decline property purchase
					NoticeBoardState=0;
				}
			}
			else
			{
				//property is OWNED
				if(Property[ActiveProperty].Owner==ActivePlayer)
				{
					//Player Owned, no action
					if(ButtonClicked==14)
					{
						//acknowledge, slow game speed down
						NoticeBoardState=0;
					}
				}
				else
				{
					//Other Player Owned, pay rent
					if(ButtonClicked==14)
					{
						//acknowledge, slow game speed down
						NoticeBoardState=0;
						Player[Property[ActiveProperty].Owner].Money+=Property[ActiveProperty].Rent[Property[ActiveProperty].HouseCount];
						Player[ActivePlayer].Money-=Property[ActiveProperty].Rent[Property[ActiveProperty].HouseCount];
					}
				}

			}
			break;
		case 2:
			//Player landed on Utility
				//Roll Dice for Rent Price
			if(Property[ActiveProperty].Owner==-1)
			{
				//Utility is NOT OWNED
				//accept/decline, update property values, player funds
				if(ButtonClicked==14)
				{
					//buy Utility purchase
					Property[ActiveProperty].Owner=ActivePlayer;
					Player[ActivePlayer].Money-=Property[ActiveProperty].BuyCost;
					NoticeBoardState=0;
				}
				if(ButtonClicked==15)
				{
					//decline Utility purchase
					NoticeBoardState=0;
				}
			}
			else
			{
			//Utility is OWNED
				//acknowledge, slow game speed down

				if(Property[ActiveProperty].Owner==ActivePlayer)
				{
					//Player Own, do nothing
					if(ButtonClicked==14)
					{
						NoticeBoardState=0;
					}
				}
				else
				{
					//Pay Rent
					if(ButtonClicked==14)
					{
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
						NoticeBoardState=0;
					}
				}
			}
			break;
		case 3:
			//Player landed on RailRoad
			if(Property[ActiveProperty].Owner==-1)
			{
				//RailRoad is NOT OWNED
				//accept/decline, update property values, player funds
				if(ButtonClicked==14)
				{
					//buy RailRoad purchase
					Property[ActiveProperty].Owner=ActivePlayer;
					Player[ActivePlayer].Money-=Property[ActiveProperty].BuyCost;
					NoticeBoardState=0;
				}
				if(ButtonClicked==15)
				{
					//decline RailRoad purchase
					NoticeBoardState=0;
				}
			}
			else
			{
				//RailRoad is OWNED
				int RR_Count=0;
				for(i=0;i<40;i++)
				{
					//Run though all property, just in case game files were modded, and railroads were changed
					//count railroads own by owner of current railroad
					if(Property[i].Owner==Property[ActiveProperty].Owner&&Property[i].Type==2)
					{
						RR_Count++;
					}
				}

				if(RR_Count>4)
					RR_Count=4;

				if(Property[ActiveProperty].Owner==ActivePlayer)
				{
					//Player own, do nothing
					if(ButtonClicked==14)
					{
						NoticeBoardState=0;
					}
				}
				else
				{
					if(ButtonClicked==14)
					{
						Player[Property[ActiveProperty].Owner].Money+=100;	//filler until later
						Player[ActivePlayer].Money-=100;
						NoticeBoardState=0;
					}
				}
			}
			break;
		case 4:
			//Player landed on Tax
			NoticeBoardState=0;
			break;
		case 5:
			//Player landed on Community Chest
			NoticeBoardState=0;
			break;
		case 6:
			//Player landed on Chance
			NoticeBoardState=0;
			break;
		default:
			NoticeBoardState=0;
			break;
		}
		break;
	case 1:
		//Build
		switch(ButtonClicked)
		{
		case 1:
			ActiveFrame=0;
			break;
		case 5:
			//Return to Trade Menu
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
		break;
	case 2:
		//Trade
		switch(ButtonClicked)
		{
		case 1:
			//Return to Main Board
			ActiveFrame=0;
		break;

		case 3:
			//Return to Build Menu
			CheckMonopolyStatus();
			ActiveFrame=1;
		break;

		case 200 ... 239:
			//handles building trade offer
			TradeOfferBuilder(ButtonClicked);
		break;
		}
		break;
	case 3:
		//Mortgage
		switch(ButtonClicked)
		{
		case 1:
			ActiveFrame=0;
			break;
		}
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

	//while Buffer Length is 10 minus 1 for /0 still limit loop for 1 Mil - 1
	//can go beyond limit with recursive loop

	//setting is toggle to add "$" to front text string
	//setting 0 - no addition to number string
	//setting 1 - addition of "$" to number string
	//setting 2 - addition of "Player " to number string

	//BaseNum=12345;	//debug override
	//setting=1;

	//char Buffer[10];	//dont have to append /0 at the end when zero entire buffer from start

	int i=0;	//index count
	int j=10;	//decimal index
	int DecimalCount=0;

	if(setting==0)
	{
		StringBuffer[0]='$';
		i++;
	}
	
	if(setting==2)
	{
		StringBuffer[0]='P';
		StringBuffer[1]='l';
		StringBuffer[2]='a';
		StringBuffer[3]='y';
		StringBuffer[4]='e';
		StringBuffer[5]='r';
		StringBuffer[6]=' ';

		i+=7;
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
	if(Property[ActiveProperty].Type<3)
	{
		NoticeBoardState=Property[ActiveProperty].Type+1;
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


	//Bug tracking:
	/*	when AI lands on Taxes, render frezzes
	 *  when AI lands on Chest/chance, render frezzes, maybe
	 *  	When AI lands on anything but normal property/corners render locks up, game keeps running
	 */

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
			AI_ButtonClicked=0;
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
		//ButtonClickedState=14;
		break;
	case 6:
		//Chance
		//ButtonClickedState=14;
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

void CheckMonopolyStatus(void)
{
	//ActivePlayer.MonopoliesOwned
	//MonopolyBlockLookUp[8][3]

	int i=0;
	int j=0;						//# of owned properties
	int l=0;
	int m=0;
	int k=0;
	int ActiveProperty=0;
	int BlockCount[8]={0};			//completion status of monopoly
	int BlockCountMax[8]={0};
	int PropertiesOwned[40]={0};	//0=GO, shouldn't be modable, isn't yet, but shold never be


	for(i=0;i<40;i++)
	{
		if(Property[i].Owner==ActivePlayer&&Property[i].Type==0)
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
		printf("%d / %d || ",BlockCount[i],BlockCountMax[i]);
		if(BlockCount[i]==BlockCountMax[i])
			Player[ActivePlayer].MonopoliesOwned[i]=1;
		else
			Player[ActivePlayer].MonopoliesOwned[i]=0;
	}
	printf("\n");

	//update build buttons visible (add / sub houses)
	for(i=0;i<40;i++)
	{
		if(Property[i].hasColor==1&&Property[i].Owner==ActivePlayer&&Property[i].Type==0)
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
		if(Player[ActivePlayer].MonopoliesOwned[i]==0)
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




}

