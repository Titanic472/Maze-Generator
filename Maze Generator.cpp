//Maze Generator by Stanislav Budzynskyi
//Hard maze type generation will be ready soon
#include <iostream>
#include <time.h>
#include <locale.h>

using namespace std;

int  ReloadAmount = 0;

void console_clear(){
    system("cls");
}

struct Vector2{
    int x, y;

    Vector2(int x=0, int y=0)
        : x(x), y(y)
    {
    }

    Vector2 operator+(const Vector2& a){
        return Vector2(a.x+x, a.y+y);
    }

    Vector2 operator+=(const Vector2& a){
        *this = Vector2(this->x+a.x, this->y+a.y);
        return *this;
    }
};

class Maze{
    private:
    int Size = 11, StuckAmount = 0;//max size shown correctly in terminal is 11, if you want to test different maze sizes remember that
    int MapSize = Size*10+3;
    int** Map;
    bool MapCreated = false;
    Vector2 LastMoves[2], PlayerPos;
    public:
    ~Maze() {
        for (int y = 0; y < MapSize; ++y) {
            delete[] Map[y];
        }
        delete[] Map;
    }
    Vector2 RandomDirection(bool AllowLeft, bool AllowRight, bool AllowUp, bool AllowDown){
        while(AllowLeft || AllowRight || AllowUp || AllowDown){
            int r = rand()%4;
            if(r == 0 && AllowLeft == 1)return {-1, 0};
            if(r == 1 && AllowRight == 1)return {1, 0};
            if(r == 2 && AllowUp == 1)return {0, -1};
            if(r == 3 && AllowDown == 1)return {0, 1};
        }
    }

    Vector2 Direction(char Dir){
        switch(Dir){
            case 'L':
                return {-1, 0};
            case 'R':
                return {1, 0};
            case 'U':
                return {0, -1};
            case 'D':
                return {0, 1};
            default:
                cout << "Bad Direction\n";
                return {0, 0};
        }
    }

    void SetSize(int S){
        MapCreated = false;
        Size = S;
        MapSize = Size*10+3;
    }

    void CreateEmpty(){
        if(MapCreated){
            for(int y = 0; y<MapSize;++y)for(int x = 0; x<MapSize;++x)Map[y][x] = 0;
        }
        else{
            Map = new int*[MapSize];
            for(int y = 0; y<MapSize;++y){
                Map[y] = new int[MapSize];
                for(int x = 0; x<MapSize;++x)Map[y][x] = 0;
            }
            MapCreated = true;
        }
    }

    void ShowMap(){
        for(int i = 0; i<MapSize;++i){
            for(int j = 0; j<MapSize;++j){
                //cout << i << " " << j << "\n";
                //cout << Map[i][j];
                if(i==PlayerPos.y && j == PlayerPos.x){
                    cout << "PL";
                    continue;
                }
                switch(Map[i][j]){
                    case 0:
                        cout << "██";
                        break;
                    case 1:
                        cout << "  ";
                        break;
                    case 2:
                        cout << "· ";
                        break;
                    case 3:
                        cout << "EX";
                        break;
                    default:
                        break;
                }
            }
            cout <<"\n";
        }
        cout << "\n\n\n";
    }

    void SetPoint(Vector2 Pos, int Value){
        Map[Pos.y][Pos.x] = Value;
    }

    void DeleteLine(Vector2 StartPos, Vector2 EndPos){
        int Max, Min;
        if(StartPos.x == EndPos.x){
            Max = max(EndPos.y, StartPos.y);
            Min = min(EndPos.y, StartPos.y);
            for(int i = Min; i<=Max; ++i)Map[i][StartPos.x] = 0;
        }
        else{
            Max = max(EndPos.x, StartPos.x);
            Min = min(EndPos.x, StartPos.x);
            for(int i = Min; i<=Max; ++i)Map[StartPos.y][i] = 0;
        }

    }

    bool IsStuck(Vector2 Pos){
        if(!CheckMoveAllowed('L', Pos) && !CheckMoveAllowed('R', Pos) && !CheckMoveAllowed('U', Pos) && !CheckMoveAllowed('D', Pos))return true;
        else return false;
    }

    bool CheckMoveAllowed(char Direction, Vector2 Pos, bool AllowYZero = true){
        if(!AllowYZero && Pos.y==0)return false;
        switch(Direction){
            case 'L':
                if(Pos.x == 1 || Map[Pos.y-1][Pos.x-1]==1 || Map[Pos.y+1][Pos.x-1]==1 || Map[Pos.y][Pos.x-2]==1)return false;
                else return true;
            case 'R':
                if(Pos.x == MapSize-2 || Map[Pos.y-1][Pos.x+1]==1 || Map[Pos.y+1][Pos.x+1]==1 || Map[Pos.y][Pos.x+2]==1)return false;
                else return true;
            case 'U':
                if(Pos.y !=1 && (Map[Pos.y-1][Pos.x-1]>=1 || Map[Pos.y-1][Pos.x+1]>=1 || Map[Pos.y-2][Pos.x]>=1))return false;
                else return true;
            case 'D':
                if(Pos.y == MapSize-2 || Map[Pos.y+1][Pos.x+1]==1 || Map[Pos.y+1][Pos.x-1]==1 || Map[Pos.y+2][Pos.x]==1)return false;
                else return true;
        }
    }
    void RestartBase(){
        StuckAmount = 0;
        ++ReloadAmount;
        GenerateBase();
    }

    void GenerateBase(){
        LastMoves[0] = {0, 0};
        LastMoves[1] = {0, 0};
        int StartPoint;
        CreateEmpty();
        Vector2 CurrPos = {rand()%(MapSize-5)+2, MapSize-1} , Modifiers = {0, -1};
        PlayerPos = CurrPos;
        StartPoint = CurrPos.x;
        SetPoint(CurrPos, 1);
        CurrPos +=Modifiers;
        SetPoint(CurrPos, 1);
        Modifiers = RandomDirection(1, 1, 1, 0);
        while(true){
            CurrPos+=Modifiers;
            SetPoint(CurrPos, 1);
            if(CurrPos.y == 0)break;
            if(IsStuck(CurrPos)){
                if(StuckAmount >=10 || LastMoves[1].x == 0 && LastMoves[1].y==0){
                    RestartBase();
                    return;
                }
                else{
                    DeleteLine(CurrPos, LastMoves[0]);
                    DeleteLine(LastMoves[0], LastMoves[1]);
                    CurrPos = LastMoves[1];
                    if(LastMoves[1].x - LastMoves[0].x >0){
                        if(CheckMoveAllowed('R', CurrPos))Modifiers = {1, 0};
                        else if(CheckMoveAllowed('U', CurrPos))Modifiers = {0, -1};
                        else if(CheckMoveAllowed('D', CurrPos))Modifiers = {0, 1};
                        else{
                            RestartBase();
                            return;
                        }
                    }
                    else if(LastMoves[1].x - LastMoves[0].x <0){
                        if(CheckMoveAllowed('L', CurrPos))Modifiers = {-1, 0};
                        else if(CheckMoveAllowed('U', CurrPos))Modifiers = {0, -1};
                        else if(CheckMoveAllowed('D', CurrPos))Modifiers = {0, 1};
                        else{
                            RestartBase();
                            return;
                        }
                    }
                    else if(LastMoves[1].y - LastMoves[0].y <0){
                        if(CheckMoveAllowed('L', CurrPos))Modifiers = {-1, 0};
                        else if(CheckMoveAllowed('R', CurrPos))Modifiers = {1, 0};
                        else if(CheckMoveAllowed('U', CurrPos))Modifiers = {0, -1};
                        else{
                            RestartBase();
                            return;
                        }
                    }
                    else if(LastMoves[1].y - LastMoves[0].y >0){
                        if(CheckMoveAllowed('L', CurrPos))Modifiers = {-1, 0};
                        else if(CheckMoveAllowed('R', CurrPos))Modifiers = {1, 0};
                        else if(CheckMoveAllowed('D', CurrPos))Modifiers = {0, 1};
                        else{
                            RestartBase();
                            return;
                        }
                    }
                    SetPoint(LastMoves[1], 1);
                    LastMoves[0] = {0, 0};
                    LastMoves[1] = {0, 0};
                    continue;
                }
            }
            Vector2 CheckPositions[3];
            if(CurrPos.x==1 && Modifiers.x==-1){
                if(Map[CurrPos.y-1][CurrPos.x] == 0 && Map[CurrPos.y-1][CurrPos.x+1] == 0)Modifiers = {0, -1};
                else if(Map[CurrPos.y][CurrPos.x+1] == 0 && Map[CurrPos.y-1][CurrPos.x+1] == 0 && Map[CurrPos.y+1][CurrPos.x+1] == 0)Modifiers = {1, 0};
                else{
                    RestartBase();
                    return;
                }
                continue;
            }
            else if(CurrPos.x==MapSize-2 && Modifiers.x==1){
                if(Map[CurrPos.y-1][CurrPos.x] == 0 && Map[CurrPos.y-1][CurrPos.x-1] == 0)Modifiers = {0, -1};
                else if(Map[CurrPos.y][CurrPos.x-1] == 0 && Map[CurrPos.y-1][CurrPos.x-1] == 0 && Map[CurrPos.y+1][CurrPos.x-1] == 0)Modifiers = {-1, 0};
                else{
                    RestartBase();
                    return;
                }
                continue;
            }
            else if(CurrPos.y==MapSize-2 && Modifiers.y==1){
                if(Map[CurrPos.y][CurrPos.x+1] == 0 && Map[CurrPos.y-1][CurrPos.x+1] == 0 && Map[CurrPos.y+1][CurrPos.x+1] == 0 && CurrPos.x>StartPoint)Modifiers = {1, 0};
                else if(Map[CurrPos.y][CurrPos.x-1] == 0 && Map[CurrPos.y-1][CurrPos.x-1] == 0 && Map[CurrPos.y+1][CurrPos.x-1] == 0 )Modifiers = {-1, 0};
                else if(Map[CurrPos.y-1][CurrPos.x] == 0 && Map[CurrPos.y-1][CurrPos.x+1] == 0 && Map[CurrPos.y-1][CurrPos.x-1] == 0 )Modifiers = {0, -1};
                else {
                    RestartBase();
                    return;
                }
                continue;
            }
            if(CurrPos.y+Modifiers.y*2<0)CheckPositions[0] = {CurrPos.x, 0};
            else CheckPositions[0] = {CurrPos.x+Modifiers.x*2, CurrPos.y+Modifiers.y*2};
            if(Modifiers.x==0){
                CheckPositions[1] = {CurrPos.x+1,CurrPos.y+Modifiers.y};
                CheckPositions[2] = {CurrPos.x-1,CurrPos.y+Modifiers.y};
            }
            else{
                CheckPositions[1] = {CurrPos.x+Modifiers.x,CurrPos.y+1};
                CheckPositions[2] = {CurrPos.x+Modifiers.x,CurrPos.y-1};
            }
            if(rand()%8==0 || Map[CheckPositions[0].y][CheckPositions[0].x]==1 || Map[CheckPositions[1].y][CheckPositions[1].x]==1 || Map[CheckPositions[2].y][CheckPositions[2].x]==1){
                bool AllowLeft = false, AllowRight = false, AllowUp = false, AllowDown = false;
                if(CurrPos.x>1 && Modifiers.x!=1 && Map[CurrPos.y+1][CurrPos.x-1] == 0 && Map[CurrPos.y-1][CurrPos.x-1] == 0 && Map[CurrPos.y][CurrPos.x-2] == 0) AllowLeft = true;
                if(CurrPos.x<MapSize-2 && Modifiers.x!=-1 && Map[CurrPos.y+1][CurrPos.x+1] == 0 && Map[CurrPos.y-1][CurrPos.x+1] == 0 && Map[CurrPos.y][CurrPos.x+2] == 0) AllowRight = true;
                if(CurrPos.y<MapSize-2 && Modifiers.y!=-1 && Map[CurrPos.y+1][CurrPos.x+1] == 0 && Map[CurrPos.y+1][CurrPos.x-1] == 0 && Map[CurrPos.y+2][CurrPos.x] == 0) AllowDown = true;
                if(CurrPos.y>1 && Modifiers.y!=1 && Map[CurrPos.y-1][CurrPos.x+1] == 0 && Map[CurrPos.y-1][CurrPos.x-1] == 0 && Map[CurrPos.y-2][CurrPos.x] == 0 || CurrPos.y==1) AllowUp = true;
                if(AllowLeft || AllowRight || AllowUp || AllowDown) {
                        Modifiers = RandomDirection(AllowLeft, AllowRight, AllowUp, AllowDown);
                        LastMoves[1] = LastMoves[0];
                        LastMoves[0] = CurrPos;
                }
            }
        }
        SetPoint(CurrPos, 3);
        //cout << "Created Base\n";
        //ShowMap();
    }

    int BadEasyGenerations = 0;
    void GenerateEasy(){
        Vector2 CurrPos, Modifiers = {0, -1};
        GenerateBase();
        CurrPos = {PlayerPos.x, PlayerPos.y-1};
        int Tries = 0;
        while(CurrPos.y!=1 || Modifiers.y!=-1){
            ++Tries;
            if(CurrPos.y==MapSize || Tries == MapSize*MapSize){
                ++BadEasyGenerations;
                GenerateEasy();
                return;
            }
            if((CurrPos.y!=MapSize-2&&Modifiers.y!=1) && Map[CurrPos.y+Modifiers.y][CurrPos.x+Modifiers.x]==1){
                if(rand()%4==0)GenerateBranch(CurrPos);
                CurrPos+=Modifiers;
            }
            else{
                Vector2 LastPos = CurrPos;
                if(CurrPos.x>1&& Modifiers.x==0 && Map[CurrPos.y][CurrPos.x-1]==1)Modifiers = Direction('L');
                else if(CurrPos.x<MapSize-2 && Modifiers.x==0 && Map[CurrPos.y][CurrPos.x+1]==1)Modifiers = Direction('R');
                else if(Modifiers.y==0 && Map[CurrPos.y-1][CurrPos.x]==1)Modifiers = Direction('U');
                else if(CurrPos.y<MapSize-2 && Modifiers.y==0 && Map[CurrPos.y+1][CurrPos.x]==1)Modifiers = Direction('D');
                CurrPos+=Modifiers;
                if(rand()%5==0)GenerateBranch(LastPos);
            }
            //cout << CurrPos.x << " " << CurrPos.y << " " << Modifiers.x << " " << Modifiers.y << "\n";
        }
    }

    void GenerateBranch(Vector2 Pos, char Dir = 'N'){
        //cout << "Generating Branch\n";
        if(Dir == 'N'){
            if(CheckMoveAllowed('L', Pos, false) && rand()%2 ==0)GenerateBranch(Pos, 'L');
            if(CheckMoveAllowed('R', Pos, false) && rand()%2 ==0)GenerateBranch(Pos, 'R');
            if(CheckMoveAllowed('U', Pos, false) && rand()%2 ==0)GenerateBranch(Pos, 'U');
            if(CheckMoveAllowed('D', Pos, false) && rand()%2 ==0)GenerateBranch(Pos, 'D');
            //ShowMap();
            return;
        }
        else{
            while(rand()%5!=0 && CheckMoveAllowed(Dir, Pos, false)){
                Pos+=Direction(Dir);
                if(Pos.y==0)return;
                SetPoint(Pos, 1);
            }
            if(CheckMoveAllowed('L', Pos, false) && (rand()%2 ==0 || rand()%2 ==0 && Dir == 'L'))GenerateBranch(Pos, 'L');
            if(CheckMoveAllowed('R', Pos, false) && (rand()%2 ==0 || rand()%2 ==0 && Dir == 'R'))GenerateBranch(Pos, 'R');
            if(CheckMoveAllowed('U', Pos, false) && (rand()%2 ==0 || rand()%2 ==0 && Dir == 'U'))GenerateBranch(Pos, 'U');
            if(CheckMoveAllowed('D', Pos, false) && (rand()%2 ==0 || rand()%2 ==0 && Dir == 'D'))GenerateBranch(Pos, 'D');
            return;
        }
    }

    bool CheckMove(char Dir){
        Dir = toupper(Dir);
        Vector2 MoveDir = Direction(Dir);
        if(PlayerPos.x == 0 && Dir == 'L')return false;
        if(PlayerPos.x == MapSize-1 && Dir == 'R')return false;
        if(PlayerPos.y == 0 && Dir == 'U')return false;
        if(PlayerPos.y == MapSize-1 && Dir == 'D')return false;
        if(Map[PlayerPos.y+MoveDir.y][PlayerPos.x+MoveDir.x]>0)return true;
        else return false;
    }

    void Move(char Dir){
        Dir = toupper(Dir);
        if(CheckMove(Dir)){
            PlayerPos+=Direction(Dir);
            if(Map[PlayerPos.y][PlayerPos.x]==3){
                console_clear();
                ShowMap();
                cout << "FINISH LINE REACHED!\n";
                while(true){}
            }
        }
    }
};

int main(){
    //ios_base::sync_with_stdio(0);
    //cin.tie(0);
    setlocale(LC_ALL, "pl_PL.UTF-8");
    srand(time(NULL));
    //system("color 0A");
    Maze Labirynt;
    Labirynt.GenerateEasy();
    char a;
    Labirynt.ShowMap();
    while(cin >> a){
            bool b = Labirynt.CheckMove(a);//moves: r - right, l - left, d - down, u - up;
            console_clear();
            cout << (int)b << "\n";
            Labirynt.Move(a);
            Labirynt.ShowMap();
    }
    return 0;
}
