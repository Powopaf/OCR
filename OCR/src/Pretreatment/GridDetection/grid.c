#include "grid.h"
#include "Shape/shape.h"
#include "Lib/Lib.h"
#include "List/Node.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Node** ReduceArray(Node** lst, int* size, int nsize)
{
    int count = nsize;
    if(nsize==0)
    {
        for(int i = 0; i<*size; i++)
        {
            if(lst[i]!=NULL)
            {
                count++;
            }
        }
    }

    
    Node** res = (Node**)malloc(count*sizeof(Node*));
    int k = 0;
    for(int i = 0; i<*size; i++)
    {
        if(lst[i]!=NULL)
        {
            res[k] = lst[i];
            k++;
        }
    }
    *size = count;
    free(lst);
    return res;
}


double FindLowestDist(Shape* s1, Shape* s2)
{
    int p1[4][2] = {
        {s1->Minj,s1->Mini},
        {s1->Minj,s1->Maxi},
        {s1->Maxj,s1->Mini},
        {s1->Maxj,s1->Maxi}
    };

    int p2[4][2] = {
        {s2->Minj,s2->Mini},
        {s2->Minj,s2->Maxi},
        {s2->Maxj,s2->Mini},
        {s2->Maxj,s2->Maxi}
    };

    double MinDist = distance(p1[0][0],p1[0][1],p2[0][0],p2[0][1]);
    for(int i = 0; i<4; i++)
    {
        for(int j = 0; j<4; j++)
        {
            double dist = distance(p1[i][0],p1[i][1],p2[j][0],p2[j][1]);
            if(dist<MinDist)
            {
                MinDist = dist;
            }
        }
    }

    return MinDist;

}

void AverageClusterSize(Node** cluster,double* avHeight, double* avWidth)
{
    if(cluster==NULL)
    {
        return;
    }
    int sumh = 0;
    int sumw = 0;
    int count = 0;
    Node* lst = *cluster;
    while(lst!=NULL)
    {
        Shape* data = lst->data;
        sumh+=data->h;
        sumw+=data->w;
        count++;
        lst = lst->next;
    }
    *avHeight = sumh/(double)count;
    *avWidth = sumw/(double)count;
}

void FindCluster(Node** visited, Node** cluster, Node** shapeList, Shape* shape)
{   
    if(!ContainsNode(*visited,shape))
    {
        Node* nv = NewNode(shape);
        Node* nc = NewNode(shape);
        AddNode(visited,nv);

        AddNode(cluster,nc);

        Node* current = *shapeList;
        while(current!=NULL)
        {
            if(shape->id!=current->data->id)
            {
                double seuil = 1.5;
                double h = shape->h;
                //double w = shape->w;
                double avHeight;
                double avWidth;
                AverageClusterSize(cluster,&avHeight,&avWidth);
                if(h<avHeight + seuil && h>avHeight - seuil && FindLowestDist(shape,current->data)<(avHeight+avWidth))
                {
                    FindCluster(visited,cluster,shapeList,current->data);
                }
            }
            current = current->next;
        }
    }
}

void ShapeFilter(Node** shapeList)
{
    Node* current = *shapeList;
    int sumH = 0;
    int sumW = 0;
    int sumL = 0;
    int count = 0;
    while (current != NULL)
    {
        sumH += current->data->h;
        sumW += current->data->w;
        sumL += current->data->Len;
        count++;
        current = current->next;
    }

    double averageH = (double)sumH / count;
    double averageW = (double)sumW / count;
    double averageL = (double)sumL / count;


    current = *shapeList;
    int i = 0;
    
    while (current != NULL)
    {
        Node* next = current->next;
        int h = current->data->h;
        int w = current->data->w;
        int l = current->data->Len;
        if (h > 5 * averageH ||
            h < 0.1 * averageH ||
            w > 5 * averageW ||
            w < 0.0 * averageW ||
            l > 5 * averageL ||
            l < 0.1 * averageL)
        {
            RemoveNode(shapeList, i);
        }
        else
        {
            i++;
        }
        current = next;
    }
}


Node** CreateCluster(Node** shapeList, int* size)
{
    int n = LenNode(shapeList);
    Node** clusterList = (Node**)malloc(n*sizeof(Node*));

    Node* c = *shapeList;
    Node* visited = NULL;
    int i = 1;
    int count = 0;
    while(c!=NULL)
    {
        Node* cluster = NULL;
        FindCluster(&visited,&cluster,shapeList,c->data);
        clusterList[i-1] = cluster;
        if(cluster!=NULL)
        {
            count++;
        }
        //
        i++;
        c = c->next;
    }
    FreeNodeList(&visited,0);
    *size = n; 
    return ReduceArray(clusterList,size, count);
}

int ListSum(Node* lst)
{
    int sum = 0;
    while(lst!=NULL)
    {
        sum+=lst->data->Len;
        lst = lst->next;
    }
    return sum;
}

void AdjustList(Node** lst) {
    if (*lst == NULL) {
        return;
    }

    int sumH = 0;
    Node* c = *lst;
    int count = 0;

    while (c != NULL) {
        sumH += c->data->h;
        count++;
        c = c->next;
    }

    double avH = sumH / (double)count;
    Node* prev = NULL;
    c = *lst;
    double seuil = 5.0;
    while (c != NULL) {
        if (c->data->h < avH-seuil) {
            Node* temp = c;
            if (prev == NULL) {
                *lst = c->next;
            } else {
                prev->next = c->next;
            }
            c = c->next;

            // Libérer correctement la mémoire
            free(temp);        // Libère le Node
        } else {
            prev = c;
            c = c->next;
        }

    }
}

void ClusterFilter(Node** clusterList, int size)
{
    if(size>2)
    {
        double sumSize = 0;
        for(int i = 0; i<size; i++)
        {
            sumSize+=ListSum(clusterList[i]);
        }
        double avSize = sumSize/(double)size;
        int k = 0;

        for(int i = 0; i<size; i++)
        {
            if(ListSum(clusterList[i])<avSize)
            {
                FreeNodeList(&clusterList[i],0);
                clusterList[i] = NULL;
            }
            else
            {
                k++;
            }
        }
        clusterList = ReduceArray(clusterList,&size,k);
    }
}


void ProcessGrid(SDL_Surface *surface) {
    int width = surface->w;
    int height = surface->h;
    int** Map;
    int** surf;
    
    SDL_LockSurface(surface);
    MallocMatrix(&Map, height, width);
    MallocMatrix(&surf, height, width);
    
    InitMatrix(surface,&Map,&surf);

    Node* shapeList = NULL;
    int id = 0;

    for(int j = 0; j < height; j++) 
    {
        for(int i = 0; i < width; i++) 
        {
            if (surf[j][i] == 255 && Map[j][i] == 0) {
                id++;
                Shape* s = CreateShape(id, j, i);
                FindShape(s, surf, Map, j, i, height, width);
                ComputeShape(s);

                if (IsShapeValid(surface, s)) 
                {
                    Node* n = NewNode(s);
                    AddNode(&shapeList, n);
                } 
                else 
                {
                    free(s);
                }
            }
        }
    }

    FreeMatrix(Map, height);
    FreeMatrix(surf, height);
    
    ShapeFilter(&shapeList);
    
    int size = 0;
    Node** clusterList = CreateCluster(&shapeList,&size);
    //Draw(surface, shapeList,255,255,0);
    
    ClusterFilter(clusterList,size);
    
    for(int i = 0; i<size; i++)
    {

        AdjustList(&clusterList[i]);
    }
    
    for(int i = 0; i<size; i++)
    {
        //d
        float hue = (i * 360.0 / size);
        int r = (int)(255 * (1 + sin(hue * 3.14 / 180)) / 2);
        int g = (int)(255 * (1 + sin((hue + 120) * 3.14 / 180)) / 2);
        int b = (int)(255 * (1 + sin((hue + 240) * 3.14 / 180)) / 2);
        Draw(surface,clusterList[i],r,g,b);
        //d
    }


    for(int i = 0; i<size; i++)
    {
        PrintNodeList(clusterList[i]," Cluster");
        FreeNodeList(&clusterList[i],0);
    }
    free(clusterList);

    FreeNodeList(&shapeList,1);

    SDL_UnlockSurface(surface);
}

