#include<stdlib.h>
#include<raylib.h>

#define MAX_ELEMENTS 20

typedef struct{
    Vector2 coordinates;
    float height;
    float width;
    int element;
}decoration;

decoration decore[MAX_ELEMENTS]={0};

void init_decore(){
    int index=0;
    for(int i=0;i<5;i++){
        if(index<MAX_ELEMENTS){
            decore[index].element=1;
            decore[index].coordinates.x=50+rand()%100;
            decore[index].coordinates.y=50+(rand()%100+((SCREEN_HEIGHT)-200));
            decore[index].width=50+rand()%50;
            decore[index].height=100+rand()%100;
            index++;
        }
    }
    for(int i=0;i<5;i++){
        if(index<MAX_ELEMENTS){
            decore[index].element=1;
            decore[index].coordinates.x=100+(rand()%100+((SCREEN_WIDTH)-200));
            decore[index].coordinates.y=100+(rand()%((SCREEN_HEIGHT)-200));
            decore[index].width=50+rand()%50;
            decore[index].height=100+rand()%100; 
            index++;
        }
    }
    for(int i=0;i<10;i++){
        if(index<MAX_ELEMENTS){
            decore[index].element=2;
            decore[index].coordinates.x=rand()%SCREEN_WIDTH;
            decore[index].coordinates.y=rand()%SCREEN_HEIGHT;
            decore[index].width=20+rand()%20;
            decore[index].height=50+rand()%50;
            index++;
        }
    }
}

void drawDecore(){
    for(int i=0;i<MAX_ELEMENTS;i++){
        if(decore[i].element==1){
            Rectangle rect={
                decore[i].coordinates.x, 
                decore[i].coordinates.y-decore[i].height, 
                decore[i].width, 
                decore[i].height
            };
            DrawRectangleRec(rect,WHITE);
        }
        else if(decore[i].element==2){
            float body_w=decore[i].width;
            float body_h=(decore[i].height)*0.4f;
            Rectangle body={
                decore[i].coordinates.x-(body_w/2),
                decore[i].coordinates.y-(body_h),
                body_w,
                body_h
            };
            DrawRectangleRec(body,BROWN);
            float leafRadius=decore[i].width*2;
                Vector2 leafRadius_pos={
                    decore[i].coordinates.x, 
                    body.y-leafRadius/2
                };
            DrawCircleV(leafRadius_pos,leafRadius,GREEN);
        }
    }
}