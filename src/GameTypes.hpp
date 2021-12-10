//============================================================================
// Name        : GameTypes.hpp
// Author      : MATT
// Version     :
// Copyright   :
// Description : All data classes/types used in program
//============================================================================

#ifndef GAMETYPES_HPP_
#define GAMETYPES_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string.h>
#include <math.h>

const int ButtonCount=110;
const int TextBoxCount=20;

class Tile
{
	public:
		/*char TileName[30];			//new tile attributes
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

		int RGB[3];
		int hasColor;
		int PosX;
		int PosY;

		sf::Text NameText;
		sf::Text CostText;
		sf::RectangleShape TileShape;
		sf::RectangleShape TileBlockColor;
		sf::RectangleShape TileHouse[4];
		sf::RectangleShape TileHotel;
		*/
		char TileName[30];		//legacy attributes
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

class Agent
{
	public:
		int ID;
		int Pos;
		int Money;
		int DB_roll;
		int DB_count;
		int inJail;
		int isBankrupt;
		int hasRolled;
		int hasAI;
		float MonopoliesOwned[8];	//dirty trick to track Completed Monopolies, sub 1 to get to LookUpTable array index

		int GetOutJail[2];	//0,1,2 since both chance/chest have one, may have more since chest/chance can be modify

		int PosX;
		int PosY;

		int VelX;
		int VelY;

		int Rot;

		int BuildDesireBlock[8];		//Property Block Build desire list
		int TradeDesireBlock[8];		//Property BLock Trade desire list

		sf::RectangleShape AgentSprite;
		sf::Texture AgentTexture;
		sf::Font AgentFont;
		sf::Text AgentText;
};

class Element
{
	public:
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

class TradeStruct
{
	public:
		int target_GetProp[30]={0};
		int target_GetFunds;
		int player_GetProp[30]={0};
		int player_GetFunds;
		int T_GP_Count;		//index # of props in array
		int P_GP_Count;
};

class GameCard
{
	public:
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

class Game
{

public:
		Tile Property[40];
		Tile PropertyDeed[40];
		Agent Player[4];
		TradeStruct Offer;

		Element Button[ButtonCount];
		Element TextBox[TextBoxCount];
		Element Line[2];
		Element Dice[2];
		Element PlayerCount;
		Element Funds[4];
		Element Cards[2];
		Element NoticeBoard;

		int NoticeBoardState;
};



sf::String IntToString(int,int);



#endif /* GAMETYPES_HPP_ */
