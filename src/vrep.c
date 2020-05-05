#include "vrep.h"

int isOnLine(float sensorValue){
    return(sensorValue > 0.7 ? 1:0 );
}

int isOffLine(float sensorValue){
    return(sensorValue < 0.5 ? 1:0 );
}

// use this function if the bot runs on
// black line over white surface
void invert(float sensorValue[], int n){
    for(int i=0; i<n; i++){
        sensorValue[i] = 1.0 - sensorValue[i];
    }
}

// get opposite direction
int invertDir(int dir){
    return (dir + 2) % 4;
}

void equateArray(float arr1[], float arr2[], int n){
    for(int i=0; i<n; i++){
        arr1[i] = arr2[i];
    }
}

void reverseArr(int arr[],int start, int end){
    while(start<end){
        int temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}

void readSensor(int clientID, int sensorHandle[], int n, float sensorValue[]){
    float* auxvalue = NULL;
    int* auxvaluecount = 0;
    for(int i=0; i<n; i++){
        if (simxReadVisionSensor(clientID,sensorHandle[i],NULL, &auxvalue, &auxvaluecount, simx_opmode_buffer) == simx_return_ok){
          sensorValue[i] = auxvalue[10];
        }
    }
}

// returns in what direction bot must turn
// if NESW of from and to is given
// eg. to turn from N to E
// turn right i.e 1
int turn(int from, int to){
    int i = to - from;
    switch(i){
        case -2:    return 2;
        case 3:     return -1;
        case -3:    return 1;
    }
    return  i;
}

// turns the bot in the direction given by 'lr'
void botTurn(int clientID, int sensorHandle[], int n, float sensorValue[], int leftMotorHandle, int rightMotorHandle, float optspeed, int lr,
             int reverseTrigger, int* noseDir){
    float turnK =3;
    float leftMotorSpeed;
    float rightMotorSpeed;

    *noseDir = (*noseDir + lr) % 4;

    // no turn
    if (lr == 0){
        leftMotorSpeed = optspeed;
        rightMotorSpeed = optspeed;
        // move forward till you leave the node
        do{
            readSensor(clientID, sensorHandle, n, sensorValue);
            invert(sensorValue, n);
            simxSetJointTargetVelocity(clientID, leftMotorHandle, leftMotorSpeed, simx_opmode_oneshot);
            simxSetJointTargetVelocity(clientID, rightMotorHandle, rightMotorSpeed, simx_opmode_oneshot);
        }
        while( !(isOffLine(sensorValue[4]) && isOffLine(sensorValue[5])) );
        return;
    }

    // reverse
    if (lr == 2){
        leftMotorSpeed = optspeed;
        rightMotorSpeed = -optspeed;
        // we rotate from right side
        // reverse trigger is 0 if there is a path on right side
        if(reverseTrigger == 0){
            // rotate till the middle sensor is off center
            do{
               readSensor(clientID, sensorHandle, n, sensorValue);
               invert(sensorValue, n);
               simxSetJointTargetVelocity(clientID, leftMotorHandle, leftMotorSpeed, simx_opmode_oneshot);
               simxSetJointTargetVelocity(clientID, rightMotorHandle, rightMotorSpeed, simx_opmode_oneshot);
            }
            while( ! isOffLine(sensorValue[6]));
            // rotate till the sensors are realligned on new path
            do{
               readSensor(clientID, sensorHandle, n, sensorValue);
               invert(sensorValue, n);
               simxSetJointTargetVelocity(clientID, leftMotorHandle, leftMotorSpeed, simx_opmode_oneshot);
               simxSetJointTargetVelocity(clientID, rightMotorHandle, rightMotorSpeed, simx_opmode_oneshot);
            }
            while( ! ( isOffLine(sensorValue[4]) && isOffLine(sensorValue[5]) && isOnLine(sensorValue[6]) ) );
        }
        // rotate till the middle sensor is off center
        do{
           readSensor(clientID, sensorHandle, n, sensorValue);
           invert(sensorValue, n);
           simxSetJointTargetVelocity(clientID, leftMotorHandle, leftMotorSpeed, simx_opmode_oneshot);
           simxSetJointTargetVelocity(clientID, rightMotorHandle, rightMotorSpeed, simx_opmode_oneshot);
        }
        while( ! ( isOffLine(sensorValue[4]) && isOffLine(sensorValue[5]) && isOffLine(sensorValue[6]) ) );
        // rotate till the sensors are realligned on new path
        do{
           readSensor(clientID, sensorHandle, n, sensorValue);
           invert(sensorValue, n);
           simxSetJointTargetVelocity(clientID, leftMotorHandle, leftMotorSpeed, simx_opmode_oneshot);
           simxSetJointTargetVelocity(clientID, rightMotorHandle, rightMotorSpeed, simx_opmode_oneshot);
        }
        while( ! ( isOffLine(sensorValue[4]) && isOffLine(sensorValue[5]) && isOnLine(sensorValue[6]) ) );
        // reverse ka error nikalne, maze me agar current node pata he toh redetect mat kar, ye part daal
        return;
    }

    // left(-1) or right(1) turn
    leftMotorSpeed = optspeed + turnK*lr;
    rightMotorSpeed = optspeed - turnK*lr;
    // rotate till the middle sensor is off center
    do{
       readSensor(clientID, sensorHandle, n, sensorValue);
       invert(sensorValue, n);
       simxSetJointTargetVelocity(clientID, leftMotorHandle, leftMotorSpeed, simx_opmode_oneshot);
       simxSetJointTargetVelocity(clientID, rightMotorHandle, rightMotorSpeed, simx_opmode_oneshot);
    }
    while( ! isOffLine(sensorValue[6]));
    // rotate till the sensors are realligned on new path
    do{
       readSensor(clientID, sensorHandle, n, sensorValue);
       invert(sensorValue, n);
       simxSetJointTargetVelocity(clientID, leftMotorHandle, leftMotorSpeed, simx_opmode_oneshot);
       simxSetJointTargetVelocity(clientID, rightMotorHandle, rightMotorSpeed, simx_opmode_oneshot);
    }
    while( ! isOnLine(sensorValue[6]) );
    // move forward till you leave the node
    leftMotorSpeed = optspeed;
    rightMotorSpeed = optspeed;
    do{
        readSensor(clientID, sensorHandle, n, sensorValue);
        invert(sensorValue, n);
        simxSetJointTargetVelocity(clientID, leftMotorHandle, leftMotorSpeed, simx_opmode_oneshot);
        simxSetJointTargetVelocity(clientID, rightMotorHandle, rightMotorSpeed, simx_opmode_oneshot);
    }
    while( !(isOffLine(sensorValue[4]) && isOffLine(sensorValue[5])) );
}


int nodePattern(int clientID, int sensorHandle[], int n, float sensorValue[], float dir[], int noseDir, int* end, int* reverseTrigger){

    int leftTrigger=-1, rightTrigger=-1, forwardTrigger=-1, endTrigger=-1;
    int node = 0;
    
    // if there is path on left or right side, we have entered a node
    // we keep checking other direction to confirm type of node
    while(isOnLine(sensorValue[4]) || isOnLine(sensorValue[5])){
        node = 1;
        if(isOnLine(sensorValue[4])) leftTrigger = 0;
        if(isOnLine(sensorValue[5])) rightTrigger = 0;
        if(isOnLine(sensorValue[6])) forwardTrigger = 0;
        if(isOnLine(sensorValue[7])){
            leftTrigger = -1;
            rightTrigger = -1;
            forwardTrigger = -1;
            endTrigger = 0;
            break;
        }
        readSensor(clientID, sensorHandle, n, sensorValue);
        invert(sensorValue, n);
    }

    // if we have encountered a node
    // store the dir values for scanMaze
    if (leftTrigger == 0 || rightTrigger == 0){
        int currDir = invertDir(noseDir);
        dir[currDir] = 0;
        dir[(currDir+1)%4] = leftTrigger;
        dir[(currDir+2)%4] = forwardTrigger;
        dir[(currDir+3)%4] = rightTrigger;
        //printf("\ndebug> left: %d, forward: %d, right: %d\n",leftTrigger, forwardTrigger, rightTrigger);
    }

    // set end variable and store dir values
    if(endTrigger == 0){
        *end = 1;
        int currDir = invertDir(noseDir);
        dir[currDir] = 0;
        dir[(currDir+1)%4] = -1;
        dir[(currDir+2)%4] = -1;
        dir[(currDir+3)%4] = -1;
        *reverseTrigger = -1;
    }
    // set end trigger if node is not end and has right turn
    else{
        *reverseTrigger = rightTrigger;
    }
    return node;

}

int main(int argc,char* argv[])
{
    int portNb=0;
    int botHandle;
    int gspotHandle;
    int leftMotorHandle;
    int rightMotorHandle;

    int n=8;
    int* sensorHandle;
    sensorHandle = (int*)malloc(n*sizeof (int));
    float* sensorValue;
    sensorValue = (float*)malloc(n*sizeof (float));
    for(int i=0; i<n; i++){
        sensorValue[i] = 1;
    }
    int sensorWeight[4] = {-3, -1, 1, 3};

    float pos = 0;
    float sum = 0;
    float optspeed = 200*PI/180;
    float leftMotorSpeed = optspeed;
    float rightMotorSpeed = optspeed;

    float position[3] = {0,0,0};
    float prevPos[3] = {0,0,0};
    float error = 0.2;

    int noseDir = 0;
    int currDir = 0;
    int prevDir = 0;
    float dir[4] = {-1, -1, -1, -1};
    int* nextDir = (int*)malloc(sizeof (int));
    int dirSize = 0;
    int turnDir = 0;
    int reverseTrigger = -1;

    float dist = 0;

    int end = 0;
    int map = 1;

    int lineK = 1;

    // capture handles from vrep
    if (argc>=6+n){
        portNb=atoi(argv[1]);
        botHandle = atoi(argv[2]);
        gspotHandle = atoi(argv[3]);
        leftMotorHandle=atoi(argv[4]);
        rightMotorHandle=atoi(argv[5]);
        for(int i=0; i<n; i++){
            sensorHandle[i]=atoi(argv[6+i]);
        }
    }
    else{
        printf("Indicate following arguments: 'portNumber bothandle leftMotorHandle rightMotorHandle sensorHandle[7]'!\n");
        extApi_sleepMs(5000);
        return 0;
    }

    // check connection
    int clientID=simxStart((simxChar*)"127.0.0.1",portNb,true,true,2000,5);
    if (clientID!=-1){
        // Set up position
        simxGetObjectPosition(clientID, gspotHandle, -1, position, simx_opmode_streaming);

        // Set up sensors
        float* auxvalue = NULL;
        int* auxvaluecount = 0;
        for(int i=0; i<n; i++){
            simxReadVisionSensor(clientID,sensorHandle[i],NULL, &auxvalue, &auxvaluecount, simx_opmode_streaming);
        }

        // start maze
        initializeMaze();

        // while we are connected
        while (simxGetConnectionId(clientID)!=-1){
            // printf("\ndebug> connected");
            
            // read position
            simxGetObjectPosition(clientID, gspotHandle, -1, position, simx_opmode_buffer);

            // read sensor values
            readSensor(clientID, sensorHandle, n, sensorValue);
            invert(sensorValue, n);

            // wait till all sensor values have been recieved
            int sensorError = 0;
            for(int i=0; i<n; i++){
                if(sensorValue[i] <= 0 || sensorValue[i] >= 1){
                    sensorError = 1;
                    break;
                }
            }
            if(sensorError) continue;

            // initialization
            end = 0;
            sum = 0;
            pos = 0;
            leftMotorSpeed = optspeed;
            rightMotorSpeed = optspeed;

            // find node pattern
            int node = nodePattern(clientID, sensorHandle, n, sensorValue, dir, noseDir, &end, &reverseTrigger);

            // if node encountered
            if( node != 0){
                dist = max(fabs(position[0] - prevPos[0]), fabs(position[1] - prevPos[1]));
                // printf("\ndebug> X: %f, Y: %f, dist: %f", position[0], position[1], dist);
                equateArray(prevPos, position, 3);

                // if bot doesn't know where to turn, scan node
                if( ! dirSize && map){
                  currDir = invertDir(noseDir);
                  nextDir = scanMaze(position[0], position[1], prevDir, currDir, dist, end, dir, &dirSize, error);
                  reverseArr(nextDir, 0, dirSize-1);
                }

                // if bot doesn't know where to turn still
                // movement complete
                if (dirSize == 0){
                    leftMotorSpeed = 0;
                    rightMotorSpeed = 0;
                    simxSetJointTargetVelocity(clientID, leftMotorHandle, leftMotorSpeed, simx_opmode_oneshot);
                    simxSetJointTargetVelocity(clientID, rightMotorHandle, rightMotorSpeed, simx_opmode_oneshot);
                    // printf("\ndebug> stop");
                    nextDir = shortestPath(&dirSize);
                    printDir(nextDir, dirSize);
                    reverseArr(nextDir, 0, dirSize-1);
                    noseDir = 0;
                    map = 0;

                    // reset bot to initial position for round 2 within 20 seconds
                    extApi_sleepMs(20000);
                }
                dirSize--;
                turnDir = turn(noseDir, nextDir[dirSize]);
                // printf("\ndebug> TURNING %dn", turnDir);
                botTurn(clientID, sensorHandle, n, sensorValue, leftMotorHandle, rightMotorHandle, optspeed, turnDir, reverseTrigger, &noseDir);
                prevDir = nextDir[dirSize];
                noseDir = nextDir[dirSize];
                
                // print the graph obtained till now
                printGraph();

            }

            // line follow
            for(int i=0; i<4; i++){
                // printf("\ndebug> sensor %d: %f",i,sensorValue[i]);
                sum += sensorValue[i];
                pos += sensorWeight[i]*sensorValue[i];
            }
            pos /= sum;
            // printf("\ndebug> pos: %f", pos);

            // if bot on left side of line pos is positive
            // if bot on right side of line pos is negative
            leftMotorSpeed = optspeed + lineK*pos;
            rightMotorSpeed = optspeed - lineK*pos;

            // reverse if dead end
            if(isOffLine(sensorValue[0]) && isOffLine(sensorValue[1]) && isOffLine(sensorValue[2]) && isOffLine(sensorValue[3])
            && isOffLine(sensorValue[4]) && isOffLine(sensorValue[5]) && isOffLine(sensorValue[6]) && !node && pos < 0.3){
                botTurn(clientID, sensorHandle, n, sensorValue, leftMotorHandle, rightMotorHandle, optspeed, 2, reverseTrigger, &noseDir);
            }

            // run motors
            simxSetJointTargetVelocity(clientID, leftMotorHandle, leftMotorSpeed, simx_opmode_oneshot);
            simxSetJointTargetVelocity(clientID, rightMotorHandle, rightMotorSpeed, simx_opmode_oneshot);

        }
        simxFinish(clientID);
    }
    return(0);
}
