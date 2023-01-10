#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//#include "walk.h" //Remember to create a new .h and .c file!!!!!!!!!!!

/*--------------- global variable ---------------*/
    const int ROW = 9;
    const int COLUMN = 9;
    const int CHESS_NUMBER = 40;
    const int MOVEFUNC_NUMBER = 10;

/*----------------  struct define  -------------------*/
struct chess{
    bool color;//0->red 1->blue
    int type;
    bool upgrade;//1->upgrade
};
typedef struct chess Chess;

struct chessboard{
    int array[9][9];
};
typedef struct chessboard Chessboard;

struct Node{
    Chessboard data;
    struct Node* next;
};
typedef struct Node NODE;

void initialize_chessboard(Chessboard *initial);
void print_chess(Chessboard chessboard,Chess all_chess[]);
bool legal_move(Chess moving_chess,int before_column,int before_row,int after_column,int after_row);
void Chesses(Chess* array);
bool movement00(bool,int,int,int,int);//步兵走法
bool movement01(bool,int,int,int,int);//角行走法
bool movement02(bool,int,int,int,int);//飛車走法
bool movement03(bool,int,int,int,int);//香車走法
bool movement04(bool,int,int,int,int);//桂馬走法
bool movement05(bool,int,int,int,int);//銀將走法
bool movement06(bool,int,int,int,int);//金將走法(不升級)
bool movement07(bool,int,int,int,int);//升級角行走法
bool movement08(bool,int,int,int,int);//升級飛車走法
bool movement09(bool,int,int,int,int);//王將走法(不升級)
bool legal_target(Chess target_chess,int target_column,int target_row,Chess moving_chess);
bool legal_color(Chess moving_chess,int current_color);
bool eat_enemy(Chess target_chess,Chess moving_chess);
void keep_eaten_chess(int *eaten_chess_b,int *eaten_chess_r,Chess *target_chess,int chess_box[][40],int target_chess_num);
void New_chessboard(int moving_chess_num,int current_column,int current_row,int target_column,int target_row,Chessboard *current_chessboard);
bool Upgrade_or_not(int target_column,Chess moving_chess);
void print_chessbox(int chess_box[][40],int eaten_chess_b,int eaten_chess_r);

Chessboard Pop_stack(NODE **Stack);
void Push_into_stack(NODE **Stack,Chessboard current_chessboard);
bool Empty_queue(NODE** front);
void Load_last_game(Chessboard keep_all_chessboard[],int *cnt);
void Insert_into_queue(Chessboard current_chessboard,NODE **rear,NODE **front);
void output_game_to_file(NODE **front);

void return_eaten(Chessboard previous_chessboard,Chessboard current_chessboard,Chess all_chess[],int chess_box[][40],int* eaten_chess_b,int* eaten_chess_r);//if a chess was eaten, change it's color back, and take it out of chessbox
void return_upgraded(Chessboard previous_chessboard,Chessboard current_chessboard,Chess all_chess[]);//if a chess was upgraded, return back to unupgraded
void return_from_chessbox(Chessboard previous_chessboard,Chessboard current_chessboard,int chess_box[][40]);//if a chess was placed from chessbox, put it back

//bool legal_road(int current_chessboard,int target_column,int target_row);

/*-------------------  main  -----------------------*/
int main()
{
    Chessboard current_chessboard,previous_chessboard; //chess board, every space keeps a number according to the current chess on the board (-1 if its empty)
    int current_row,current_column,target_row,target_column;//keeps the row and column that the user inputs
    Chess all_chess[40]; //an array that keeps the 40 chesses
    bool correct_color,correct_target,correct_move;
    //bool no_chess_on_the_way;
    int current_color=0;
    int chess_box[2][40];//keep the eaten chesses, [0] for eaten red(blue),[1] for eaten blue(red)
    int eaten_chess_b=0,eaten_chess_r=0;//total chess eaten
    Chess moving_chess,target_chess;
    bool upgrade;
    bool from_chessbox;
    int picked_chess;
    int moving_chess_num ,target_chess_num;
    bool load_file;
    int array_size=0;
    Chessboard keep_all_chessboard [400];
    NODE *Stack=NULL;//initialize Stack
    NODE *front = NULL;
    NODE *rear = NULL;

    bool next;

    bool (*movefun_ptr_arr[MOVEFUNC_NUMBER])(bool,int,int,int,int);
    movefun_ptr_arr[0] = movement00;
    movefun_ptr_arr[1] = movement01;
    movefun_ptr_arr[2] = movement02;
    movefun_ptr_arr[3] = movement03;
    movefun_ptr_arr[4] = movement04;
    movefun_ptr_arr[5] = movement05;
    movefun_ptr_arr[6] = movement06;
    movefun_ptr_arr[7] = movement07;
    movefun_ptr_arr[8] = movement08;
    movefun_ptr_arr[9] = movement09;

    printf("Would you like load your last game? 1 for yes, 0 for no: ");
    scanf("%d",&load_file);

    if(load_file)
    {
        int chessboard_cnt=0;
        Load_last_game(keep_all_chessboard,&array_size);//keep all the chessboards in keep_all_chessboard

        //重播棋局，(含輸出格式)，輸入1可以看下一步，輸入0可以回上一步
        print_chess(keep_all_chessboard[chessboard_cnt],all_chess);
        while(1)
        {
            printf("Watch the next step (1 for next, 0 for previous): ");
            scanf("%d",&next);
            if(next)
            {
                print_chess(keep_all_chessboard[++chessboard_cnt],all_chess);
            }
            else
            {
                print_chess(keep_all_chessboard[--chessboard_cnt],all_chess);
            }
            if(chessboard_cnt==array_size-1)
            {
                break;
            }
        }
        return 0;
    }

    for(int i=0;i<40;i++)
    {
        chess_box[0][i]=-1;
        chess_box[1][i]=-1;
    }
    initialize_chessboard(&current_chessboard);
    Push_into_stack(&Stack,current_chessboard);
    Chesses(all_chess) ;
    print_chess(current_chessboard,all_chess);

    /////////////////////////////////////////////////////////////
    while(1)
    {
        from_chessbox=0;
        if((eaten_chess_b>0 && current_color==0)|| (eaten_chess_r>0 && current_color==1))
        {
            printf("\nWould you like to pick a chess from your chess box? 1 for yes, 0 for no: ");
            scanf("%d",&from_chessbox);
        }
        if(from_chessbox)//select a chess from chessbox
        {
            print_chessbox(chess_box,eaten_chess_b,eaten_chess_r);
            printf("\nPlease enter the chess you wanna pick: ");
            scanf("%d",&picked_chess);
            if(current_color==0)//blue
            {
                moving_chess_num = chess_box[0][picked_chess];
                while(moving_chess_num==-1)
                {
                    printf("There are no chesses in this block.");
                    printf("\nPlease enter the chess you wanna pick: ");
                    scanf("%d",&picked_chess);
                    moving_chess_num = chess_box[0][picked_chess];
                }
                chess_box[0][picked_chess]=-1;
                eaten_chess_b=eaten_chess_b-1;
            }
            else
            {
                moving_chess_num = chess_box[1][picked_chess];
                while(moving_chess_num==-1)
                {
                    printf("There are no chesses in this block.");
                    printf("\nPlease enter the chess you wanna pick: ");
                    scanf("%d",&picked_chess);
                    moving_chess_num = chess_box[1][picked_chess];
                }
                chess_box[1][picked_chess] = -1;
                eaten_chess_r=eaten_chess_r-1;
            }
        }
        else
        {
            printf("\nPlease enter the chess you wanna move, input 100 to go back to the previous play: \n");
            printf("Row: ");
            scanf("%d",&current_row);
            int previous_cnt=0;
            while(current_row==100)
            {
                if(Stack==NULL)
                {
                    printf("There's no previous play!");
                }
                else
                {
                    if(previous_cnt==0)
                    {printf("tmpp \n\n\n\n\n");
                        current_chessboard = Pop_stack(&Stack);//go back to the previous chess board
                    }
                    previous_chessboard = current_chessboard;
                    current_chessboard = Pop_stack(&Stack);

                    return_eaten(previous_chessboard,current_chessboard,all_chess,chess_box,&eaten_chess_b,&eaten_chess_r);//if a chess was eaten, change it's color back, and take it out of chessbox
                    return_upgraded(previous_chessboard,current_chessboard,all_chess);//if a chess was upgraded, return back to unupgraded
                    return_from_chessbox(previous_chessboard,current_chessboard,chess_box);//if a chess was placed from chessbox, put it back

                    print_chess(current_chessboard,all_chess);
                    previous_cnt++;printf("cnt:%d\n",previous_cnt);
                }

                /*
                    WARNING!!!
                */

                printf("\nPlease enter the chess you wanna move, input 100 to go back to the previous play: \n");
                printf("Row: ");
                scanf("%d",&current_row);
            }
            printf("Column: ");
            scanf("%d",&current_column);

            current_row=9-current_row;
            current_column=current_column-1;
            moving_chess_num = current_chessboard.array[current_row][current_column];

        }
        Chess moving_chess = all_chess[moving_chess_num];//moving_chess is just a copy, the value in it won't be changed
        printf("Chess %d\n",moving_chess_num);
        printf("\nPlease enter the destination: \n");
        printf("Row: ");
        scanf("%d",&target_row);
        printf("Column: ");
        scanf("%d",&target_column);
        target_row=9-target_row;
        target_column=target_column-1;

        target_chess_num = current_chessboard.array[target_row][target_column];
        Chess target_chess = all_chess[target_chess_num];

        //printf("type: %d \n color: %d \n upgrade: %d\n",moving_chess.type,moving_chess.color,moving_chess.upgrade);
        if(from_chessbox)
        {
            correct_move=1;
            while(1)
            {
                if(target_chess_num==-1)
                {
                    correct_target = 1;
                    break;
                }
                else
                {
                    printf("\nPlease enter the destination: \n");
                    printf("Row: ");
                    scanf("%d",&target_row);
                    printf("Column: ");
                    scanf("%d",&target_column);
                    target_row=9-target_row;
                    target_column=target_column-1;
                    target_chess_num = current_chessboard.array[target_row][target_column];
                }
            }
        }
        else
        {
            correct_move = legal_move(moving_chess,current_column,current_row,target_column,target_row);//printf("legal move: %d\n",correct_move);
            correct_target = legal_target(target_chess,target_column,target_row,moving_chess);//printf("legal target: %d\n",correct_target);
        }

        correct_color = legal_color(moving_chess,current_color);//printf("legal color: %d\n",correct_color);

        //no_chess_on_the_way = legal_road(current_chessboard,target_column,target_row);
        // if the moving direction, moving color and target color is correct
        if(correct_move && correct_color && correct_target)// && no_chess_on_the_way)
        {
            if(target_chess_num!=-1)
            {
                if(eat_enemy(target_chess,moving_chess))// check if the target position has an enemy
                {
                    if(target_chess_num ==38) //blue king was eaten red wins
                    {
                        New_chessboard(moving_chess_num,current_column,current_row,target_column,target_row,&current_chessboard);
                        Push_into_stack(&Stack,current_chessboard);
                        Insert_into_queue(current_chessboard,&rear,&front);printf("finish queue\n\n\n\n");
                        output_game_to_file(&front);
                        printf("\n\n\n***************** Game end, you lose! *****************");
                        return 0;
                    }
                    else if(target_chess_num ==39) //red king was eaten blue wins
                    {
                        New_chessboard(moving_chess_num,current_column,current_row,target_column,target_row,&current_chessboard);
                        Push_into_stack(&Stack,current_chessboard);
                        Insert_into_queue(current_chessboard,&rear,&front);
                        output_game_to_file(&front);
                        printf("\n\n\n***************** Game end, you win! *****************");
                        return 0;
                    }
                    else
                    {//printf("test %d %d\n",eaten_chess_b,eaten_chess_r);
                        keep_eaten_chess(&eaten_chess_b,&eaten_chess_r,&all_chess[target_chess_num],chess_box,target_chess_num);
                    }
                }
            }

            //judge if the chess can be upgraded
            if(Upgrade_or_not(target_column,moving_chess)&& !from_chessbox)
            {
                printf("Do you wanna upgrade your chess? 1 for yes and 0 for no: ");
                scanf("%d",&upgrade);
                if(upgrade)
                {
                    all_chess[moving_chess_num].upgrade = 1;
                    //printf("moving_chess.upgrade: %d\n",all_chess[moving_chess_num].upgrade);
                }
            }
            New_chessboard(moving_chess_num,current_column,current_row,target_column,target_row,&current_chessboard);
            //renew the current chess board

        }
        else
        {
            if(moving_chess_num==-1)
            {
                printf("THERE ARE NO CHESSES HERE!");
            }
            else if(correct_color==0)
            {
                printf("IT'S NOT YOUR TURN YET!");
            }
            else if(correct_move==0)
            {
                printf("YOU CANNOT WALK THIS WAY!");
            }
            else if(correct_target==0)
            {
                printf("YOU CANNOT PLACE YOUR CHESS HERE!");
            }
            continue;
        }
        print_chess(current_chessboard,all_chess);
        print_chessbox(chess_box,eaten_chess_b,eaten_chess_r);
        current_color = (++current_color)%2; //changing the current color

        Push_into_stack(&Stack,current_chessboard);

        Insert_into_queue(current_chessboard,&rear,&front);

    }
}
void return_eaten(Chessboard previous_chessboard,Chessboard current_chessboard,Chess all_chess[],int chess_box[][40],int *eaten_chess_b,int *eaten_chess_r)
//if a chess was eaten, change it's color back, and take it out of chessbox
{
    int p_cnt=0,c_cnt=0;
    int eaten_chess;
    for(int i=0;i<COLUMN;i++)
    {
        for(int j=0;j<ROW;j++)
        {
            p_cnt += previous_chessboard.array[j][i];//total number in previous chess_board
            c_cnt += current_chessboard.array[j][i];//total number in current chess_board
        }

    }printf("p_cnt: %d\n",p_cnt);printf("c_cnt: %d\n",c_cnt);

    eaten_chess = c_cnt - p_cnt -1;//current is the chessboard that was popped out
    printf("eaten_chess: %d\n",eaten_chess);
    if(eaten_chess==-1)
        return 1;
    for(int j=0;j<40;j++) //take chess out of chessbox
    {
        if(chess_box[all_chess[eaten_chess].color][j] == eaten_chess)//find the space that keeps the eaten chess
        {
            chess_box[all_chess[eaten_chess].color][j] = -1;
            break;
        }
    }
    if(all_chess[eaten_chess].color==0)
        --*eaten_chess_b;
    else
        --*eaten_chess_r;
    all_chess[eaten_chess].color = (all_chess[eaten_chess].color+1)%2;//change color back
}
void return_upgraded(Chessboard previous_chessboard,Chessboard current_chessboard,Chess all_chess[])
//if a chess was upgraded, return back to unupgraded
{
    return 1;
}
void return_from_chessbox(Chessboard previous_chessboard,Chessboard current_chessboard,int chess_box[][40])
//if a chess was placed from chessbox, put it back
{
    return 1;
}
/*bool legal_road(int current_chessboard,int target_column,int target_row)
{
    if()
}*/
void Load_last_game(Chessboard keep_all_chessboard[],int *cnt)
{
    //重播棋局，(含輸出格式)，輸入1可以看下一步，輸入0可以回上一步
    FILE * fp;
    fp = fopen ("Previous game.txt", "r");
    Chessboard input_chessboard;
    int blank_line;
    int keep_chess;
    while(fp)
    {
        for(int i=0;i<COLUMN;i++)
        {
            for(int j=0;j<ROW;j++)
            {
                fscanf(fp,"%d",&keep_chess);
                input_chessboard.array[j][i]=keep_chess;
            }
        }
        if(feof(fp))
            break;
        keep_all_chessboard[*cnt] = input_chessboard ;
        ++(*cnt);
    }
    fclose(fp);
}

void output_game_to_file(NODE **front)
{
    FILE *fptr;
    fptr = fopen("Previous game.txt","w");

    while((*front)!=NULL) //queue isn't empty,there are still plays to load
    {
        for(int j=0;j<COLUMN;j++)
        {
            for(int i=0;i<ROW;i++)
            {
                fprintf(fptr,"%d ",(*front)->data.array[i][j]);
            }
            fprintf(fptr,"\n");
        }
        fprintf(fptr,"\n");
        (*front)=(*front)->next;
    }
    //程式結束前閉檔
    fclose(fptr);
}
bool Empty_queue(NODE **front) //used in function - output_game_to_file()
{
    if((*front) == NULL)
        return true;
    else
        return false;
}
void Insert_into_queue(Chessboard current_chessboard,NODE **rear,NODE **front)
{
    NODE *tmp;
    tmp = (NODE*)malloc(sizeof(NODE));
    tmp->data = current_chessboard;
    tmp->next = NULL;
    if((*rear)==NULL)
    {
        (*front) = tmp;
    }
    else
    {
        (*rear)->next = tmp;
    }
    (*rear) = tmp;
}
Chessboard Pop_stack(NODE **Stack)
{
    NODE *tmp;
    tmp = (*Stack);
    (*Stack) = (*Stack)->next;if((*Stack)==NULL)printf("NULLLL \n\n\n\n");
    return (tmp->data);
}
void Push_into_stack(NODE **Stack,Chessboard current_chessboard)
{
    NODE *tmp;
    tmp = (NODE*)malloc(sizeof(NODE));
    tmp->data = current_chessboard;
    tmp->next = (*Stack); //head of Stack
    (*Stack) = tmp;
}

void print_chessbox(int chess_box[][40],int eaten_chess_b,int eaten_chess_r)
{
    printf("Blue chess box:");
    for(int i=0;i<eaten_chess_b;i++)
    {
        printf("%d ",chess_box[0][i]);
    }
    printf("\n");
    printf("Red chess box:");
    for(int i=0;i<eaten_chess_r;i++)
    {
        printf("%d ",chess_box[1][i]);
    }
    printf("\n");
}
bool Upgrade_or_not(int target_column,Chess moving_chess)
{
    if(moving_chess.color==0)//blue
    {
        if(target_column<3 && moving_chess.upgrade==0)
        {
            return true;
        }
        else
            return false;
    }
    else//red
    {
        if(target_column>5 && moving_chess.upgrade==0)
        {
            return true;
        }
        else
            return false;
    }

}
bool eat_enemy(Chess target_chess,Chess moving_chess)
{
    if(target_chess.color!=moving_chess.color )
        return true;
    else
        return false;
}
void New_chessboard(int moving_chess_num,int current_column,int current_row,int target_column,int target_row,Chessboard *current_chessboard)
{
    current_chessboard->array[current_row][current_column] = -1;
    current_chessboard->array[target_row][target_column] = moving_chess_num;
}
void keep_eaten_chess(int *eaten_chess_b,int *eaten_chess_r,Chess *target_chess,int chess_box[][40],int target_chess_num)
{
    target_chess->upgrade=0;
    if(target_chess->color)//red was eaten
    {
        target_chess->color = 0;  //blue to red
        chess_box[0][*eaten_chess_b] = target_chess_num ;
        *eaten_chess_b=*eaten_chess_b+1;
        printf("eaten_chess_b: %d\n",*eaten_chess_b);

    }
    else//blue was eaten
    {
        target_chess->color = 1;
        chess_box[1][*eaten_chess_r] = target_chess_num ;
        *eaten_chess_r=*eaten_chess_r+1;
        printf("eaten_chess_r: %d\n",*eaten_chess_r);

    }
}
bool legal_target(Chess target_chess,int target_column,int target_row,Chess moving_chess)
{
    bool moving_color = moving_chess.color;
    bool t_color = target_chess.color;
    bool m_color = moving_chess.color;

    if(target_chess.upgrade>1 || target_chess.type>7 || target_chess.color>1 )
    {
        return true;
    }

    if(m_color==t_color)
        return false;
    else
        return true;
}

void initialize_chessboard(Chessboard *initial){
    for(int i=0;i<ROW ;i++)
    {
        for(int j=0;j<COLUMN;j++)
        {
            initial->array[i][j] = -1;//沒有棋子
        }
    }

    for(int i=0; i<ROW ; i++){
        initial->array[i][6] = 8-i;
        initial->array[i][2] = 17-i;
    }
    initial->array[1][7] = 18;
    initial->array[7][1] = 19;
    initial->array[7][7] = 20;
    initial->array[1][1] = 21;
    initial->array[0][8] = 22;
    initial->array[8][8] = 23;
    initial->array[0][0] = 24;
    initial->array[8][0] = 25;
    initial->array[1][8] = 26;
    initial->array[7][8] = 27;
    initial->array[1][0] = 28;
    initial->array[7][0] = 29;
    initial->array[2][8] = 30;
    initial->array[6][8] = 31;
    initial->array[2][0] = 32;
    initial->array[6][0] = 33;
    initial->array[3][8] = 34;
    initial->array[5][8] = 35;
    initial->array[3][0] = 36;
    initial->array[5][0] = 37;
    initial->array[4][8] = 38;
    initial->array[4][0] = 39;

    /*for(int cnt = 0; cnt<9;cnt++)
    {
        for(int c=0;c<9;c++)
        {
            printf("%d ",initial[c][cnt]);
        }
        printf("\n");
    }printf("\n\n");*/
}
//Change the number into each chinese character
/*string chess_symbol_and_color(Chess all_chess[],int current_chess)
{
    switch(all_chess[current_chess].)
    {

    }
}*/
void print_chess(Chessboard chessboard,Chess all_chess[]) //print chess board
{
    //string keep_chess;
    printf("09 08 07 06 05 04 03 02 01 \n");
    for(int C_cnt=0; C_cnt<COLUMN;C_cnt++)
    {
        for(int R_cnt=0; R_cnt<ROW; R_cnt++)
        {
            /*keep_chess = chess_symbol_and_color(all_chess[],chessboard.array[R_cnt][C_cnt]);
            if(all_chess[chessboard.array[R_cnt][C_cnt]].color ==0)//blue
            {
                //print blue string
            }
            else //red
            {
                //print red string
            }*/
            printf("%d ",chessboard.array[R_cnt][C_cnt]);
        }
        switch (C_cnt){
            case 0: {printf("一\n");break;}
            case 1: {printf("二\n");break;}
            case 2: {printf("三\n");break;}
            case 3: {printf("四\n");break;}
            case 4: {printf("五\n");break;}
            case 5: {printf("六\n");break;}
            case 6: {printf("七\n");break;}
            case 7: {printf("八\n");break;}
            case 8: {printf("九\n");break;}
            default: {printf(" ");break;}
        }
    }
}

void Chesses(Chess* array) //Information of all chesses
{
    //步兵
    //0
    (*(array)).color = 0;
    (*(array)).type = 0;
    (*(array)).upgrade = 0;

    //1
    (*(array+1)).color = 0;
    (*(array+1)).type = 0;
    (*(array+1)).upgrade = 0;

    //2
    (*(array+2)).color = 0;
    (*(array+2)).type = 0;
    (*(array+2)).upgrade = 0;

    //3
    (*(array+3)).color = 0;
    (*(array+3)).type = 0;
    (*(array+3)).upgrade = 0;

    //4
    (*(array+4)).color = 0;
    (*(array+4)).type = 0;
    (*(array+4)).upgrade = 0;

    //5
    (*(array+5)).color = 0;
    (*(array+5)).type = 0;
    (*(array+5)).upgrade = 0;

    //6
    (*(array+6)).color = 0;
    (*(array+6)).type = 0;
    (*(array+6)).upgrade = 0;

    //7
    (*(array+7)).color = 0;
    (*(array+7)).type = 0;
    (*(array+7)).upgrade = 0;

    //8
    (*(array+8)).color = 0;
    (*(array+8)).type = 0;
    (*(array+8)).upgrade = 0;

    //9
    (*(array+9)).color = 1;
    (*(array+9)).type = 0;
    (*(array+9)).upgrade = 0;

    //10
    (*(array+10)).color = 1;
    (*(array+10)).type = 0;
    (*(array+10)).upgrade = 0;

    //11
    (*(array+11)).color = 1;
    (*(array+11)).type = 0;
    (*(array+11)).upgrade = 0;

    //12
    (*(array+12)).color = 1;
    (*(array+12)).type = 0;
    (*(array+12)).upgrade = 0;

    //13
    (*(array+13)).color = 1;
    (*(array+13)).type = 0;
    (*(array+13)).upgrade = 0;

    //14
    (*(array+14)).color = 1;
    (*(array+14)).type = 0;
    (*(array+14)).upgrade = 0;

    //15
    (*(array+15)).color = 1;
    (*(array+15)).type = 0;
    (*(array+15)).upgrade = 0;

    //16
    (*(array+16)).color = 1;
    (*(array+16)).type = 0;
    (*(array+16)).upgrade = 0;

    //17
    (*(array+17)).color = 1;
    (*(array+17)).type = 0;
    (*(array+17)).upgrade = 0;

    //18 藍角行
    (*(array+18)).color = 0;
    (*(array+18)).type = 1;
    (*(array+18)).upgrade = 0;

    //19 紅角行
    (*(array+19)).color = 1;
    (*(array+19)).type = 1;
    (*(array+19)).upgrade = 0;

    //20 藍飛車
    (*(array+20)).color = 0;
    (*(array+20)).type = 2;
    (*(array+20)).upgrade = 0;

    //21 紅飛車
    (*(array+21)).color = 1;
    (*(array+21)).type = 2;
    (*(array+21)).upgrade = 0;

    //22 藍香車
    (*(array+22)).color = 0;
    (*(array+22)).type = 3;
    (*(array+22)).upgrade = 0;

    //23 藍香車
    (*(array+23)).color = 0;
    (*(array+23)).type = 3;
    (*(array+23)).upgrade = 0;

    //24 紅香車
    (*(array+24)).color = 1;
    (*(array+24)).type = 3;
    (*(array+24)).upgrade = 0;

    //25 紅香車
    (*(array+25)).color = 1;
    (*(array+25)).type = 3;
    (*(array+25)).upgrade = 0;

    //26 藍桂馬
    (*(array+26)).color = 0;
    (*(array+26)).type = 4;
    (*(array+26)).upgrade = 0;

    //27 藍桂馬
    (*(array+27)).color = 0;
    (*(array+27)).type = 4;
    (*(array+27)).upgrade = 0;

    //28 紅桂馬
    (*(array+28)).color = 1;
    (*(array+28)).type = 4;
    (*(array+28)).upgrade = 0;

    //29 紅桂馬
    (*(array+29)).color = 1;
    (*(array+29)).type = 4;
    (*(array+29)).upgrade = 0;

    //30 藍銀將
    (*(array+30)).color = 0;
    (*(array+30)).type = 5;
    (*(array+30)).upgrade = 0;

    //31 藍銀將
    (*(array+31)).color = 0;
    (*(array+31)).type = 5;
    (*(array+31)).upgrade = 0;

    //32 紅銀將
    (*(array+32)).color = 1;
    (*(array+32)).type = 5;
    (*(array+32)).upgrade = 0;

    //33 紅銀將
    (*(array+33)).color = 1;
    (*(array+33)).type = 5;
    (*(array+33)).upgrade = 0;

    //34 藍金將
    (*(array+34)).color = 0;
    (*(array+34)).type = 6;
    (*(array+34)).upgrade = 0;

    //35 藍金將
    (*(array+35)).color = 0;
    (*(array+35)).type = 6;
    (*(array+35)).upgrade = 0;

    //36 紅金將
    (*(array+36)).color = 1;
    (*(array+36)).type = 6;
    (*(array+36)).upgrade = 0;

    //37 紅金將
    (*(array+37)).color = 1;
    (*(array+37)).type = 6;
    (*(array+37)).upgrade = 0;

    //38 藍王將
    (*(array+38)).color = 0;
    (*(array+38)).type = 7;
    (*(array+38)).upgrade = 0;

    //39 紅王將
    (*(array+39)).color = 1;
    (*(array+39)).type = 7;
    (*(array+39)).upgrade = 0;
}

bool legal_color(Chess moving_chess,int current_color) //判斷要被移動的棋是否為輸入方的棋
{
    if(current_color%2==moving_chess.color)
    {
        //printf("current_color: %d\n",current_color%2);
        //printf("moving_chess.color %d\n",moving_chess.color);
        return true;
    }
    else
        return false;
}

///////////////////////////// Movement Restrictions ////////////////////////////

//步兵走法
bool movement00(bool color, int before_x, int before_y, int after_x, int after_y){
    if(!color)//color==blue
    {
        if((before_x==after_x) && (before_y-1==after_y))
        {
            return true;
        }
        else
            return false;
    }
    else //color==red
    {
        if((before_x==after_x) && (before_y+1==after_y))
        {
            return true;
        }
        else
            return false;
    }
}

//角行走法
bool movement01(bool color, int before_x, int before_y, int after_x, int after_y){
    if( ((after_x-before_x) == (after_y-before_y)) || ((after_x-before_x) == (before_y-after_y)))
    {
        return true;
    }
    else
        return false;
}

//飛車走法
bool movement02(bool color, int before_x, int before_y, int after_x, int after_y){
    if((before_x == after_x) || (before_y == after_y))
        return true;
    else
        return false;
}

//香車走法
bool movement03(bool color, int before_x, int before_y, int after_x, int after_y){
    if(!color) //blue
    {
        if((before_x == after_x) && (after_y < before_y))
            return true;
        else
            return false;
    }
    else //red
    {
        if((before_x == after_x) && (after_y > before_y))
             return true;
        else
            return false;
    }
}

//桂馬走法
bool movement04(bool color, int before_x, int before_y, int after_x, int after_y){
    if(!color)//blue
    {
        if(((before_x-1==after_x)|| (before_x+1==after_x)) && (before_y-2==after_y))
            return true;
        else
            return false;
    }
    else //red
    {
        if(((before_x-1==after_x)|| (before_x+1==after_x)) && (before_y+2==after_y))
            return true;
        else
            return false;
    }
}

//銀將走法
bool movement05(bool color, int before_x, int before_y, int after_x, int after_y){
    if(!color)//blue
    {
        if( ((-1 <= (after_x-before_x) && (after_x-before_x) <= 1) && (before_y-1==after_y)) ||
           (((-1 == (after_x-before_x)) || ((after_x-before_x)== 1)) && (before_y+1==after_y)) )
            return true;
        else
            return false;
    }
    else //red
    {
        if( ((-1 <= (after_x-before_x)&& (after_x-before_x) <= 1) && (before_y+1==after_y)) ||
           (((-1 == (after_x-before_x)) || ((after_x-before_x)== 1)) && (before_y-1==after_y)) )
            return true;
        else
            return false;
    }
}

//金將走法
bool movement06(bool color, int before_x, int before_y, int after_x, int after_y){

    if((before_y==after_y) && ((-1 == (after_x-before_x)) || ((after_x-before_x)== 1)))
        return true;

    else if(!color)//blue
    {
        if( (((-1 <= (after_x-before_x)) &&((after_x-before_x) <= 1)) && (before_y-1 == after_y)) || ((after_x == before_x) && (before_y+1 == after_y)))
            return true;
        else
            return false;
    }
    else //red
    {
        if( (((-1 <= (after_x-before_x)) &&((after_x-before_x) <= 1))  && (before_y+1 == after_y)) || ((after_x == before_x) && (before_y-1 == after_y)))
            return true;
        else
            return false;
    }
}

//升級角行走法
bool movement07(bool color, int before_x, int before_y, int after_x, int after_y){
    if((before_y==after_y) && ((-1 == (after_x-before_x)) || ((after_x-before_x)== 1)))
    {
        return true;
    }
    else if( !color && ((((-1 <= (after_x-before_x)) &&((after_x-before_x) <= 1))  && (before_y-1 == after_y)) || ((after_x == before_x) && (before_y+1 == after_y))))//blue
    {
        return true;
    }
    else if( (color) && ((((-1 <= (after_x-before_x)) &&((after_x-before_x) <= 1))  && (before_y+1 == after_y)) || ((after_x == before_x) && (before_y-1 == after_y))))
    {
        return true;
    }
    else if( ((after_x-before_x) == (after_y-before_y)) || ((after_x-before_x) == (before_y-after_y)))
    {
        return true;
    }
    else
        return false;
}

//升級飛車走法
bool movement08(bool color, int before_x, int before_y, int after_x, int after_y){
    if((before_y==after_y) && ((-1 == (after_x-before_x)) || ((after_x-before_x)== 1)))
    {
        return true;
    }
    else if( !color && ((((-1 <= (after_x-before_x)) &&((after_x-before_x) <= 1))  && (before_y-1 == after_y)) || ((after_x == before_x) && (before_y+1 == after_y))))//blue
    {
        return true;
    }
    else if( (color) && ((((-1 <= (after_x-before_x)) &&((after_x-before_x) <= 1))  && (before_y+1 == after_y)) || ((after_x == before_x) && (before_y-1 == after_y))))
    {
        return true;
    }
    else if((before_x == after_x) || (before_y == after_y))
        return true;
    else
        return false;
}

//王將走法
bool movement09(bool color, int before_x, int before_y, int after_x, int after_y){
    if(((-1 <= (after_x-before_x)) &&((after_x-before_x) <= 1))  && ((-1 <= (after_y-before_y)) &&((after_y-before_y) <= 1)) )
        return true;
    else
        return false;
}



bool legal_move(Chess moving_chess,int before_column,int before_row,int after_column,int after_row) //使用者輸入的欲移動位置以及目標位置(前提是已經確認移動的旗子是己方棋)
{
    bool Upgrade = moving_chess.upgrade;//printf("Upgrade %d\n",Upgrade);
    int Type = moving_chess.type;//printf("Type %d\n",Type);
    bool current_color = moving_chess.color;//printf("color %d\n",current_color);
    bool correct_step=1;

    if(Upgrade>1 || Type>7 || current_color>1 || before_column>8 || before_column<0 || before_row>8 || before_row<0 || after_row>8 || after_row<0 || after_column <0 ||after_column>8)
    {
        return false;
    }
    else if(Upgrade) // 升級過的棋
    {
        switch(Type)
        {
            case 0:{correct_step=movement06(current_color, before_row, before_column, after_row, after_column);break;}//符合升級步兵移動軌跡
            case 1:{correct_step=movement07(current_color, before_row, before_column, after_row, after_column);break;}//符合升級角行移動軌跡
            case 2:{correct_step=movement08(current_color, before_row, before_column, after_row, after_column);break;}//符合升級飛馬移動軌跡
            case 3:{correct_step=movement06(current_color, before_row, before_column, after_row, after_column);break;}//符合升級香車移動軌跡
            case 4:{correct_step=movement06(current_color, before_row, before_column, after_row, after_column);break;}//符合升級桂馬移動軌跡
            case 5:{correct_step=movement06(current_color, before_row, before_column, after_row, after_column);break;}//符合升級銀將移動軌跡
            default:{return false;}
        }
    }
    else// 未升級過的棋
    {
        switch(Type)
        {
            case 0:{correct_step=(movement00(current_color, before_row, before_column, after_row, after_column));break;}//步兵
            case 1:{correct_step=(movement01(current_color, before_row, before_column, after_row, after_column));break;}//角行
            case 2:{correct_step=(movement02(current_color, before_row, before_column, after_row, after_column));break;}//飛車
            case 3:{correct_step=(movement03(current_color, before_row, before_column, after_row, after_column));break;}//香車
            case 4:{correct_step=(movement04(current_color, before_row, before_column, after_row, after_column));break;}//桂馬
            case 5:{correct_step=(movement05(current_color, before_row, before_column, after_row, after_column));break;}//銀將
            case 6:{correct_step=(movement06(current_color, before_row, before_column, after_row, after_column));break;}//金將
            case 7:{correct_step=(movement09(current_color, before_row, before_column, after_row, after_column));break;}//王將
            default:{return false;}
        }
    }
    if(correct_step)
        return true;
    else
        return false;
}

