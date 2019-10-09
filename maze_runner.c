// C / C++ program for Dijkstra's shortest path algorithm for adjacency 
// list representation of graph 
#include <stdbool.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h> 

// A structure to represent a node in adjacency list 
struct AdjListNode 
{ 
	int dest; 
	int weight;
	int dir; 
	struct AdjListNode* next; 
}; 

// A structure to represent an adjacency liat 
struct AdjList 
{ 
	struct AdjListNode* head; // pointer to head node of list 
	int X;
	int Y;
	int incomplete;
	int dir[4];
}; 

// A structure to represent a graph. A graph is an array of adjacency lists. 
// Size of array will be V (number of vertices in graph) 
struct Graph 
{ 
	int V; 
	struct AdjList* array; 
}; 

// A utility function to create a new adjacency list node 
struct AdjListNode* newAdjListNode(int dest, int weight, int dir) 
{ 
	struct AdjListNode* newNode = (struct AdjListNode*) malloc(sizeof(struct AdjListNode)); 
	newNode->dest = dest; 
	newNode->weight = weight;
    newNode->dir = dir;
	newNode->next = NULL; 
	return newNode; 
}

struct Graph* createGraph() 
{ 
	struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph)); 
    graph->V = 1; 
	graph->array = (struct AdjList*) malloc(graph->V * sizeof(struct AdjList)); 
    graph->array[0].head = NULL; 
    for(int i=0;i<4;i++)
        graph->array[0].dir[i] = -1;
	return graph; 
}

struct Graph* updateGraph(struct Graph* graph)
{
    graph->V+=1;
    graph->array = (struct AdjList*) realloc(graph->array,graph->V*sizeof(struct AdjList));
    graph->array[graph->V-1].head = NULL;
    for(int i=0;i<4;i++)
        graph->array[graph->V-1].dir[i] = -1;
    return graph;
}

void addEdge(struct Graph* graph, int src, int dest, int weight, int dirSrc, int dirDest) 
{ 
	// Add an edge from src to dest. A new node is added to the adjacency 
	// list of src. The node is added at the beginning 
	struct AdjListNode* newNode = newAdjListNode(dest, weight,dirSrc); 
	newNode->next = graph->array[src].head; 
	graph->array[src].head = newNode; 

	// Since graph is undirected, add an edge from dest to src also 
	newNode = newAdjListNode(src, weight,dirDest); 
	newNode->next = graph->array[dest].head; 
	graph->array[dest].head = newNode; 
} 

//************************************************************************************
// Dijkstra - shortest path algorithm

// Structure to represent a min heap node 
struct MinHeapNode 
{ 
    int  v; 
    int dist; 
}; 
  
// Structure to represent a min heap 
struct MinHeap 
{ 
    int size;      // Number of heap nodes present currently 
    int capacity;  // Capacity of min heap 
    int *pos;     // This is needed for decreaseKey() 
    struct MinHeapNode **array; 
}; 
  
// A utility function to create a new Min Heap Node 
struct MinHeapNode* newMinHeapNode(int v, int dist) 
{ 
    struct MinHeapNode* minHeapNode = 
           (struct MinHeapNode*) malloc(sizeof(struct MinHeapNode)); 
    minHeapNode->v = v; 
    minHeapNode->dist = dist; 
    return minHeapNode; 
} 
  
// A utility function to create a Min Heap 
struct MinHeap* createMinHeap(int capacity) 
{ 
    struct MinHeap* minHeap = 
         (struct MinHeap*) malloc(sizeof(struct MinHeap)); 
    minHeap->pos = (int *)malloc(capacity * sizeof(int)); 
    minHeap->size = 0; 
    minHeap->capacity = capacity; 
    minHeap->array = 
         (struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*)); 
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
  
    if (left < minHeap->size && 
        minHeap->array[left]->dist < minHeap->array[smallest]->dist ) 
      smallest = left; 
  
    if (right < minHeap->size && 
        minHeap->array[right]->dist < minHeap->array[smallest]->dist ) 
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
void decreaseKey(struct MinHeap* minHeap, int v, int dist) 
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
  
// A utility function used to print the solution 
void printArr(int dist[], int n) 
{ 
    printf("\nVertex   Distance from Source\n"); 
    for (int i = 0; i < n; ++i) 
        printf("%d \t\t %d\n", i, dist[i]); 
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

int* getDirection(struct Graph* graph,int parent[], int src, int dest, int* dirSize)
{
    int min = INT_MAX;
    struct AdjListNode* pCrawl = NULL;
    int* dir = (int*)malloc(sizeof(int));
    int capacity = 0;
    int px = dest, nx = dest;
    while(nx!=src)
    {
        capacity++;
        dir = (int*)realloc(dir,capacity);
        nx = parent[px];
        min = INT_MAX;
        pCrawl = graph->array[nx].head;
        while(pCrawl!=NULL)
        {
            if(pCrawl->dest==px && pCrawl->weight<min)
            {
                min = pCrawl->weight;
                dir[capacity-1] = pCrawl->dir;
                //printArr(dir,capacity);
            }
            pCrawl = pCrawl->next;
        }
        px=nx;
    }
    reverseArray(dir, 0, capacity-1);
    printArr(dir,capacity);
    *dirSize = capacity;
    return dir;
}

// The main function that calulates distances of shortest paths from src to all 
// vertices. It is a O(ELogV) function 
int* dijkstra(struct Graph* graph, int src, int dest, int* dirSize) 
{ 
    int V = graph->V;// Get the number of vertices in graph 
    int dist[V];      // dist values used to pick minimum weight edge in cut 
    int parent[V];
  
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
  
    // In the followin loop, min heap contains all nodes 
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
                                          pCrawl->weight + dist[u] < dist[v]) 
            { 
                parent[v] = u;
                dist[v] = dist[u] + pCrawl->weight; 
  
                // update distance value in min heap also 
                decreaseKey(minHeap, v, dist[v]); 
            } 
            pCrawl = pCrawl->next; 
        } 
    } 
  
    // print the calculated shortest distances 
    printf("\ninside dijkstra");
    printArr(parent, V);
    return getDirection(graph, parent, src, dest, dirSize);
} 

//*********************************************************************************************************

int main()
{
    // // create the graph given in above fugure 
    // int V = 9; 
    // struct Graph* graph = createGraph();
    // graph = updateGraph(graph);
    // graph = updateGraph(graph);
    // graph = updateGraph(graph);
    // graph = updateGraph(graph);
    // graph = updateGraph(graph);
    // graph = updateGraph(graph);
    // graph = updateGraph(graph);
    
    // addEdge(graph, 0, 1, 4,0,0); 
    // addEdge(graph, 0, 7, 8,0,0); 
    // addEdge(graph, 1, 2, 8,0,0); 
    // addEdge(graph, 1, 7, 11,0,0); 
    // addEdge(graph, 2, 3, 7,0,0); 
    // addEdge(graph, 2, 8, 2,0,0); 
    // addEdge(graph, 2, 5, 4,0,0); 
    // addEdge(graph, 3, 4, 9,0,0); 
    // addEdge(graph, 3, 5, 14,0,0); 
    // addEdge(graph, 4, 5, 10,0,0); 
    // addEdge(graph, 5, 6, 2,0,0); 
    // addEdge(graph, 6, 7, 1,0,0); 
    // addEdge(graph, 6, 8, 6,0,0); 
    // addEdge(graph, 7, 8, 7,0,0); 
  
    // dijkstra(graph, 0, 0); 

    struct Graph* graph = createGraph();
    int currentNode=0, prevNode=0, X=0, Y=0, dirSrc = 3, dirDest = 3, dist=0, turns = 0, end = 0, endFlag = 0, finish = INT_MAX;
    int search = 0, dirSize = 0;
    int* dir = NULL;
    // for first node
    struct AdjListNode* pCrawl;
    
    while(!end||turns)
    {
        // get these data from bot

        printf("\nentry dir: ");
        scanf("%d",&dirDest);
        printf("\nX: ");
        scanf("%d",&X);
        printf("\nY: ");
        scanf("%d",&Y);

        for (search=0;search<graph->V;search++)             // checking if node previously encountered
        {
            if(X==graph->array[search].X&&Y==graph->array[search].Y)  
            {
                currentNode=search;
                break;
            }
        }
        if(search==graph->V)                                // newNode
        {
            currentNode=graph->V;
            graph = updateGraph(graph);
        
            graph->array[currentNode].X = X;
            graph->array[currentNode].Y = Y;
            if(!end)
            {
                printf("\nend ?: ");
                scanf("%d",&end);
                if(end)
                    endFlag = 1;
            }

            if(endFlag)
            {
                finish = currentNode;
                endFlag=0;
            }	
            else
            {
                for(int j=0;j<4;j++)
                {
                    printf("\ndirection %d: ",j);
                    scanf("%d",&(graph->array[currentNode].dir[j]));    // if dir exist enter 0 else -1
                    if(graph->array[currentNode].dir[j]==0)
                    {
                        turns+=1;
                        graph->array[currentNode].incomplete+=1;
                    }
                }
            }        
        }
        printf("\ncurrent node: %d",currentNode);
        
        printf("\ndirection 0: %d",graph->array[currentNode].dir[0]);
        printf("\ndirection 1: %d",graph->array[currentNode].dir[1]);
        printf("\ndirection 2: %d",graph->array[currentNode].dir[2]);
        printf("\ndirection 3: %d",graph->array[currentNode].dir[3]);

        // dont do this for start and end
        if(currentNode!=0 && currentNode!=finish)
        {
            graph->array[currentNode].incomplete-=1;
            turns-=1;
        }

        // setting values of distances to direction of nodes
        if(prevNode==currentNode)	// start node or looped on same node
        {    
            graph->array[currentNode].dir[dirSrc] = -1;
            graph->array[currentNode].dir[dirDest] = -1;
        }
        else 				// for new node or previously encountered node
        {
            graph->array[currentNode].dir[dirDest] = dist;
            addEdge(graph, prevNode, currentNode, dist, dirSrc, dirDest);   
        }
        printf("\nturns: %d",turns);
        // if all directions of current node explored
        // then do this
        // *************************************
        if(!graph->array[currentNode].incomplete && turns)
        {
            // find incomplete node
            pCrawl = graph->array[currentNode].head;
            while (pCrawl!=NULL)
            {
                if (graph->array[pCrawl->dest].incomplete)
                {
                    printf("\nturning %d",pCrawl->dir);
                    currentNode = pCrawl->dest;
                    printf("\ncurrent node: %d",currentNode);
                    break;
                }
                pCrawl = pCrawl->next;
            }
            if(pCrawl==NULL)
            {   // if x is current node, go from x+1 to max node, then from 1 to current node
                // the for loop below is enough to handle all completed node decisions
                // the upper part is just to speed up
                for(int i=(currentNode+1)%graph->V;i!=currentNode;i=(i+1)%graph->V)
                {
                    if(graph->array[i].incomplete)
                    {
                        dir = dijkstra(graph, currentNode, i, &dirSize);
                        printf("\nturning");
                        printArr(dir, dirSize); // +1 nai pata
                        free(dir);
                        currentNode = i;
                        printf("\ncurrent node: %d",currentNode);
                        break;
                    }
                }
            }
        } 
        printf("\nturns: %d",turns);
        // bot going to explore a previously unexplored direction
        printf("\nend: %d",end);
        if(end && !turns)
            break;	
        
        // change current node as per dijkstra, then proceed
        printf("\nleaving dir: ");
        scanf("%d",&dirSrc);
        printf("\ndist: ");
        scanf("%d",&dist);
        graph->array[currentNode].dir[dirSrc] = dist;
        if(currentNode!=0 && currentNode!=finish)
        {
            graph->array[currentNode].incomplete-=1;
            turns-=1; 
        }   
        
        
        prevNode = currentNode;

        // print logs
        printf("\nend: %d",end);
        printf("\nturns: %d",turns);
        printf("\ncurrent: %d",currentNode);
        printf("\nfinish: %d",finish);
        printf("\nincomplete: %d",graph->array[currentNode].incomplete);
        printf("\ndirection 0: %d",graph->array[currentNode].dir[0]);
        printf("\ndirection 1: %d",graph->array[currentNode].dir[1]);
        printf("\ndirection 2: %d",graph->array[currentNode].dir[2]);
        printf("\ndirection 3: %d",graph->array[currentNode].dir[3]);
        
    }
    
    dir = dijkstra(graph, 0, finish, &dirSize);
    printf("\nbot stop");
    printArr(dir, dirSize); // +1 kyu kiya nai pata
    // BOT STOP
    
    return 0;
}
