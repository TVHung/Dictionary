#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "jrb.h"
#include "dllist.h"

#define INFINITIVE_VALUE 10000000
typedef struct {
    JRB edges;
    JRB vertices;
    JRB bus;
} Graph;

Graph createGraph();// ham khoi tao do thi

char *Conduong(Graph graph,int v1,int v2);// ham dua ra ten tuyen bus tu v1->v2

void addVertex(Graph graph, int id, char* name);// ham them dinh vao trong vertices

char *getVertex(Graph graph, int id);// ham dua ra ten dinh ma dc hieu la id

void addEdge(Graph graph, int v1, int v2, double weight,char *s);// ham them canh lien ket giua v1->v2, dong thoi cap nhat trong so

double getEdgeValue(Graph graph, int v1, int v2);// ham dua ra trong so cho su lien ket v1->v2

int indegree(Graph graph, int v, int* output);// ham dua ra so dinh vao v

int outdegree(Graph graph, int v, int* output);// ham dua ra so dinh ra tu v

double shortestPath(Graph graph, int s, int t, int *path, int *length);// ham tim kiem duuong di ngan nhat

void dropGraph(Graph graph);// ham xoa  bo du lieu cua graph

int getId(Graph graph,char *name);// ham dua ra id tu name

void nhap_thog_so(Graph graph,FILE *fo)// ham truyen du lieu vao graph
{
   // FILE *fo = fopen("bus.txt","r");
   int dem = 1;
    while(!(feof(fo)))
    {
       char buff1[20];
       char buff2[800];
       fgets(buff1,20,fo);
       //printf("%s\n",buff1 );
       fgets(buff2,800,fo);
       
       char *token = strtok(buff2,"-");
      
       
           if( (getId(graph,token)) == -1){
              addVertex(graph,dem,token);
              dem++;
             // printf("%d\n",dem );

           }
       while(token != NULL)
       {
           
           char *str1 = token; 
           
           token = strtok(NULL,"-");
           char *str2 = token;
           
           
           
           if(token != NULL)
           {
              int x = getId(graph,str1);


              int y = getId(graph,str2);

              if(x != -1 &&  y != -1)
                    addEdge(graph,x,y,1,buff1);
              else if(x != -1 && y == -1){
                    addVertex(graph,dem,str2);
                    addEdge(graph,x,dem,1,buff1);
                    dem++;
              
                }
            }
        }
    }


                    
           
}


int main(){
    Graph g = createGraph();
    FILE *fo = fopen("bus.txt","r");
        nhap_thog_so(g,fo);
    fclose(fo);
   
   int s, t, path[1200], length;
    s = 2;
    t = 100;
    double distance_s_t = shortestPath(g, s, t, path, &length);
    if (distance_s_t != INFINITIVE_VALUE){
        printf("Shortest distance from s --> t: %lf\n", distance_s_t);
        printf("Path: ");
        for (int i=length-1; i>=0; i--){
            printf("%s  ",getVertex(g,path[i]) );
            if(i-1 >=0)
            printf("use bus:%s",Conduong(g,path[i],path[i-1]) );
           

        }
        printf("\n");
    }else{
        printf("No Path from s --> t\n");
    }

    dropGraph(g);
    
}
char *Conduong(Graph graph,int v1,int v2)
{
    JRB node = jrb_find_int(graph.bus,v1);
    if(node == NULL) return NULL;
    else{
        JRB tree;
        tree = make_jrb();
        tree = (JRB)jval_v(node->val);
        JRB node2;
        node2 = jrb_find_int(tree,v2);
        if(node2 == NULL) return NULL;
        else return jval_s(node2->val);
    }
}
void addVertex(Graph graph, int id, char* name){
    JRB node = jrb_find_int(graph.vertices, id);
    if (node==NULL) // only add new vertex
        jrb_insert_int(graph.vertices, id, new_jval_s(strdup(name)));
}

char *getVertex(Graph graph, int id){
    JRB node = jrb_find_int(graph.vertices, id);
    if (node==NULL)
        return NULL;
    else
        return jval_s(node->val);
}

// ham dua ra id khi co name

int getId(Graph graph,char *name)
{
    JRB node;
    JRB buff;
    jrb_traverse(node,graph.vertices){
         if(strcmp(name,jval_s(node->val)) == 0) return (jval_i(node->key));
    }
    return -1;
}
// Make connection between v1 and v2
void addEdge(Graph graph, int v1, int v2,double weight,char *s){
    JRB node, tree,tree2;
    JRB node2;
    if (getEdgeValue(graph, v1, v2) == INFINITIVE_VALUE)    
    {
        node = jrb_find_int(graph.edges, v1);
        node2 = jrb_find_int(graph.bus,v1);
        
        if (node == NULL)
        {
            tree = make_jrb();
            tree2 = make_jrb();
            jrb_insert_int(graph.edges, v1, new_jval_v(tree));
            jrb_insert_int(graph.bus,v1,new_jval_v(tree2));
        }
        else
        {
            tree = (JRB)jval_v(node->val);
            tree2 = (JRB)jval_v(node2->val);
        }
        jrb_insert_int(tree, v2, new_jval_d(weight));
        jrb_insert_int(tree2,v2,new_jval_s(s));
    }
}

// Get weight from v1 to v2
double getEdgeValue(Graph graph, int v1, int v2){
    JRB node = jrb_find_int(graph.edges, v1);
    if (node == NULL)
    {
        return INFINITIVE_VALUE;
    }
    JRB tree = (JRB)jval_v(node->val);
    JRB node1 = jrb_find_int(tree, v2);
    if (node1 == NULL) {
        return INFINITIVE_VALUE;
    }
    return jval_d(node1->val);
}

// Liet ke nhung dinh {w} ma ton tai canh w-->v
int indegree(Graph graph, int v, int* output){
    JRB tree, node;
    int total = 0;
    jrb_traverse(node, graph.edges){
        tree = (JRB) jval_v(node->val);
        if (jrb_find_int(tree, v))
        {
            output[total] = jval_i(node->key);
            total++;
        }
    }
    return total;
}

// Liet ke nhung dinh {w} ma ton tai canh v-->w
int outdegree(Graph graph, int v, int* output){
    JRB tree, node;
    int total;
    node = jrb_find_int(graph.edges, v);
    if (node==NULL)
        return 0;
    tree = (JRB) jval_v(node->val);
    total = 0;
    jrb_traverse(node, tree){
        output[total] = jval_i(node->key);
        total++;
    }
    return total;
}

double shortestPath(Graph graph, int s, int t, int *path, int *length){
    // Khoi tao cac distance = 0
    double distance[1000], min;
    int previous[1000], u, visit[1000];
    
    for (int i=0; i<1000; i++){
        distance[i] = INFINITIVE_VALUE;
        visit[i] = 0;
        previous[i] = 0;
    }
    distance[s] = 0;
    previous[s] = s;
    visit[s] = 1;
    
    Dllist ptr, queue, node;
    queue = new_dllist();
    dll_append(queue, new_jval_i(s));
    
    // Duyet Queue
    while (!dll_empty(queue)){
        min = INFINITIVE_VALUE;
        dll_traverse(ptr, queue){
            // Lay ra min{distance}
            u = jval_i(ptr->val);
            if (min > distance[u]){
                min = distance[u];
                node = ptr;
            }
        }
        u = jval_i(node->val);
        visit[u] = 1;
        dll_delete_node(node);
        if (u == t) break;
        
        int output[100];
        int n = outdegree(graph, u, output);
        
        // Cap nhap distance cua tat ca cac dinh ma lien ke voi dinh min
        for (int i=0; i<n; i++){
            int v = output[i];
            double w = getEdgeValue(graph, u, v);
            if (distance[v] > distance[u] + w){
                distance[v] = distance[u] + w;
                previous[v] = u;
            }
            if (visit[v] == 0){
                dll_append(queue, new_jval_i(v));
            }
        }
    }
        
    // Truy vet lai de lay duong di tu s ---> t va luu trong path[]
    double distance_s_t = distance[t];
    int count=0;
    if (distance[t] != INFINITIVE_VALUE){
        path[count++] = t;
        while (t != s){
            t = previous[t];
            path[count++] = t;
        }
        *length = count;
    }
    return distance_s_t;
}

Graph createGraph(){
    Graph g;
    g.edges = make_jrb();
    g.vertices = make_jrb();
    g.bus = make_jrb();
    return g;
}

void dropGraph(Graph graph){
    jrb_free_tree(graph.edges);
    jrb_free_tree(graph.vertices);
    jrb_free_tree(graph.bus);
}

