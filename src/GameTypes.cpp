//============================================================================
// Name        : GameTypes.hpp
// Author      : MATT
// Version     :
// Copyright   :
// Description : Just for int to string function
//============================================================================

#include "GameTypes.hpp"


sf::String IntToString(int BaseNum, int setting)		//this apparently works now?
{
	//Custom Header File

	char StringBuffer[20];

	//gMonopoly.TextBuffer;

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

