#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;



struct map{
char wall='#';
char point='.';
static const int row = 25;
static const int col = 25;
char num[row][col];
};

void generate_map(map &m){
     for (int i = 0; i < m.row; i++)
    {
        for (int j = 0; j < m.col; j++)
        {
            if (i == 0 || i == m.row - 1 || j == 0 || j == m.col - 1 ||
                (i == 3 && j >= 3 && j <= 8) ||
                (i == 3 && j >= 16 && j <= 21) ||
                (i == 4 && j >= 3 && j <= 8) ||
                (i == 4 && j >= 16 && j <= 21) ||
                (i >= 8 && i <= 16 && j == 4) ||
                (i == 12 && j >= 5 && j <= 20) ||
                (i >= 8 && i <= 16 && j == 20) ||
                (i >= 16 && i <= 21 && j == 8) ||
                (i == 21 && j >= 9 && j <= 15) ||
                (i == 8 && j >= 8 && j <= 16) ||
                (i >= 9 && i <= 13 && j == 12) ||
                (i == 20 && j >= 3 && j <= 6) ||
                (i == 21 && j >= 3 && j <= 6) ||
                (i == 20 && j >= 18 && j <= 21) ||
                (i == 21 && j >= 18 && j <= 21))
            {
                cout<<" "<<m.wall<<" ";
            }
            else
            {
                cout<<" "<<m.point<<" ";
            }
            
        }
        cout<<endl;
    }

};



int main(){
    map myMap;
    generate_map( myMap );
    
    return 0;
}