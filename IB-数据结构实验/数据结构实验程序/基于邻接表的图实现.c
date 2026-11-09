#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 最大顶点数
#define MAX_VERTEX_NUM 20
// 最大管理图数量
#define MAX_GRAPH_NUM 10
// 状态码定义
#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0

typedef int status;   // 状态类型
typedef int KeyType;  // 顶点关键字类型

// 顶点数据结构：关键字 + 名称
typedef struct {
    KeyType key;
    char name[20];
} VertexType;

// 边结点结构（邻接表）
typedef struct ArcNode {
    int adjvex;              // 邻接点的索引
    struct ArcNode *nextarc; // 指向下一条边
} ArcNode;

// 顶点结构
typedef struct VNode {
    VertexType data;         // 顶点数据
    ArcNode *firstarc;       // 指向第一条依附该顶点的边
} VNode, AdjList[MAX_VERTEX_NUM];

// 图结构（邻接表）
typedef struct {
    AdjList vertices;        // 邻接表
    int vexnum, arcnum;      // 顶点数、边数
    char graph_name[20];     // 图名
} ALGraph;

// 图管理器：管理多张图
typedef struct {
    ALGraph graphs[MAX_GRAPH_NUM];
    int graph_count;         // 当前图数量
    int current_graph;       // 当前操作图下标
} GraphManager;

// ====================== 函数原型声明 ======================
void InitGraphManager(GraphManager *gm);
status AddGraph(GraphManager *gm, char graph_name[]);
status DeleteGraph(GraphManager *gm, char graph_name[]);
status SwitchGraph(GraphManager *gm, char graph_name[]);
void ListAllGraphs(GraphManager *gm);

status CreateGraph(ALGraph *G, VertexType V[], int n, KeyType VR[][2], int m);
status DestroyGraph(ALGraph *G);
int LocateVex(ALGraph *G, KeyType u);
status PutVex(ALGraph *G, KeyType old_key, KeyType new_key, char value[]); 
int FirstAdjVex(ALGraph *G, int u);
int NextAdjVex(ALGraph *G, int v, int w);
status InsertVex(ALGraph *G, VertexType v);
status DeleteVex(ALGraph *G, KeyType v);
status InsertArc(ALGraph *G, KeyType v, KeyType w);
status DeleteArc(ALGraph *G, KeyType v, KeyType w);
static void DFS(ALGraph *G, int v, int visited[], void (*visit)(VertexType));
status DFSTraverse(ALGraph *G, void (*visit)(VertexType));
status BFSTraverse(ALGraph *G, void (*visit)(VertexType));

void VerticesSetLessThanK(ALGraph *G, KeyType v, int k);
int ShortestPathLength(ALGraph *G, KeyType v, KeyType w);
int ConnectedComponentsNums(ALGraph *G);
status SaveGraph(ALGraph *G, char FileName[]);
status LoadGraph(ALGraph *G, char FileName[]);

void Visit(VertexType v);
void DisplayGraph(ALGraph *G);
void Menu(GraphManager *gm);

// ====================== 图管理器实现 ======================

// 初始化图管理器
void InitGraphManager(GraphManager *gm) {
    gm->graph_count = 0;
    gm->current_graph = -1;
}

// 添加新图
status AddGraph(GraphManager *gm, char graph_name[]) {
    if (gm->graph_count >= MAX_GRAPH_NUM) {
        printf("错误：最多只能管理%d个图！\n", MAX_GRAPH_NUM);
        return ERROR;
    }

    // 检查重名
    for (int i = 0; i < gm->graph_count; i++) {
        if (strcmp(gm->graphs[i].graph_name, graph_name) == 0) {
            printf("错误：图名\"%s\"已存在！\n", graph_name);
            return ERROR;
        }
    }

    strcpy(gm->graphs[gm->graph_count].graph_name, graph_name);
    gm->graphs[gm->graph_count].vexnum = 0;
    gm->graphs[gm->graph_count].arcnum = 0;
    
    gm->current_graph = gm->graph_count;
    gm->graph_count++;
    
    printf("成功添加图\"%s\"，已自动切换到该图\n", graph_name);
    return OK;
}

// 删除指定图
status DeleteGraph(GraphManager *gm, char graph_name[]) {
    int pos = -1;
    
    // 查找位置
    for (int i = 0; i < gm->graph_count; i++) {
        if (strcmp(gm->graphs[i].graph_name, graph_name) == 0) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        printf("错误：图\"%s\"不存在！\n", graph_name);
        return ERROR;
    }

    // 销毁图内存
    DestroyGraph(&gm->graphs[pos]);

    // 后面的图前移
    for (int i = pos; i < gm->graph_count - 1; i++) {
        gm->graphs[i] = gm->graphs[i + 1];
    }

    gm->graph_count--;

    // 调整当前图
    if (gm->current_graph == pos) {
        if (gm->graph_count > 0) {
            gm->current_graph = 0;
            printf("已删除当前图，自动切换到图\"%s\"\n", gm->graphs[0].graph_name);
        } else {
            gm->current_graph = -1;
            printf("已删除所有图，当前无激活图\n");
        }
    } else if (gm->current_graph > pos) {
        gm->current_graph--;
    }

    printf("成功删除图\"%s\"\n", graph_name);
    return OK;
}

// 切换当前操作图
status SwitchGraph(GraphManager *gm, char graph_name[]) {
    for (int i = 0; i < gm->graph_count; i++) {
        if (strcmp(gm->graphs[i].graph_name, graph_name) == 0) {
            gm->current_graph = i;
            printf("已切换到图\"%s\"\n", graph_name);
            return OK;
        }
    }

    printf("错误：图\"%s\"不存在！\n", graph_name);
    return ERROR;
}

// 列出所有图
void ListAllGraphs(GraphManager *gm) {
    if (gm->graph_count == 0) {
        printf("当前没有管理任何图\n");
        return;
    }

    printf("\n===== 所有图列表 =====\n");
    for (int i = 0; i < gm->graph_count; i++) {
        if (i == gm->current_graph) {
            printf("* %d. %s (当前操作图)\n", i + 1, gm->graphs[i].graph_name);
        } else {
            printf("  %d. %s\n", i + 1, gm->graphs[i].graph_name);
        }
    }
    printf("======================\n");
}

// ====================== 基础图操作 ======================

// 创建无向图
status CreateGraph(ALGraph *G, VertexType V[], int n, KeyType VR[][2], int m) {
    int i, j, k;
    ArcNode *p;

    G->vexnum = n;
    G->arcnum = 0;

    // 初始化顶点
    for (i = 0; i < n; i++) {
        G->vertices[i].data = V[i];
        G->vertices[i].firstarc = NULL;
    }

    // 建立边
    for (k = 0; k < m; k++) {
        int pos1 = -1, pos2 = -1;
        for (i = 0; i < n; i++) {
            if (G->vertices[i].data.key == VR[k][0]) pos1 = i;
            if (G->vertices[i].data.key == VR[k][1]) pos2 = i;
        }
        if (pos1 == -1 || pos2 == -1 || pos1 == pos2) continue;

        // 无向图双向插入
        p = (ArcNode*)malloc(sizeof(ArcNode));
        p->adjvex = pos2;
        p->nextarc = G->vertices[pos1].firstarc;
        G->vertices[pos1].firstarc = p;

        p = (ArcNode*)malloc(sizeof(ArcNode));
        p->adjvex = pos1;
        p->nextarc = G->vertices[pos2].firstarc;
        G->vertices[pos2].firstarc = p;

        G->arcnum++;
    }

    return OK;
}

// 销毁图（释放所有边结点）
status DestroyGraph(ALGraph *G) {
    int i;
    ArcNode *p, *q;

    for (i = 0; i < G->vexnum; i++) {
        p = G->vertices[i].firstarc;
        while (p) {
            q = p;
            p = p->nextarc;
            free(q);
        }
        G->vertices[i].firstarc = NULL;
    }

    G->vexnum = 0;
    G->arcnum = 0;
    return OK;
}

// 根据关键字查找顶点位置
int LocateVex(ALGraph *G, KeyType u) {
    int i;
    for (i = 0; i < G->vexnum; i++) {
        if (G->vertices[i].data.key == u) return i;
    }
    return -1;
}

// ====================== 修改后的顶点赋值函数 ======================
status PutVex(ALGraph *G, KeyType old_key, KeyType new_key, char value[]) {
    int pos = LocateVex(G, old_key);
    if (pos == -1) return ERROR; // 原顶点不存在

    // 检查新关键字是否已存在（排除自身）
    if (new_key != old_key) {
        for (int i = 0; i < G->vexnum; i++) {
            if (G->vertices[i].data.key == new_key) {
                return ERROR; // 新关键字重复
            }
        }
    }

    // 同时更新关键字和名称
    G->vertices[pos].data.key = new_key;
    strcpy(G->vertices[pos].data.name, value);
    return OK;
}

// 获取第一个邻接点
int FirstAdjVex(ALGraph *G, int u) {
    if (u < 0 || u >= G->vexnum) return -1;
    if (G->vertices[u].firstarc == NULL) return -1;
    return G->vertices[u].firstarc->adjvex;
}

// 获取下一个邻接点
int NextAdjVex(ALGraph *G, int v, int w) {
    ArcNode *p;
    if (v < 0 || v >= G->vexnum || w < 0 || w >= G->vexnum) return -1;
    p = G->vertices[v].firstarc;
    while (p) {
        if (p->adjvex == w) {
            if (p->nextarc) return p->nextarc->adjvex;
            else return -1;
        }
        p = p->nextarc;
    }
    return -1;
}

// 插入顶点
status InsertVex(ALGraph *G, VertexType v) {
    int i;
    if (G->vexnum >= MAX_VERTEX_NUM) return ERROR;
    // 检查关键字重复
    for (i = 0; i < G->vexnum; i++) {
        if (G->vertices[i].data.key == v.key) return ERROR;
    }
    G->vertices[G->vexnum].data = v;
    G->vertices[G->vexnum].firstarc = NULL;
    G->vexnum++;
    return OK;
}

// 删除顶点及所有关联边
status DeleteVex(ALGraph *G, KeyType v) {
    int i, j, pos;
    ArcNode *p, *q, *pre;

    pos = LocateVex(G, v);
    if (pos == -1) return ERROR;

    // 删除所有指向该顶点的边
    for (i = 0; i < G->vexnum; i++) {
        if (i == pos) continue;
        pre = NULL;
        p = G->vertices[i].firstarc;
        while (p) {
            if (p->adjvex == pos) {
                if (pre == NULL)
                    G->vertices[i].firstarc = p->nextarc;
                else
                    pre->nextarc = p->nextarc;
                q = p;
                p = p->nextarc;
                free(q);
                G->arcnum--;
            } else {
                if (p->adjvex > pos) p->adjvex--;
                pre = p;
                p = p->nextarc;
            }
        }
    }

    // 删除该顶点的所有边
    p = G->vertices[pos].firstarc;
    while (p) {
        q = p;
        p = p->nextarc;
        free(q);
    }

    // 顶点前移
    for (i = pos; i < G->vexnum-1; i++) {
        G->vertices[i] = G->vertices[i+1];
    }

    G->vexnum--;
    return OK;
}

// 插入边（无向图双向插入）
status InsertArc(ALGraph *G, KeyType v, KeyType w) {
    int pos1 = LocateVex(G, v);
    int pos2 = LocateVex(G, w);
    ArcNode *p;

    if (pos1 == -1 || pos2 == -1 || pos1 == pos2) return ERROR;

    // 检查边已存在
    p = G->vertices[pos1].firstarc;
    while (p) {
        if (p->adjvex == pos2) return ERROR;
        p = p->nextarc;
    }

    p = (ArcNode*)malloc(sizeof(ArcNode));
    p->adjvex = pos2;
    p->nextarc = G->vertices[pos1].firstarc;
    G->vertices[pos1].firstarc = p;

    p = (ArcNode*)malloc(sizeof(ArcNode));
    p->adjvex = pos1;
    p->nextarc = G->vertices[pos2].firstarc;
    G->vertices[pos2].firstarc = p;

    G->arcnum++;
    return OK;
}

// 删除边
status DeleteArc(ALGraph *G, KeyType v, KeyType w) {
    int pos1 = LocateVex(G, v);
    int pos2 = LocateVex(G, w);
    ArcNode *p, *pre, *q;
    int found = 0;

    if (pos1 == -1 || pos2 == -1 || pos1 == pos2) return ERROR;

    // 删除 v -> w
    pre = NULL;
    p = G->vertices[pos1].firstarc;
    while (p) {
        if (p->adjvex == pos2) {
            if (pre == NULL)
                G->vertices[pos1].firstarc = p->nextarc;
            else
                pre->nextarc = p->nextarc;
            q = p;
            p = p->nextarc;
            free(q);
            found = 1;
            break;
        }
        pre = p;
        p = p->nextarc;
    }

    if (!found) return ERROR;

    // 删除 w -> v
    pre = NULL;
    p = G->vertices[pos2].firstarc;
    while (p) {
        if (p->adjvex == pos1) {
            if (pre == NULL)
                G->vertices[pos2].firstarc = p->nextarc;
            else
                pre->nextarc = p->nextarc;
            q = p;
            p = p->nextarc;
            free(q);
            break;
        }
        pre = p;
        p = p->nextarc;
    }

    G->arcnum--;
    return OK;
}

// ====================== 遍历 ======================

// 深度优先遍历递归函数
static void DFS(ALGraph *G, int v, int visited[], void (*visit)(VertexType)) {
    ArcNode *p;
    int w;

    if (visit != NULL) {
        visit(G->vertices[v].data);
    }
    visited[v] = 1;

    p = G->vertices[v].firstarc;
    while (p) {
        w = p->adjvex;
        if (!visited[w]) {
            DFS(G, w, visited, visit);
        }
        p = p->nextarc;
    }
}

// 深度优先遍历
status DFSTraverse(ALGraph *G, void (*visit)(VertexType)) {
    int i;
    int *visited;

    visited = (int *)malloc(G->vexnum * sizeof(int));
    for (i = 0; i < G->vexnum; i++)
        visited[i] = 0;

    for (i = 0; i < G->vexnum; i++) {
        if (!visited[i])
            DFS(G, i, visited, visit);
    }

    free(visited);
    return OK;
}

// 广度优先遍历
status BFSTraverse(ALGraph *G, void (*visit)(VertexType)) {
    int i, u, w;
    int *visited;
    int *queue;
    int front, rear;
    ArcNode *p;

    visited = (int *)malloc(G->vexnum * sizeof(int));
    for (i = 0; i < G->vexnum; i++)
        visited[i] = 0;

    queue = (int *)malloc(G->vexnum * sizeof(int));
    front = rear = 0;

    for (i = 0; i < G->vexnum; i++) {
        if (!visited[i]) {
            visit(G->vertices[i].data);
            visited[i] = 1;
            queue[rear++] = i;

            while (front != rear) {
                u = queue[front++];
                p = G->vertices[u].firstarc;
                while (p != NULL) {
                    w = p->adjvex;
                    if (!visited[w]) {
                        visit(G->vertices[w].data);
                        visited[w] = 1;
                        queue[rear++] = w;
                    }
                    p = p->nextarc;
                }
            }
        }
    }

    free(visited);
    free(queue);
    return OK;
}

// ====================== 扩展功能 ======================

// 输出与顶点 v 距离小于 k 的所有顶点
void VerticesSetLessThanK(ALGraph *G, KeyType v, int k) {
    int i, u, w, dist;
    int *visited, *distance;
    int *queue;
    int front, rear;
    ArcNode *p;
    int pos = LocateVex(G, v);

    if (pos == -1 || k <= 0) {
        printf("输入错误！\n");
        return;
    }

    visited = (int *)malloc(G->vexnum * sizeof(int));
    distance = (int *)malloc(G->vexnum * sizeof(int));
    queue = (int *)malloc(G->vexnum * sizeof(int));
    front = rear = 0;

    for (i = 0; i < G->vexnum; i++) {
        visited[i] = 0;
        distance[i] = -1;
    }

    visited[pos] = 1;
    distance[pos] = 0;
    queue[rear++] = pos;

    printf("与顶点 %d(%s) 距离小于 %d 的顶点有：\n", v, G->vertices[pos].data.name, k);
    while (front != rear) {
        u = queue[front++];
        if (distance[u] < k-1) {
            p = G->vertices[u].firstarc;
            while (p) {
                w = p->adjvex;
                if (!visited[w]) {
                    visited[w] = 1;
                    distance[w] = distance[u] + 1;
                    printf("%d(%s) 距离：%d\n", 
                        G->vertices[w].data.key, 
                        G->vertices[w].data.name, 
                        distance[w]);
                    queue[rear++] = w;
                }
                p = p->nextarc;
            }
        }
    }

    free(visited);
    free(distance);
    free(queue);
}

// 求两点间最短路径长度（BFS）
int ShortestPathLength(ALGraph *G, KeyType v, KeyType w) {
    int i, u, x;
    int *visited, *distance;
    int *queue;
    int front, rear;
    ArcNode *p;
    int pos1 = LocateVex(G, v);
    int pos2 = LocateVex(G, w);

    if (pos1 == -1 || pos2 == -1) return -1;
    if (pos1 == pos2) return 0;

    visited = (int *)malloc(G->vexnum * sizeof(int));
    distance = (int *)malloc(G->vexnum * sizeof(int));
    queue = (int *)malloc(G->vexnum * sizeof(int));
    front = rear = 0;

    for (i = 0; i < G->vexnum; i++) {
        visited[i] = 0;
        distance[i] = -1;
    }

    visited[pos1] = 1;
    distance[pos1] = 0;
    queue[rear++] = pos1;

    while (front != rear) {
        u = queue[front++];
        p = G->vertices[u].firstarc;
        while (p) {
            x = p->adjvex;
            if (!visited[x]) {
                visited[x] = 1;
                distance[x] = distance[u] + 1;
                if (x == pos2) {
                    int len = distance[x];
                    free(visited);
                    free(distance);
                    free(queue);
                    return len;
                }
                queue[rear++] = x;
            }
            p = p->nextarc;
        }
    }

    free(visited);
    free(distance);
    free(queue);
    return -1;
}

// 求连通分量个数
int ConnectedComponentsNums(ALGraph *G) {
    int i, count = 0;
    int *visited;

    visited = (int *)malloc(G->vexnum * sizeof(int));
    for (i = 0; i < G->vexnum; i++)
        visited[i] = 0;

    for (i = 0; i < G->vexnum; i++) {
        if (!visited[i]) {
            count++;
            DFS(G, i, visited, NULL);
        }
    }

    free(visited);
    return count;
}

// ====================== 文件操作 ======================

// 保存图到文件
status SaveGraph(ALGraph *G, char FileName[]) {
    FILE *fp = fopen(FileName, "w");
    if (!fp) return ERROR;

    ArcNode *p;
    int i, j;
    int **visited;

    fprintf(fp, "%d %d\n", G->vexnum, G->arcnum);

    for (i = 0; i < G->vexnum; i++) {
        fprintf(fp, "%d %s\n", G->vertices[i].data.key, G->vertices[i].data.name);
    }

    // 避免重复保存无向边
    visited = (int**)malloc(G->vexnum * sizeof(int*));
    for (i = 0; i < G->vexnum; i++) {
        visited[i] = (int*)calloc(G->vexnum, sizeof(int));
    }

    for (i = 0; i < G->vexnum; i++) {
        p = G->vertices[i].firstarc;
        while (p) {
            j = p->adjvex;
            if (i < j && !visited[i][j]) {
                fprintf(fp, "%d %d\n", 
                    G->vertices[i].data.key, 
                    G->vertices[j].data.key);
                visited[i][j] = visited[j][i] = 1;
            }
            p = p->nextarc;
        }
    }

    for (i = 0; i < G->vexnum; i++) {
        free(visited[i]);
    }
    free(visited);

    fclose(fp);
    return OK;
}

// 从文件加载图
status LoadGraph(ALGraph *G, char FileName[]) {
    FILE *fp = fopen(FileName, "r");
    if (!fp) return ERROR;

    int i, j;
    int key1, key2;
    ArcNode *p;
    int vexnum, arcnum;

    fscanf(fp, "%d%d", &vexnum, &arcnum);
    G->vexnum = vexnum;
    G->arcnum = 0;

    for (i = 0; i < vexnum; i++) {
        fscanf(fp, "%d%s", &G->vertices[i].data.key, G->vertices[i].data.name);
        G->vertices[i].firstarc = NULL;
    }

    for (i = 0; i < arcnum; i++) {
        fscanf(fp, "%d%d", &key1, &key2);

        int pos1 = -1, pos2 = -1;
        for (j = 0; j < vexnum; j++) {
            if (G->vertices[j].data.key == key1) pos1 = j;
            if (G->vertices[j].data.key == key2) pos2 = j;
        }
        if (pos1 == -1 || pos2 == -1) continue;

        p = (ArcNode*)malloc(sizeof(ArcNode));
        p->adjvex = pos2;
        p->nextarc = G->vertices[pos1].firstarc;
        G->vertices[pos1].firstarc = p;

        p = (ArcNode*)malloc(sizeof(ArcNode));
        p->adjvex = pos1;
        p->nextarc = G->vertices[pos2].firstarc;
        G->vertices[pos2].firstarc = p;

        G->arcnum++;
    }

    fclose(fp);
    return OK;
}

// 访问顶点：输出信息
void Visit(VertexType v) {
    printf("%d %s ", v.key, v.name);
}

// 显示图邻接表
void DisplayGraph(ALGraph *G) {
    int i;
    ArcNode *p;

    printf("\n图\"%s\"的邻接表表示：\n", G->graph_name);
    for (i = 0; i < G->vexnum; i++) {
        printf("%d %s", G->vertices[i].data.key, G->vertices[i].data.name);
        p = G->vertices[i].firstarc;
        while (p) {
            printf(" %d", p->adjvex);
            p = p->nextarc;
        }
        printf("\n");
    }
}

// 菜单
void Menu(GraphManager *gm) {
    printf("\n============================== 无向图邻接表操作菜单 ==============================\n");
    printf("//单图操作（当前操作图：");
    if (gm->current_graph != -1) {
        printf("%s", gm->graphs[gm->current_graph].graph_name);
    } else {
        printf("无");
    }
    printf("）\n");
    printf("1. 创建图\t\t2. 销毁图\t\t3. 查找顶点\t\t4. 顶点赋值\n");
    printf("5. 获得第一邻接点\t6. 获得下一邻接点\t7. 插入顶点\t\t8. 删除顶点\n");
    printf("9. 插入弧\t\t10. 删除弧\t\t11. 深度优先遍历\t12. 广度优先遍历\n");
    printf("13. 距离小于k的顶点集合\t14. 顶点间最短路径长度\t15. 图的连通分量个数\t16. 保存图到文件\n");
    printf("17. 从文件加载图\t18. 显示图\n");
    printf("//多图管理\n");
    printf("19. 添加新图\t\t20. 删除图\t\t21. 切换当前图\t\t22. 显示所有图\n");
    printf("0. 退出\n");
    printf("=================================================================================\n");
    printf("请输入您的选择：");
}

// 主函数
int main() {
    GraphManager gm;
    InitGraphManager(&gm);
    int choice;
    KeyType key1, key2;
    char name[20], filename[50];
    int k, len, pos;
    VertexType v;

    // 默认创建一张图
    AddGraph(&gm, "默认图");

    while (1) {
        Menu(&gm);
        scanf("%d", &choice);
        printf("\n");

        // 未选择图时不能操作
        if (choice >= 1 && choice <= 18 && gm.current_graph == -1) {
            printf("错误：当前没有激活的图，请先添加新图！\n");
            continue;
        }

        ALGraph *current_G = &gm.graphs[gm.current_graph];

        switch (choice) {
            case 1: {
                int n, m, i;
                VertexType V[MAX_VERTEX_NUM];
                KeyType VR[MAX_VERTEX_NUM*MAX_VERTEX_NUM][2];

                printf("请输入顶点个数：");
                scanf("%d", &n);
                printf("请输入顶点信息（关键字 名称）：\n");
                for (i = 0; i < n; i++) {
                    scanf("%d%s", &V[i].key, V[i].name);
                }
                printf("请输入边的个数：");
                scanf("%d", &m);
                printf("请输入边信息（顶点1关键字 顶点2关键字）：\n");
                for (i = 0; i < m; i++) {
                    scanf("%d%d", &VR[i][0], &VR[i][1]);
                }

                if (CreateGraph(current_G, V, n, VR, m) == OK)
                    printf("图创建成功！\n");
                else
                    printf("图创建失败！\n");
                break;
            }
            case 2:
                DestroyGraph(current_G);
                printf("图销毁成功！\n");
                break;
            case 3:
                printf("请输入要查找的顶点关键字：");
                scanf("%d", &key1);
                pos = LocateVex(current_G, key1);
                if (pos != -1)
                    printf("顶点 %d(%s) 存在，位序为 %d\n", key1, current_G->vertices[pos].data.name, pos);
                else
                    printf("顶点不存在！\n");
                break;
            // ====================== 修改后的顶点赋值交互 ======================
            case 4:
                printf("请输入要赋值的顶点原关键字：");
                scanf("%d", &key1);
                KeyType new_key;
                printf("请输入新的顶点关键字：");
                scanf("%d", &new_key);
                printf("请输入新的顶点名称：");
                scanf("%s", name);
                if (PutVex(current_G, key1, new_key, name) == OK)
                    printf("顶点赋值成功！\n");
                else
                    printf("顶点赋值失败！（原关键字不存在或新关键字已存在）\n");
                break;
            case 5:
                printf("请输入顶点位序：");
                scanf("%d", &pos);
                int adj = FirstAdjVex(current_G, pos);
                if (adj != -1)
                    printf("第一个邻接点位序为 %d，关键字为 %d\n", adj, current_G->vertices[adj].data.key);
                else
                    printf("该顶点没有邻接点！\n");
                break;
            case 6:
                int v_pos, w_pos;
                printf("请输入顶点v的位序：");
                scanf("%d", &v_pos);
                printf("请输入邻接点w的位序：");
                scanf("%d", &w_pos);
                int next = NextAdjVex(current_G, v_pos, w_pos);
                if (next != -1)
                    printf("下一个邻接点位序为 %d，关键字为 %d\n", next, current_G->vertices[next].data.key);
                else
                    printf("没有下一个邻接点！\n");
                break;
            case 7:
                printf("请输入新顶点信息（关键字 名称）：");
                scanf("%d%s", &v.key, v.name);
                if (InsertVex(current_G, v) == OK)
                    printf("顶点插入成功！\n");
                else
                    printf("顶点插入失败！\n");
                break;
            case 8:
                printf("请输入要删除的顶点关键字：");
                scanf("%d", &key1);
                if (DeleteVex(current_G, key1) == OK)
                    printf("顶点删除成功！\n");
                else
                    printf("顶点删除失败！\n");
                break;
            case 9:
                printf("请输入边的两个顶点关键字：");
                scanf("%d%d", &key1, &key2);
                if (InsertArc(current_G, key1, key2) == OK)
                    printf("边插入成功！\n");
                else
                    printf("边插入失败！\n");
                break;
            case 10:
                printf("请输入要删除的边的两个顶点关键字：");
                scanf("%d%d", &key1, &key2);
                if (DeleteArc(current_G, key1, key2) == OK)
                    printf("边删除成功！\n");
                else
                    printf("边删除失败！\n");
                break;
            case 11:
                printf("深度优先遍历结果：");
                DFSTraverse(current_G, Visit);
                printf("\n");
                break;
            case 12:
                printf("广度优先遍历结果：");
                BFSTraverse(current_G, Visit);
                printf("\n");
                break;
            case 13:
                printf("请输入顶点关键字：");
                scanf("%d", &key1);
                printf("请输入距离k：");
                scanf("%d", &k);
                VerticesSetLessThanK(current_G, key1, k);
                break;
            case 14:
                printf("请输入两个顶点关键字：");
                scanf("%d%d", &key1, &key2);
                len = ShortestPathLength(current_G, key1, key2);
                if (len != -1)
                    printf("顶点 %d 和 %d 之间的最短路径长度为 %d\n", key1, key2, len);
                else
                    printf("两个顶点不可达！\n");
                break;
            case 15:
                printf("图的连通分量个数为：%d\n", ConnectedComponentsNums(current_G));
                break;
            case 16:
                printf("请输入文件名：");
                scanf("%s", filename);
                if (SaveGraph(current_G, filename) == OK)
                    printf("图保存成功！\n");
                else
                    printf("图保存失败！\n");
                break;
            case 17:
                printf("请输入文件名：");
                scanf("%s", filename);
                if (LoadGraph(current_G, filename) == OK)
                    printf("图加载成功！\n");
                else
                    printf("图加载失败！\n");
                break;
            case 18:
                DisplayGraph(current_G);
                break;
            case 19:
                printf("请输入新图名称：");
                scanf("%s", name);
                AddGraph(&gm, name);
                break;
            case 20:
                printf("请输入要删除的图名称：");
                scanf("%s", name);
                DeleteGraph(&gm, name);
                break;
            case 21:
                printf("请输入要切换的图名称：");
                scanf("%s", name);
                SwitchGraph(&gm, name);
                break;
            case 22:
                ListAllGraphs(&gm);
                break;
            case 0:
                for (int i = 0; i < gm.graph_count; i++) {
                    DestroyGraph(&gm.graphs[i]);
                }
                printf("程序退出！\n");
                return 0;
            default:
                printf("输入错误，请重新输入！\n");
        }
    }

    return 0;
}