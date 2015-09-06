/*
 * File: life.cpp
 * --------------
 * Your name and comments go here!
 */
 
#include "genlib.h"
#include "extgraph.h"
#include "simpio.h"
#include <iostream>
#include <fstream>
#include <string>
#include "grid.h"
#include "random.h"
#include "lifeGraphics.h"
#include "vector.h"

void PrintGrid(Grid<int> &mainGrid, int rows, int cols);
void Welcome();
void PopulateGrid(Grid<int> &colony, Grid<int> &colonySum , Vector<int> &liveListX , Vector<int> &liveListY ,  Vector<int> &changeListX , Vector<int> &changeListY ) ;
void PopulateRandom(Grid<int> &colony);
void PopulateFromFile(Grid<int> &colony , Grid<int> &colonySum, ifstream &popFile) ;
bool PointOnBoard(int numRows, int numCols , int xIndex, int yIndex) ;
void DrawUI(Grid<int> &colony) ;
void InitializeWindowSize();
void GenerateAndUpdate(Grid<int> &colony , Grid<int> &colonySum , Vector<int> &liveListX, Vector<int> &liveListY, Vector<int> &changeListX, Vector<int> &changeListY ) ;
void processCell( Grid<int> &colony , Grid<int> &colonySum , int xCord , int yCord , Vector<int> &nextChangeListX , Vector<int> &nextChangeListY , Vector<int> &nextLiveListX, Vector<int> &nextLiveListY , Grid<int> &nextColony, Grid<int> &nextColonySum , Grid<bool> &updatedOnce , Vector<int> &graphicClearListX , Vector<int> &graphicClearListY ) ;
void NeighbourUpdate(int xCord, int yCord , int amount , Vector<int> &nextChangeListX, Vector<int> &nextChangeListY , Grid<int> &nextColonySum , Grid<int> &colony , Grid<bool> &updatedOnce) ;
void LifeUIUpdater( Vector<int> &graphicClearListX, Vector<int> &graphicClearListY , Vector<int> &nextLiveListX , Vector<int> nextLiveListY , Grid<int> &nextColony ) ;
int main()
{
	const int defRows = 60 ;
	const int defCols = 60 ;
	SetWindowTitle("Life!");
	InitializeWindowSize();
	InitGraphics();
	Welcome();
	Grid<int> colony(defRows, defCols) ;
	Grid<int> colonySum(defRows, defCols) ;	
	Vector<int> changeListX ;
	Vector<int> changeListY ;
	Vector<int> liveListX ;
	Vector<int> liveListY ;
	PopulateGrid(colony, colonySum , liveListX , liveListY , changeListX , changeListY ) ;
	PrintGrid(colony , colony.numRows() , colony.numCols() ) ;
	cout<<endl ;
	PrintGrid(colonySum, colonySum.numRows() , colonySum.numCols() ) ;
	DrawUI(colony) ;
	GenerateAndUpdate(colony, colonySum , liveListX, liveListY , changeListX, changeListY ) ;



	return 0;
}


void PrintGrid(Grid<int> &mainGrid , int rows, int cols)
{
	for( int r=0 ; r< rows ; r++)
	{
		for(  int c=0 ; c< cols ; c++)
			cout<<mainGrid[r][c]<<" " ;
		cout<<endl ;
	}
}

void InitializeWindowSize()
{
	double width = GetFullScreenWidth() ;
	double height = GetFullScreenHeight() ;
	SetWindowSize(0.5*width , 0.5*height) ;
}
void Welcome()
{
	SetPointSize(48);
	MovePen(GetWindowWidth()/2-TextStringWidth("Welcome To Life!")/2,GetWindowHeight()/2);
	DrawTextString("Welcome To Life!");
	UpdateDisplay();
	cout << "Welcome to the game of Life, a simulation of the lifecycle of a bacteria colony." << endl;
	cout << "Cells live and die by the following rules:" << endl;
	cout << "\tA cell with 1 or fewer neighbors dies of loneliness" << endl;
	cout << "\tLocations with 2 neighbors remain stable" << endl;
	cout << "\tLocations with 3 neighbors will spontaneously create life" << endl;
	cout << "\tLocations with 4 or more neighbors die of overcrowding" << endl;
	cout << "In the animation, new cells are dark and lighten as they age." << endl;
	cout << "Hit RETURN when ready: ";
	GetLine();

	cout<<endl<<"You can start your colony with random cells or read from a prepared file."<<endl<<"Enter name of colony file (or RETURN to seed randomly):" ;
}

void PopulateGrid( Grid<int> &colony, Grid<int> &colonySum, Vector<int> &liveListX , Vector<int> &liveListY , Vector<int> &changeListX , Vector<int> &changeListY )
{
	ifstream popFile ;
	while(true)
	{
		string fileName ;
		fileName = GetLine() ;
		if (fileName.empty() )
		{
			PopulateRandom( colony ) ;
			break ;
		}
		else
		{
			popFile.open( fileName.c_str() ) ;
			if( popFile.is_open() )
			{
				PopulateFromFile( colony , colonySum, popFile ) ;
				break ;
			}
			cout<<endl<<"File not found. Try again: " ;
			popFile.clear() ;
		}
	}
	
	int numRows = colony.numRows() ;
	int numCols = colony.numCols() ;

	for(int r = 0 ; r < numRows ; r++)
	{
		for(int c = 0 ; c < numCols ; c++)
		{
			colonySum[r][c] = 0 ;
			if(colony[r][c]!=0)
			{
				liveListX.add(r) ;
				liveListY.add(c) ;
			}
			else
			{
				changeListX.add(r) ; // maintains blank cells that need consideration for prospective new cell growth, all are considered the first time, thats why being populated here.
				changeListY.add(c) ;
			}
			for(int i = -1 ; i <= 1 ; i++)
			{

				for(int j = -1 ; j <= 1 ; j++)
				{
					if( PointOnBoard( numRows, numCols , (r+i) , (c+j)) )
					{
						colonySum[r][c] += ( colony[r+i][c+j] != 0 ) ;
					}
					
				}
			}
			colonySum[r][c] = colonySum[r][c] - ( colony[r][c] !=0 ) ;
		}
	}

}

bool PointOnBoard(int numRows, int numCols , int xIndex, int yIndex)
{
	return ( ! ( (xIndex < 0 ) || (yIndex < 0) || (xIndex > (numRows -1 ) ) || (yIndex > ( numCols-1 ) ) ) ) ;
}
void PopulateRandom(Grid<int> &colony)
{
	Randomize() ;
	int rows = colony.numRows() ;
	int cols = colony.numCols() ;
	for(int r = 0 ; r < rows ; r++ )
		for(int c = 0 ; c < cols ; c++ )
		{
			int randNum = RandomInteger(1,100) ;
			if(randNum > 50)
			{
				colony[r][c] = RandomInteger(1,12) ;
			}
			else
			{
				colony[r][c] = 0 ;
			}
		}
}

void PopulateFromFile(Grid<int> &colony , Grid<int> &colonySum, ifstream &popFile ) 
{
	int rows , cols ;
	string temp ;
	while(true)
	{
		temp.clear() ;
		getline(popFile, temp) ;
		if(temp[0] != '#')
			break ;
	}
	//cout<<endl<<temp ;
	rows = StringToInteger( temp ) ;
	getline(popFile , temp ) ;
	cols = StringToInteger( temp ) ;
	colony.resize(rows, cols) ;
	colonySum.resize(rows,cols) ;
	for(int r = 0 ; r < rows ; r++ )
	{
		getline(popFile , temp) ;
		for(int c = 0 ; c < cols ; c++ )
		{
			colony[r][c] = ( temp[c] == 'X' ) ;
		}
	}
		
}

void DrawUI(Grid<int> &colony)
{
	int numRows = colony.numRows() ;
	int numCols = colony.numCols() ;
	InitLifeGraphics(numRows, numCols) ;
	for( int r = 0 ; r < numRows ; r++)
	{
		for( int c = 0 ; c < numCols ; c++)
		{
			DrawCellAt( r , c , colony[r][c]) ;
		}
	}
	UpdateDisplay() ;
}

void GenerateAndUpdate(Grid<int> &colony , Grid<int> &colonySum , Vector<int> &liveListX, Vector<int> &liveListY, Vector<int> &changeListX, Vector<int> &changeListY )
{
	int numCols = colony.numCols() ; 
	int numRows = colony.numRows() ;
	Grid<bool> updatedOnce(numRows, numCols) ; // Holds Info whether a blank cell has already been included in nextChangeList., more than double counting is avoided.
	for( int r = 0 ; r < numRows ; r++)
	{
		for(int c = 0 ; c < numCols ; c++)
		{
			updatedOnce[r][c] = 0 ;
		}
	}
		

	while(true)
	{
		int sizeChangeList = changeListX.size()  ;
		int sizeLiveList = liveListX.size() ;

		Grid<int> nextColony = colony ;
		Grid<int> nextColonySum = colonySum ;
		Vector<int> nextLiveListX ;
		Vector<int> nextLiveListY ;
		Vector<int> nextChangeListX ;
		Vector<int> nextChangeListY ;		 
		Vector<int> graphicClearListX ; // The graphics are also conditionally updated to save time , this list hols the cells that should be
		Vector<int> graphicClearListY ; // cleared off in the diagram, the nextLiveList does the other part of update to draw live cells with new age .
		
		// The changeList has blank cells that need to be considered for testing, the liveList has live cells only

	
		for (int cList = 0 ; cList < sizeChangeList ; cList++)
		{
			if( colony[ changeListX[cList] ][ changeListY[cList] ] != 0 )
			{
				//cout<<"Error !" <<endl ; // assertion checking
			}
			else if (colony[ changeListX[cList] ][ changeListY[cList] ] == 0 ) 
			{
				processCell( colony , colonySum , changeListX[cList] , changeListY[cList] , nextChangeListX , nextChangeListY , nextLiveListX, nextLiveListY , nextColony, nextColonySum , updatedOnce , graphicClearListX , graphicClearListY ) ;
			}
		}

		for(int lList = 0 ; lList < sizeLiveList  ; lList++ )
		{
			if( colony[ liveListX[lList] ][ liveListY[lList] ] == 0)
			{
				cout<<endl<<"Error !" <<endl ; // assertion checking
			}
			processCell( colony , colonySum , liveListX[lList] , liveListY[lList] , nextChangeListX , nextChangeListY , nextLiveListX, nextLiveListY , nextColony, nextColonySum , updatedOnce , graphicClearListX , graphicClearListY ) ;

		}

		GetLine() ;

		//PrintGrid( nextColony , nextColony.numRows(), nextColony.numCols() ) ;
		//cout<<endl ;
		//PrintGrid( nextColonySum , nextColonySum.numRows() , nextColonySum.numCols() ) ;
		//cout<< endl ;
		int sizeNextChangeList = nextChangeListX.size() ;
		for(int chngIter = 0 ; chngIter < sizeNextChangeList ; chngIter++)
		{
			//cout<< nextChangeListX[chngIter] <<" " << nextChangeListY[chngIter] << "\t";
			updatedOnce[ nextChangeListX[chngIter] ][ nextChangeListY[chngIter]  ] = false ; // making updatedOnce all false again, this is done as whole grid is not iterated this way,
										// Only disturbed elements are reset back to false .
		}

		LifeUIUpdater( graphicClearListX, graphicClearListY , nextLiveListX , nextLiveListY , nextColony ) ;

		//DrawUI(nextColony) ;
		
		
		liveListX = nextLiveListX ;
		liveListY = nextLiveListY ;
		changeListX = nextChangeListX ;
		changeListY = nextChangeListY ;
		colony = nextColony ;
		colonySum = nextColonySum ;


		//break ;
	}
}

void LifeUIUpdater( Vector<int> &graphicClearListX, Vector<int> &graphicClearListY , Vector<int> &nextLiveListX , Vector<int> nextLiveListY , Grid<int> &nextColony ) 
{
	// First clear the dead cells
	int sizeClearList = graphicClearListX.size() ;
	int sizeLiveList = nextLiveListX.size() ;
	int numRows = nextColony.numRows() ;
	int numCols = nextColony.numCols() ;
	for( int clearIter = 0 ; clearIter < sizeClearList ; clearIter++)
	{
		DrawCellAt( graphicClearListX[clearIter]  , graphicClearListY[clearIter] , nextColony[ graphicClearListX[clearIter] ][ graphicClearListY[clearIter] ]) ;
	}
	
	for( int drawIter = 0 ; drawIter < sizeLiveList ; drawIter++ )
	{
		if(nextColony[ nextLiveListX[drawIter] ][ nextLiveListY[drawIter] ] > 12)
		{
			//cout <<endl<<"Age > 12 found !"<<endl; // This message will keep popping in console , but anyways it is taken care of in next line
			nextColony[ nextLiveListX[drawIter] ][ nextLiveListY[drawIter] ] = 12 ;
		}
		DrawCellAt( nextLiveListX[drawIter]  , nextLiveListY[drawIter] , nextColony[ nextLiveListX[drawIter] ][ nextLiveListY[drawIter] ]) ;
	}

	UpdateDisplay() ;
	
}
void processCell( Grid<int> &colony , Grid<int> &colonySum , int xCord , int yCord , Vector<int> &nextChangeListX , Vector<int> &nextChangeListY , Vector<int> &nextLiveListX, Vector<int> &nextLiveListY , Grid<int> &nextColony, Grid<int> &nextColonySum , Grid<bool> &updatedOnce , Vector<int> &graphicClearListX , Vector<int> &graphicClearListY )
{
	
	switch( colonySum[xCord][yCord] )
	{
		case 0:
		case 1:
			if(colony[xCord][yCord] != 0) // Doomsday has come :(
			{
				nextColony[xCord][yCord] = 0 ;
				NeighbourUpdate( xCord, yCord , -1 , nextChangeListX, nextChangeListY , nextColonySum , colony , updatedOnce ) ;
				graphicClearListX.add(xCord) ; // remove from graphic window
				graphicClearListY.add(yCord) ;
				if( !updatedOnce[xCord][yCord])
				{
					nextChangeListX.add(xCord) ;
					nextChangeListY.add(yCord) ;
				}
			}
			else
			{
			
			}

			break ;
		case 2:									// Let there be life !
			if (colony[xCord][yCord] !=0 )
			{
				nextLiveListX.add(xCord) ;
				nextLiveListY.add(yCord) ;
				nextColony[xCord][yCord] += 1 ; // age increment , I will parse nextLiveList in the end to keep age <= 12 .
			}
			// if its blank it does not impact anybody , nextColonySum and nextColony are already same as colony and colonySum so dont need to copy that cell again.
			break ;
		case 3:								// Let there be life !
			if(colony[xCord][yCord] == 0) 
			{
				nextColony[xCord][yCord] += 1 ; // age increment, same comments as earlier
				NeighbourUpdate( xCord, yCord , 1 , nextChangeListX, nextChangeListY , nextColonySum , colony , updatedOnce ) ;
				nextLiveListX.add(xCord) ;
				nextLiveListY.add(yCord) ;
			}
			else
			{
				nextColony[xCord][yCord] += 1 ; // age increment , same comments as earlier
				nextLiveListX.add(xCord) ;
				nextLiveListY.add(yCord) ;
			}
			break ;
		case 4:                          // Doomsday again !
		case 5:
		case 6:
		case 7:
		case 8:
			if(colony[xCord][yCord] != 0) // Doomsday has come :(
			{
				nextColony[xCord][yCord] = 0 ;
				NeighbourUpdate( xCord, yCord , -1 , nextChangeListX, nextChangeListY , nextColonySum , colony , updatedOnce ) ;
				graphicClearListX.add(xCord) ; // remove from graphic window
				graphicClearListY.add(yCord) ;
				if( !updatedOnce[xCord][yCord])
				{
					nextChangeListX.add(xCord) ;
					nextChangeListY.add(yCord) ;
				}
			}
			else
			{
			
			}
			break ;
		default:
			cout<<endl<<colonySum[xCord][yCord]<<endl ;
			cout<<endl<<"Error !"<<endl ;
	}
}

void NeighbourUpdate(int xCord, int yCord , int amount , Vector<int> &nextChangeListX, Vector<int> &nextChangeListY , Grid<int> &nextColonySum , Grid<int> &colony , Grid<bool> &updatedOnce)
{
	int numRows = colony.numRows() ;
	int numCols = colony.numCols() ;
	for(int i = -1 ; i <= 1 ; i++ )
	{
		for(int j = -1 ; j<= 1 ; j++ )
		{
			if( PointOnBoard( numRows, numCols , (xCord+i) , (yCord+j)) )
			{
				nextColonySum[xCord+i][yCord+j] += amount ; // We will  subtract the amount added now from 0,0 cell later to balance things
				if( colony[xCord+i][yCord+j] == 0 &&  !updatedOnce[xCord+i][yCord+j] )
				{
					nextChangeListX.add(xCord+i) ;
					nextChangeListY.add(yCord+j) ;
					updatedOnce[xCord+i][yCord+j] = true ; // So that every blank cell is just iterated once because neigbour update 
															// can try to add same cell for update many times over.
				}
			}
		}
	}

	nextColonySum[xCord][yCord] -= amount ; // the subtraction of earlier addtiion to 0,0 needs to be done here.
	

} 