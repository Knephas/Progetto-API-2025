#include <stdio.h>
#include <math.h>
#include <stdbool.h> 
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#define MAX_RIGA 256

int y_mappa = -1; int x_mappa = -1; bool mappainit = false;

struct Esagono_dest{
        int y, x;
};

struct Rotte{
    int costi[5];
    struct Esagono_dest esagoni_destinazione[5];
};

struct Esagono{
    int costo;
    struct Rotte rotte;
};

struct Esagono** mappa = NULL; 

struct esagonoHeap{
    int costo;
    int y;
    int x;
};

struct minHeap{
    struct esagonoHeap* esagonoHeap;
    int len;
    int max_grandezza;
};

struct minHeap heap;

void nuovaHeap(int max){
    if(heap.esagonoHeap != NULL) free(heap.esagonoHeap);

    heap.esagonoHeap = malloc(sizeof(struct esagonoHeap) * max);
    if(heap.esagonoHeap == NULL) {printf("KO"); return;}

    heap.len = 0;
    heap.max_grandezza = max;
}

int parent(int i){
    return (i - 1) / 2;
}

int sinistra(int i){
    return 2 * i + 1;
}

int destra(int i){
    return 2 * i + 2;
}

void min_Heapify(int i){
    int l = sinistra(i);
    int r = destra(i);

    int min = i;

    if(l < heap.len && heap.esagonoHeap[l].costo < heap.esagonoHeap[i].costo){
        min = l;
    }

    if(r < heap.len && heap.esagonoHeap[r].costo < heap.esagonoHeap[min].costo){
        min = r;
    }

    if(min != i){
        struct esagonoHeap t = heap.esagonoHeap[i];
        heap.esagonoHeap[i] = heap.esagonoHeap[min];
        heap.esagonoHeap[min] = t;

        min_Heapify(min);
    }
}

void heapInsert(int costo, int y, int x){
    if(heap.len < heap.max_grandezza){
        heap.esagonoHeap[heap.len].costo = costo;
        heap.esagonoHeap[heap.len].y = y;
        heap.esagonoHeap[heap.len].x = x;
        heap.len++;

        int i = heap.len - 1; 
        while(i > 0 && heap.esagonoHeap[parent(i)].costo > heap.esagonoHeap[i].costo){
            struct esagonoHeap t = heap.esagonoHeap[i];
            heap.esagonoHeap[i] = heap.esagonoHeap[parent(i)];
            heap.esagonoHeap[parent(i)] = t;
            i = parent(i);
        }
    }else{ 
        printf("KO\n"); 
    }
}

struct esagonoHeap estrazioneMin(){
    if(heap.len != 0){
        struct esagonoHeap min = heap.esagonoHeap[0];
        heap.esagonoHeap[0] = heap.esagonoHeap[heap.len - 1];
        heap.len--;

        min_Heapify(0);
        return min;
    }
    
    struct esagonoHeap err;
    err.costo = -1;
    err.x = -1;
    err.y = -1;
    return err;
}

int travel_cost(int xp, int yp, int xd, int yd){
    if(!mappainit) return -1;

    if(yp >= y_mappa || yp < 0 || xp >= x_mappa || xp < 0 || yd >= y_mappa || yd < 0 || xd >= x_mappa || xd < 0 ){
        return -1;
    }
    
    if(yp == yd && xp == xd){ 
            return 0;
    }

    if(mappa[yp][xp].costo == 0){
        return -1;
    }

    nuovaHeap(x_mappa * y_mappa);

    int mat[y_mappa][x_mappa];
    bool visitato[y_mappa][x_mappa];
    for(int i = 0; i < y_mappa; i++){
        for(int j = 0; j < x_mappa; j++){
            mat[i][j] = INT_MAX;
        }
    }

    memset(visitato, 0, sizeof(bool) * y_mappa * x_mappa);

    mat[yp][xp] =  0;
    heapInsert(0, yp, xp);

    int dx_pari[6] = {-1,  0,  1,  0, -1, -1};    
    int dy_pari[6] = { 1,  1,  0, -1, -1,  0};

    int dx_dispari[6] = { 0,  1,  1,  1,  0, -1}; 
    int dy_dispari[6] = { 1,  1,  0, -1, -1,  0};

    int i = 0; int j = 0;
    while(heap.len > 0){
        struct esagonoHeap k = estrazioneMin();

        i = k.y;
        j = k.x;

        struct Esagono *nodo = &mappa[i][j];

        if (k.costo != mat[i][j]) {
            continue;
        }

        if (visitato[i][j]) {
            continue;
        }
        visitato[i][j] = true;

        if (i == yd && j == xd) {
            break;
        }

        int *dx, *dy;
        if(i % 2 == 0){
            dx = dx_pari;
            dy = dy_pari;
        }else{
            dx = dx_dispari;
            dy = dy_dispari;
        }

        for(int h = 0; h < 6; h++){

            int nuovo_i = i + dy[h];
            int nuovo_j = j + dx[h];

            if(nuovo_j >= 0 && nuovo_j < x_mappa && nuovo_i >= 0 && nuovo_i < y_mappa){
                if (nodo->costo == 0) continue;
                
                int costo_effettivo_da_sommare = mat[i][j] + nodo->costo;
                if(!visitato[nuovo_i][nuovo_j] && costo_effettivo_da_sommare < mat[nuovo_i][nuovo_j]){
                    mat[nuovo_i][nuovo_j] = costo_effettivo_da_sommare;
                    heapInsert(costo_effettivo_da_sommare, nuovo_i, nuovo_j);
                }
            }
        }

        if (nodo->costo != 0) { 
            for(int h = 0; h < 5; h++){
                int y_D = nodo->rotte.esagoni_destinazione[h].y;
                int x_D = nodo->rotte.esagoni_destinazione[h].x;
                int costo_D = nodo->rotte.costi[h];

                if(y_D != -1 && x_D != -1 && costo_D > 0){
                    if(y_D >= 0 && y_D < y_mappa && x_D >= 0 && x_D < x_mappa){
                        int costo_effettivo_aereo = mat[i][j] + costo_D;
                        if(costo_effettivo_aereo < mat[y_D][x_D]){
                            mat[y_D][x_D] = costo_effettivo_aereo;
                            heapInsert(costo_effettivo_aereo, y_D, x_D);
                        }
                    }
                }
            }
        }
    }

    if (mat[yd][xd] == INT_MAX) {
        return -1;
    }
    return mat[yd][xd];
}

void init(int colonne, int righe){
    if(colonne <= 0 || righe <= 0){ 
        printf("KO\n"); 
        return;
    }

    if (mappa != NULL) {
        for (int i = 0; i < y_mappa; i++) {
            free(mappa[i]);
        }
    free(mappa);
    mappa = NULL;
    }

    mappa = malloc(sizeof(struct Esagono*) * righe);

    for (int i = 0; i < righe; i++) {
       mappa[i] = malloc(sizeof(struct Esagono) * colonne);
       memset(mappa[i], 0, sizeof(struct Esagono) * colonne);
    }

    for(int i = 0; i < righe; i++){
        for(int j = 0; j < colonne; j++){
            mappa[i][j].costo = 1;
        
            for(int k = 0; k < 5; k++){
                mappa[i][j].rotte.esagoni_destinazione[k].x = -1;
                mappa[i][j].rotte.esagoni_destinazione[k].y = -1;
                mappa[i][j].rotte.costi[k] = -1;
            }
        }
    }
       
    y_mappa = righe;
    x_mappa = colonne;

    nuovaHeap(righe * colonne);

    mappainit = true;

    printf("OK\n");
    
}

void toggle_air_route(int x1, int y1, int x2, int y2){
    if(y1 >= y_mappa || y1 < 0 || x1 < 0 || x1 >= x_mappa){
        printf("KO\n");
        return;
    }

    if(y2 >= y_mappa || y2 < 0 || x2 < 0 || x2 >= x_mappa){
        printf("KO\n");
        return;
    }

    for(int i = 0; i < 5; i++){
            if(mappa[y1][x1].rotte.esagoni_destinazione[i].y == y2 && mappa[y1][x1].rotte.esagoni_destinazione[i].x == x2){
                mappa[y1][x1].rotte.esagoni_destinazione[i].y = -1;
                mappa[y1][x1].rotte.esagoni_destinazione[i].x = -1;
                mappa[y1][x1].rotte.costi[i] = -1;
                printf("OK\n");
                return;
            }
        }

    for(int i = 0; i < 5; i++){
        if(mappa[y1][x1].rotte.esagoni_destinazione[i].y == -1 && mappa[y1][x1].rotte.esagoni_destinazione[i].x == -1){
            mappa[y1][x1].rotte.esagoni_destinazione[i].y = y2;
            mappa[y1][x1].rotte.esagoni_destinazione[i].x = x2;

            int tot = 0; int c = 0;
            for(int j = 0; j < 5; j++){
                if(mappa[y1][x1].rotte.costi[j] >= 0 && j != i){
                    tot += mappa[y1][x1].rotte.costi[j];
                    c++;    
                }
            }

            if(mappa[y1][x1].rotte.costi[i] < 0){
                mappa[y1][x1].rotte.costi[i] = 0;
            }

            if(mappa[y1][x1].rotte.costi[i] > 100){
                mappa[y1][x1].rotte.costi[i] = 100;
            }

            int num = tot + mappa[y1][x1].costo;
            int den = c + 1;

            if (den > 0) { 
                mappa[y1][x1].rotte.costi[i] = num / den; 
            } else {
                mappa[y1][x1].rotte.costi[i] = mappa[y1][x1].costo; 
            }

            printf("OK\n");
            return;
        }
    }
    printf("KO\n");
}


void conversione_coordinate(int x_offset, int y_offset, int *xcubo, int *ycubo, int *zcubo) {
    int c;
    if(y_offset % 2 == 0)
        c = 0;
    else
        c = 1;

    int cx = x_offset - (y_offset - c) / 2;
    int cz = y_offset;
    int cy = -cx - cz;

    *xcubo = cx;
    *ycubo = cy;
    *zcubo = cz;
}

int DistEsagoni(int y1, int x1, int y2, int x2) {
    int cx1, cy1, cz1;
    int cx2, cy2, cz2;

    conversione_coordinate(x1, y1, &cx1, &cy1, &cz1);
    conversione_coordinate(x2, y2, &cx2, &cy2, &cz2);

    int dx = abs(cx1 - cx2);
    int dy = abs(cy1 - cy2);
    int dz = abs(cz1 - cz2);

    int maxDist = dx;
    if (dy > maxDist) maxDist = dy;
    if (dz > maxDist) maxDist = dz;

    return maxDist;
}

void change_cost(int x, int y, int v, int r){
    if (r <= 0) { 
        printf("KO\n"); 
        return;
    }

    if(v > 10 || v < -10){
        printf("KO\n"); 
        return;
    }

    if(y < 0 || y >= y_mappa || x < 0 || x >= x_mappa ) { printf("KO\n"); return;} 
    
    for(int i = y - r; i <= y + r; i++){
        for(int j = x - r; j <= x + r; j++){
            struct Esagono *nodo = &mappa[i][j];
            if(i < 0 || i >= y_mappa || j < 0 || j >= x_mappa){
                continue; 
            }

            int distanza = DistEsagoni(y, x, i, j);
            if (distanza < 0 || distanza >= r) continue;

            
            int k = floor((v * (r - distanza)) / (double)r);
            nodo->costo += k;
            
            if (nodo->costo < 0) nodo->costo = 0;
            if (nodo->costo > 100) nodo->costo = 100;

            for(int l = 0; l < 5; l++){
                if(nodo->rotte.esagoni_destinazione[l].x != -1 && nodo->rotte.esagoni_destinazione[l].y != -1){
                    nodo->rotte.costi[l] += k;
                
                    if(nodo->rotte.costi[l] < 0){
                        nodo->rotte.costi[l] = 0;
                    }

                    if(nodo->rotte.costi[l] > 100){
                        nodo->rotte.costi[l] = 100;
                    }
                }
            }
        }
    }
    
    printf("OK\n");
}



int main(){

    heap.esagonoHeap = NULL;

    char riga[MAX_RIGA];
    while(fgets(riga, MAX_RIGA, stdin) != NULL){
        char comando[MAX_RIGA];
        int vals[4] = {0};

        int argomenti = sscanf(riga, "%s %d %d %d %d", comando, &vals[0], &vals[1], &vals[2], &vals[3]);
        
        if (strcmp(comando, "init") == 0 && argomenti == 3) {
            init(vals[0], vals[1]);
        }else if (strcmp(comando, "change_cost") == 0 && argomenti == 5) {
            change_cost(vals[0], vals[1], vals[2], vals[3]);
        }else if (strcmp(comando, "travel_cost") == 0 && argomenti == 5) {
            int res = travel_cost(vals[0], vals[1], vals[2], vals[3]);
            printf("%d\n", res);
        } 
        else if (strcmp(comando, "toggle_air_route") == 0 && argomenti == 5) {
            toggle_air_route(vals[0], vals[1], vals[2], vals[3]);
        } 
        else {
            printf("KO\n");
        } 
    }

    if (heap.esagonoHeap != NULL) {
        free(heap.esagonoHeap);
    }

    if (mappa != NULL) {
        for (int i = 0; i < y_mappa; i++) {
            free(mappa[i]);
        }
    free(mappa);
    }

    return 0;
}