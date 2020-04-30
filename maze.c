#include "maze.h"

// global variable to be used internally in this library
struct Graph* graph = NULL;
int currentNode=0, prevNode=0;        
int turns = 0;                      // total paths left to check in the scanned area
int finish = -1;                    // store the index number of end node
bool end = 0;


/******************************************************************************/
// Construct a graph

// A utility function to create a new adjacency list node 
struct AdjListNode* newAdjListNode(int dest, float weight, int dir) 
{ 
	struct AdjListNode* newNode = (struct AdjListNode*) malloc(sizeof(struct AdjListNode)); 
	newNode->dest = dest;       // node index
	newNode->weight = weight;   // distance of node
    newNode->dir = dir;         // direction of node
	newNode->next = NULL;       
	return newNode; 
}

// create a graph and initialize the first node
void createGraph() 
{ 
	graph = (struct Graph*) malloc(sizeof(struct Graph)); 
    graph->V = 0;
	graph->array = (struct AdjList*) malloc(graph->V * sizeof(struct AdjList));
}

void updateGraph()
{
    graph->V+=1;
    graph->array = (struct AdjList*) realloc(graph->array,graph->V*sizeof(struct AdjList));
    graph->array[graph->V-1].head = NULL;
    graph->array[graph->V-1].X = 0;
    graph->array[graph->V-1].Y = 0;
    graph->array[graph->V-1].incomplete = 0;
    for(int i=0;i<4;i++)
        graph->array[graph->V-1].dir[i] = -1;
}

void addEdge(int src, int dest, float weight, int dirSrc) 
{ 
	// Add an edge from src to dest. A new node is added to the adjacency 
	// list of src. The node is added at the beginning 
	struct AdjListNode* newNode = newAdjListNode(dest, weight,dirSrc); 
	newNode->next = graph->array[src].head; 
	graph->array[src].head = newNode; 
}

struct Graph* getGraph()
{
    return graph;
}

void printGraph()
{
    for(int i=0; i<graph->V; i++)
    {
        printf("node %d: \n",i);
        printf("X: %f, Y: %f, incomplete: %d\n",graph->array[i].X, graph->array[i].Y, graph->array[i].incomplete);
        struct AdjListNode* pCrawl = graph->array[i].head;
        while(pCrawl)
        {
            printf("dir: %d, dist: %f, node: %d\n", pCrawl->dir, pCrawl->weight, pCrawl->dest);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}

void printDir(int dist[], int n) 
{ 
    printf("\nStep \t\t Direction\n"); 
    for (int i = 0; i < n; ++i) 
        printf("%d \t\t %d\n", i, dist[i]); 
}
 
void printArr(float dist[], int n) 
{ 
    printf("\nVertex \t\t Distance\n"); 
    for (int i = 0; i < n; ++i) 
        printf("%d \t\t %f\n", i, dist[i]); 
} 

/************************************************************************************/
// Dijkstra - shortest path algorithm
  
// A utility function to create a new Min Heap Node 
struct MinHeapNode* newMinHeapNode(int v, float dist) 
{ 
    struct MinHeapNode* minHeapNode = (struct MinHeapNode*) malloc(sizeof(struct MinHeapNode)); 
    minHeapNode->v = v; 
    minHeapNode->dist = dist; 
    return minHeapNode; 
} 
  
// A utility function to create a Min Heap 
struct MinHeap* createMinHeap(int capacity) 
{ 
    struct MinHeap* minHeap = (struct MinHeap*) malloc(sizeof(struct MinHeap)); 
    minHeap->pos = (int *)malloc(capacity * sizeof(int)); 
    minHeap->size = 0; 
    minHeap->capacity = capacity; 
    minHeap->array = (struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*)); 
    return minHeap; 
} 
  
// A utility function to swap two nodes of min heap. Needed for min heapify 
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) 
{ 
    struct MinHeapNode* t = *a; 
    *a = *b; 
    *b = t; 
} 
  
// A standard function to heapify at given idx 
// This function also updates position of nodes when they are swapped. 
// Position is needed for decreaseKey() 
void minHeapify(struct MinHeap* minHeap, int idx) 
{ 
    int smallest, left, right; 
    smallest = idx; 
    left = 2 * idx + 1; 
    right = 2 * idx + 2; 
  
    if (left < minHeap->size && minHeap->array[left]->dist < minHeap->array[smallest]->dist ) 
        smallest = left; 
  
    if (right < minHeap->size && minHeap->array[right]->dist < minHeap->array[smallest]->dist ) 
        smallest = right; 
  
    if (smallest != idx) 
    { 
        // The nodes to be swapped in min heap 
        struct MinHeapNode *smallestNode = minHeap->array[smallest]; 
        struct MinHeapNode *idxNode = minHeap->array[idx]; 
  
        // Swap positions 
        minHeap->pos[smallestNode->v] = idx; 
        minHeap->pos[idxNode->v] = smallest; 
  
        // Swap nodes 
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]); 
  
        minHeapify(minHeap, smallest); 
    } 
} 
  
// A utility function to check if the given minHeap is ampty or not 
int isEmpty(struct MinHeap* minHeap) 
{ 
    return minHeap->size == 0; 
} 
  
// Standard function to extract minimum node from heap 
struct MinHeapNode* extractMin(struct MinHeap* minHeap) 
{ 
    if (isEmpty(minHeap)) 
        return NULL; 
  
    // Store the root node 
    struct MinHeapNode* root = minHeap->array[0]; 
  
    // Replace root node with last node 
    struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1]; 
    minHeap->array[0] = lastNode; 
  
    // Update position of last node 
    minHeap->pos[root->v] = minHeap->size-1; 
    minHeap->pos[lastNode->v] = 0; 
  
    // Reduce heap size and heapify root 
    --minHeap->size; 
    minHeapify(minHeap, 0); 
  
    return root; 
} 
  
// Function to decreasy dist value of a given vertex v. This function 
// uses pos[] of min heap to get the current index of node in min heap 
void decreaseKey(struct MinHeap* minHeap, int v, float dist) 
{ 
    // Get the index of v in  heap array 
    int i = minHeap->pos[v]; 
  
    // Get the node and update its dist value 
    minHeap->array[i]->dist = dist; 
  
    // Travel up while the complete tree is not hepified. 
    // This is a O(Logn) loop 
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist) 
    { 
        // Swap this node with its parent 
        minHeap->pos[minHeap->array[i]->v] = (i-1)/2; 
        minHeap->pos[minHeap->array[(i-1)/2]->v] = i; 
        swapMinHeapNode(&minHeap->array[i],  &minHeap->array[(i - 1) / 2]); 
  
        // move to parent index 
        i = (i - 1) / 2; 
    } 
} 
  
// A utility function to check if a given vertex 
// 'v' is in min heap or not 
bool isInMinHeap(struct MinHeap *minHeap, int v) 
{ 
    if (minHeap->pos[v] < minHeap->size) 
        return true; 
    return false; 
}
  
void reverseArray(int arr[],int start, int end)
{
    while(start<end)
    {
        int temp = arr[start];  
        arr[start] = arr[end]; 
        arr[end] = temp; 
        start++; 
        end--;
    } 
}

int* getDirection(int parent[], int src, int dest, int* dirSize)
{
    float min = INT_MAX;
    struct AdjListNode* pCrawl = NULL;
    int* dir = (int*)malloc(sizeof(int));
    int capacity = 0;
    int px = dest, nx = dest;
    while(nx!=src)
    {
        capacity++;
        dir = (int*)realloc(dir,capacity*sizeof(int));
        nx = parent[px];
        min = INT_MAX;
        pCrawl = graph->array[nx].head;
        while(pCrawl!=NULL)
        {
            if(pCrawl->dest==px && pCrawl->weight<min)
            {
                min = pCrawl->weight;
                dir[capacity-1] = pCrawl->dir;
            }
            pCrawl = pCrawl->next;
        }
        px=nx;
    }
    reverseArray(dir, 0, capacity-1);
    *dirSize = capacity;
    return dir;
}

// The main function that calulates distances of shortest paths from src to all 
// vertices. It is a O(ELogV) function 
int* dijkstra(int src, int dest, int* dirSize) 
{ 
    int V = graph->V;   // Get the number of vertices in graph 
    float* dist = (float*)malloc(V * sizeof(float));        // dist values used to pick minimum weight edge in cut 
    int* parent = (int*)malloc(V * sizeof(int));
  
    // minHeap represents set E 
    struct MinHeap* minHeap = createMinHeap(V); 
  
    // Initialize min heap with all vertices. dist value of all vertices  
    for (int v = 0; v < V; ++v)
    { 
        parent[v]=-1;
        dist[v] = INT_MAX; 
        minHeap->array[v] = newMinHeapNode(v, dist[v]); 
        minHeap->pos[v] = v; 
    } 
  
    // Make dist value of src vertex as 0 so that it is extracted first 
    minHeap->array[src] = newMinHeapNode(src, dist[src]); 
    minHeap->pos[src]   = src; 
    dist[src] = 0; 
    decreaseKey(minHeap, src, dist[src]); 
  
    // Initially size of min heap is equal to V 
    minHeap->size = V; 
  
    // In the following loop, min heap contains all nodes 
    // whose shortest distance is not yet finalized. 
    while (!isEmpty(minHeap))
    { 
        // Extract the vertex with minimum distance value 
        struct MinHeapNode* minHeapNode = extractMin(minHeap); 
        int u = minHeapNode->v; // Store the extracted vertex number 
  
        // Traverse through all adjacent vertices of u (the extracted 
        // vertex) and update their distance values 
        struct AdjListNode* pCrawl = graph->array[u].head; 
        while (pCrawl != NULL)
        { 
            int v = pCrawl->dest; 
  
            // If shortest distance to v is not finalized yet, and distance to v 
            // through u is less than its previously calculated distance 
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX &&  
                pCrawl->weight + dist[u] < dist[v]){ 
                parent[v] = u;
                dist[v] = dist[u] + pCrawl->weight; 
  
                // update distance value in min heap also 
                decreaseKey(minHeap, v, dist[v]); 
            } 
            pCrawl = pCrawl->next; 
        } 
    } 
    return getDirection(parent, src, dest, dirSize);
}

/*************************************************************************/
// Scan the maze

void initializeMaze()
{
    createGraph();
    updateGraph();
    graph->array[0].dir[NORTH] = 0;
    graph->array[0].incomplete = 1;
    turns = 1;  
}

int ruleNESW()
{
    for(int i=0; i<4; i++)
        if(graph->array[currentNode].dir[i] == 0)
            return i;
    return -1;
}

void printLogs()
{
    printf("\noutput> end: %d",end);
    printf("\noutput> turns: %d",turns);
    printf("\noutput> current: %d",currentNode);
    printf("\noutput> finish: %d",finish);
    printf("\noutput> incomplete: %d",graph->array[currentNode].incomplete);
    printf("\noutput> direction 0: %d",graph->array[currentNode].dir[0]);
    printf("\noutput> direction 1: %d",graph->array[currentNode].dir[1]);
    printf("\noutput> direction 2: %d",graph->array[currentNode].dir[2]);
    printf("\noutput> direction 3: %d",graph->array[currentNode].dir[3]); 
}

// returns an array of direction
int* scanMaze(float X, float Y, int prevDir, int currDir, float dist, bool nodeEnd, int dir[], int* dirSize, float error)
{
    // printf("\ndebug> scanMaze");
    int* nextDir = (int*)malloc(sizeof(int));

    // 1. If scan is complete return NULL
    if(end && !turns)
    {
        // printf("\ndebug> scan complete");
        *dirSize = 0;
        return NULL;
    }

    // 2. Get value of currentNode
    // checking if node previously encountered (linear search)
    int search = 0;
    for (search=0;search<graph->V;search++)
    {
        if( fabs(X - graph->array[search].X) <= error && fabs(Y - graph->array[search].Y) <= error)
        {
            // printf("\ndebug> old node");
            currentNode=search;
            break;
        }
    }
    // if current node has not been encountered previously
    // add it to graph with all the properties
    // printf("\ndebug> search: %d", search);
    if(search==graph->V)
    {
        // printf("\ndebug> new node");
        currentNode=graph->V;
        updateGraph();
        graph->array[currentNode].X = X;
        graph->array[currentNode].Y = Y;
        // check if this node is end node
        if(nodeEnd)
        {
            // printf("\ndebug> end true");
            finish = currentNode;
            end = nodeEnd;
        }
        // setting node directions
        for(int i=0;i<4;i++)
        {
            graph->array[currentNode].dir[i] = dir[i];
            // if there exists a path, increment the turns variable
            if(dir[i]==0)
            {
                turns+=1;
                graph->array[currentNode].incomplete+=1;
            }
        }    
    }

    // 3. Mark the path travelled on prevNode and currentNode
    // If the path travelled was a self loop, remove from graph
    if(prevNode==currentNode)
    {    
        // printf("\ndebug> self loop");
        if(graph->array[prevNode].dir[prevDir] == 0)
        {    
            graph->array[prevNode].dir[prevDir] = -1;
            graph->array[prevNode].incomplete--;
            turns--;
        }
        if(graph->array[currentNode].dir[currDir] == 0)
        {
            graph->array[currentNode].dir[currDir] = -1;
            graph->array[currentNode].incomplete--;
            turns--;
        }
    }
    // if this node is different from previous node
    // we have successfully explored a path
    // add the distance data to previous node exit direction
    // and current node entry direction
    else
    {
        // printf("\ndebug> distance");
        if(graph->array[prevNode].dir[prevDir] == 0)
        {
            graph->array[prevNode].dir[prevDir] = dist;
            graph->array[prevNode].incomplete--;
            turns--;
            addEdge(prevNode, currentNode, dist, prevDir);
        }
        if(graph->array[currentNode].dir[currDir] == 0)
        {
            graph->array[currentNode].dir[currDir] = dist;
            graph->array[currentNode].incomplete--;
            turns--;
            addEdge(currentNode, prevNode, dist, currDir);
        }
           
    }

    prevNode = currentNode;
    // printLogs();
    
    // 4. Find the next direction to scan
    // if all directions of current node explored
    // but there are remaining paths
    // we need to somehow get there
    if(!graph->array[currentNode].incomplete && turns)
    {
        // printf("\ndebug> current node complete");
        // check each node connected to this node if it is compete or not
        // if a neighbouring is incomplete, go to that node
        struct AdjListNode* pCrawl;
        pCrawl = graph->array[currentNode].head;
        while (pCrawl!=NULL)
        {
            if (graph->array[pCrawl->dest].incomplete)
            {
                currentNode = pCrawl->dest;
                *dirSize = 1;
                *nextDir = pCrawl->dir;
                return nextDir;
            }
            pCrawl = pCrawl->next;
        }
        // if all neighbouring node were also complete
        // We check the next node higher in index
        // If all the nodes from current to highest index are also complete
        // We check the nodes from start to current node
        // these checks are within the graph, the bot is still in the current node and has not moved
        // this method can be replace by others like
        // checking current +- 1, 2, ...
        // this will give us the nearest indexed incomplete node
        if(pCrawl==NULL)
        {   
            // printf("debug> neighbours complete finding next\n");
            // if x is current node, go from x+1 to max node, then from 1 to current node
            // the for loop below is enough to handle all completed node decisions
            // the upper part is just to speed up
            for(int i=(currentNode+1)%graph->V;i!=currentNode;i=(i+1)%graph->V)
            {
                // if we find an incomplete node
                // we use dijkstra algo now itself to find shortest path to that node
                if(graph->array[i].incomplete)
                {
                    nextDir = dijkstra(currentNode, i, dirSize);
                    currentNode = i;
                    return nextDir;
                }
            }
        }
    }
    // if the current node is incomplete
    // find the next node by NESW rule
    *nextDir = ruleNESW();
    if (*nextDir != -1)
    {
        // printf("\ndebug> ruleNESW gave direction");
        *dirSize = 1;
        return nextDir;
    }
    else
    {
        // printf("\ndebug> ruleNESW gave -1");
        *dirSize = 0;
        return NULL;
    }
}

int* shortestPath(int* dirSize)
{
    return dijkstra(0, finish, dirSize);
}
