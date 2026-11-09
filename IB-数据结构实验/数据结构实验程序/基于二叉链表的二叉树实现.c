#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 宏定义状态常量
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define MAX_TREE 10  // 最多管理10棵二叉树

typedef int status;   // 状态类型
typedef int KeyType;  // 关键字类型（int）

// 二叉树结点数据域结构
typedef struct
{
    KeyType key;          // 关键字
    char others[20];      // 其他信息（字符串）
} TElemType;

// 二叉链表结点结构
typedef struct BiTNode
{
    TElemType data;       // 数据域
    struct BiTNode *lchild, *rchild;  // 左右孩子指针
} BiTNode, *BiTree;

int inputEnd = 0;  // 输入结束标记

// 多棵树管理（全局）
BiTree trees[MAX_TREE];       // 存储多棵树的根
char treeNames[MAX_TREE][20]; // 树名
int treeCnt = 0;              // 当前树数量
int curSel = -1;              // 当前选中的树下标

// 访问结点函数：输出关键字和附加信息
void Visit(BiTree p)
{
    printf("%d,%s ", p->data.key, p->data.others);
}

// 获取一棵树的最右结点（用于删除时调整结构）
BiTNode* GetRightMost(BiTree T)
{
    while (T && T->rchild)
        T = T->rchild;
    return T;
}

//==================== 二叉树创建（先序） ====================
void CreatePre(BiTree *T)
{
    if (inputEnd) {
        *T = NULL;
        return;
    }
    
    int key;
    char str[20];
    if (scanf("%d %s", &key, str) != 2) {
        inputEnd = 1;
        *T = NULL;
        return;
    }
    
    if (key == -1) {  // -1 表示输入结束
        inputEnd = 1;
        *T = NULL;
        return;
    }
    if (key == 0) {   // 0 表示空结点
        *T = NULL;
        return;
    }
    
    // 创建当前结点
    *T = (BiTree)malloc(sizeof(BiTNode));
    (*T)->data.key = key;
    strcpy((*T)->data.others, str);
    (*T)->lchild = NULL;
    (*T)->rchild = NULL;
    
    CreatePre(&(*T)->lchild);  // 递归创建左子树
    CreatePre(&(*T)->rchild);  // 递归创建右子树
}

// 创建二叉树（对外接口）
status CreateBiTree(BiTree *T)
{
    inputEnd = 0; 
    CreatePre(T);
    
    int key;
    char str[20];
    while (!inputEnd) {
        if (scanf("%d %s", &key, str) != 2) break;
        if (key == -1) {
            inputEnd = 1;
            break;
        }
    }
    return OK;
}

//==================== 销毁 & 清空 ====================
void DestroyTree(BiTree *T)
{
    if (!*T) return;
    DestroyTree(&(*T)->lchild);
    DestroyTree(&(*T)->rchild);
    free(*T);
    *T = NULL;
}

// 销毁二叉树
status DestroyBiTree(BiTree *T)
{
    DestroyTree(T);
    return OK;
}

// 清空二叉树（等同于销毁）
status ClearBiTree(BiTree *T)
{
    DestroyTree(T);
    return OK;
}

//==================== 树状态 ====================
// 判断是否为空树
status BiTreeEmpty(BiTree T)
{
    return T == NULL ? TRUE : FALSE;
}

// 求树深度
int BiTreeDepth(BiTree T)
{
    if (!T) return 0;
    int l = BiTreeDepth(T->lchild);
    int r = BiTreeDepth(T->rchild);
    return (l > r ? l : r) + 1;
}

//==================== 查找 & 修改 ====================
// 根据关键字查找结点
BiTNode* LocateNode(BiTree T, KeyType e)
{
    if (!T) return NULL;
    if (T->data.key == e) return T;
    BiTNode *p = LocateNode(T->lchild, e);
    if (p) return p;
    return LocateNode(T->rchild, e);
}

// 修改结点数据
status Assign(BiTree T, KeyType e, TElemType val)
{
    BiTNode *p = LocateNode(T, e);
    if (!p) return ERROR;
    if (val.key != e && LocateNode(T, val.key))
        return ERROR;
    p->data = val;
    return OK;
}

// 查找结点的父结点（非递归栈实现）
static BiTNode* FindParent(BiTree root, KeyType e, BiTree *parent)
{
    *parent = NULL;
    if (!root || root->data.key == e) return NULL;

    BiTree stack[1000];
    int top = -1;
    stack[++top] = root;

    while (top >= 0)
    {
        BiTree cur = stack[top--];
        
        if (cur->lchild)
        {
            if (cur->lchild->data.key == e)
            {
                *parent = cur;
                return cur->lchild;
            }
            stack[++top] = cur->lchild;
        }
        
        if (cur->rchild)
        {
            if (cur->rchild->data.key == e)
            {
                *parent = cur;
                return cur->rchild;
            }
            stack[++top] = cur->rchild;
        }
    }
    
    return NULL; 
}

// 获取兄弟结点
BiTNode* GetSibling(BiTree T, KeyType e)
{
    if (!T || T->data.key == e) return NULL;
    
    BiTree parent = NULL;
    BiTNode *cur = FindParent(T, e, &parent);
    
    if (!cur || !parent) return NULL;
    return (parent->lchild == cur) ? parent->rchild : parent->lchild;
}

//==================== 插入结点 ====================
status InsertNode(BiTree *T, KeyType e, int LR, TElemType c)
{
    if (LocateNode(*T, c.key)) return ERROR;
    BiTNode *newNode = (BiTree)malloc(sizeof(BiTNode));
    newNode->data = c;
    newNode->lchild = newNode->rchild = NULL;

    if (LR == -1)  // 作为新根
    {
        newNode->lchild = *T;
        *T = newNode;
        return OK;
    }
    
    BiTNode *fa = LocateNode(*T, e);
    if (!fa)
    {
        free(newNode);
        return ERROR;
    }
    
    if (LR == 0)  // 左孩子
    {
        newNode->lchild = fa->lchild;
        fa->lchild = newNode;
    }
    else  // 右孩子
    {
        newNode->rchild = fa->rchild;
        fa->rchild = newNode;
    }
    return OK;
}

//==================== 删除结点 ====================
status DeleteNode(BiTree *T, KeyType e)
{
    if (!*T) return ERROR;
    if ((*T)->data.key == e)
    {
        BiTNode *del = *T;
        if (!(*T)->lchild && !(*T)->rchild)  // 叶子结点
            *T = NULL;
        else if (!(*T)->lchild)  // 只有右孩子
            *T = (*T)->rchild;
        else if (!(*T)->rchild)  // 只有左孩子
            *T = (*T)->lchild;
        else  // 左右孩子都有
        {
            BiTree rightMost = GetRightMost((*T)->lchild);
            rightMost->rchild = (*T)->rchild;
            *T = (*T)->lchild;
        }
        free(del);
        return OK;
    }
    if (DeleteNode(&(*T)->lchild, e)) return OK;
    return DeleteNode(&(*T)->rchild, e);
}

//==================== 四种遍历 ====================
// 先序遍历（非递归栈）
status PreOrderTraverse(BiTree T, void (*visit)(BiTree))
{
    if (!T) return OK;
    BiTree stack[1000];
    int top = -1;
    stack[++top] = T;
    while (top >= 0)
    {
        BiTree p = stack[top--];
        visit(p);
        if (p->rchild) stack[++top] = p->rchild;
        if (p->lchild) stack[++top] = p->lchild;
    }
    return OK;
}

// 中序遍历（递归）
status InOrderTraverse(BiTree T, void (*visit)(BiTree))
{
    if (!T) return OK;
    InOrderTraverse(T->lchild, visit);
    visit(T);
    InOrderTraverse(T->rchild, visit);
    return OK;
}

// 后序遍历（递归）
status PostOrderTraverse(BiTree T, void (*visit)(BiTree))
{
    if (!T) return OK;
    PostOrderTraverse(T->lchild, visit);
    PostOrderTraverse(T->rchild, visit);
    visit(T);
    return OK;
}

// 层序遍历（队列）
status LevelOrderTraverse(BiTree T, void (*visit)(BiTree))
{
    if (!T) return OK;
    BiTree que[1000];
    int front = 0, rear = 0;
    que[rear++] = T;
    while (front < rear)
    {
        BiTree p = que[front++];
        visit(p);
        if (p->lchild) que[rear++] = p->lchild;
        if (p->rchild) que[rear++] = p->rchild;
    }
    return OK;
}

//==================== 扩展功能 ====================
// 求根到叶子的最大路径和
int GetMaxPath(BiTree T)
{
    if (!T) return 0;
    int left = GetMaxPath(T->lchild);
    int right = GetMaxPath(T->rchild);
    return (left > right ? left : right) + T->data.key;
}

int MaxPathSum(BiTree T)
{
    return GetMaxPath(T);
}

// 最近公共祖先 LCA
BiTNode* LowestCommonAncestor(BiTree T, KeyType e1, KeyType e2)
{
    if (!T) return NULL;
    if (T->data.key == e1 || T->data.key == e2)
        return T;
    BiTNode *left = LowestCommonAncestor(T->lchild, e1, e2);
    BiTNode *right = LowestCommonAncestor(T->rchild, e1, e2);
    if (left && right) return T;
    return left ? left : right;
}

// 翻转二叉树
status InvertTree(BiTree T)
{
    if (!T) return OK;
    BiTree temp = T->lchild;
    T->lchild = T->rchild;
    T->rchild = temp;
    InvertTree(T->lchild);
    InvertTree(T->rchild);
    return OK;
}

//==================== 文件保存与读取 ====================
// 先序写入文件
void SaveWrite(BiTree T, FILE *fp)
{
    if (!T)
    {
        fprintf(fp, "0 null ");
        return;
    }
    fprintf(fp, "%d %s ", T->data.key, T->data.others);
    SaveWrite(T->lchild, fp);
    SaveWrite(T->rchild, fp);
}

// 保存树到文件
status SaveBiTree(BiTree T, const char fname[])
{
    FILE *fp = fopen(fname, "w");
    if (!fp) return ERROR;
    SaveWrite(T, fp);
    fprintf(fp, "-1 end");
    fclose(fp);
    return OK;
}

// 从文件读取建树
void FileReadCreate(BiTree *T, FILE *fp)
{
    if (inputEnd) {
        *T = NULL;
        return;
    }
    
    int k;
    char s[20];
    if (fscanf(fp, "%d %s", &k, s) != 2) {
        inputEnd = 1;
        *T = NULL;
        return;
    }
    
    if (k == -1) {
        inputEnd = 1;
        *T = NULL;
        return;
    }
    if (k == 0) {
        *T = NULL;
        return;
    }
    
    *T = (BiTree)malloc(sizeof(BiTNode));
    (*T)->data.key = k;
    strcpy((*T)->data.others, s);
    (*T)->lchild = NULL;
    (*T)->rchild = NULL;
    
    FileReadCreate(&(*T)->lchild, fp);
    FileReadCreate(&(*T)->rchild, fp);
}

// 加载树
status LoadBiTree(BiTree *T, const char fname[])
{
    FILE *fp = fopen(fname, "r");
    if (!fp) return ERROR;
    
    inputEnd = 0;
    FileReadCreate(T, fp);
    
    int k;
    char s[20];
    while (!inputEnd) {
        if (fscanf(fp, "%d %s", &k, s) != 2) break;
        if (k == -1) {
            inputEnd = 1;
            break;
        }
    }
    
    fclose(fp);
    return OK;
}

//==================== 菜单 ====================
void ShowMenu()
{
    printf("\n================================二叉链表二叉树操作系统================================\n");
    printf("0.退出程序\t1.创建树\t2.切换二叉树\t3.销毁当前树\n");
    printf("4.清空当前树\t5.判断空树\t6.求树深度\t7.关键字查找结点\n");
    printf("8.修改结点数据\t9.查询兄弟结点\t10.插入结点\t11.删除结点\n");
    printf("12.先序遍历\t13.中序遍历\t14.后序遍历\t15.层序遍历\t16.根叶最大路径和\n");
    printf("17.最近公共祖先\t18.翻转二叉树\t19.保存二叉树到文件\t\t20.从文件读取二叉树\n");
    printf("======================================================================================\n");
    printf("请选择功能：");
}

//==================== 主函数 ====================
int main()
{
    int opt;
    KeyType k1, k2;
    TElemType nodeVal;
    int lrFlag;
    char fileStr[30];

    while (1)
    {
        ShowMenu();
        scanf("%d", &opt);
        if (opt == 0) break;

        // 未选择树时不能操作
        if (curSel == -1 && opt >= 3)
        {
            printf("请先创建二叉树！\n");
            continue;
        }
        BiTree nowTree = trees[curSel];

        switch (opt)
        {
            case 1:  // 创建树
                if (treeCnt >= MAX_TREE)
                {
                    printf("二叉树数量已满！\n");
                    break;
                }
                printf("请输入先序序列(0空结点，-1结束)：\n");
                CreateBiTree(&trees[treeCnt]);
                printf("输入二叉树名称：");
                scanf("%s", treeNames[treeCnt]);
                curSel = treeCnt;
                treeCnt++;
                printf("创建成功！\n");
                break;
                
            case 2:  // 切换树
                if (treeCnt == 0) {
                    printf("暂无二叉树可切换！\n");
                    break;
                }
                printf("已有二叉树列表：\n");
                for (int i = 0; i < treeCnt; i++)
                    printf("%d -> %s\n", i, treeNames[i]);
                printf("选择序号：");
                scanf("%d", &curSel);
                if (curSel < 0 || curSel >= treeCnt) {
                    printf("无效序号，切换失败！\n");
                    curSel = (treeCnt > 0) ? 0 : -1;
                }
                break;
                
            case 3: DestroyBiTree(&trees[curSel]); printf("已彻底销毁\n"); break;
            case 4: ClearBiTree(&trees[curSel]); printf("已清空为空树\n"); break;
            case 5: printf(BiTreeEmpty(nowTree) ? "是空树\n" : "非空树\n"); break;
            case 6: printf("二叉树深度：%d\n", BiTreeDepth(nowTree)); break;
            case 7:  // 查找结点
                printf("输入查找关键字：");
                scanf("%d", &k1);
                BiTNode *foundNode = LocateNode(nowTree, k1);
                if (foundNode) {
                    printf("找到结点：%d,%s\n", foundNode->data.key, foundNode->data.others);
                } else {
                    printf("不存在该结点\n");
                }
                break;
                
            case 8:  // 修改结点
                printf("输入原关键字：");
                scanf("%d", &k1);
                printf("输入新关键字 新内容：");
                scanf("%d %s", &nodeVal.key, nodeVal.others);
                printf(Assign(nowTree, k1, nodeVal) ? "修改成功\n" : "修改失败\n");
                break;
                
            case 9:  // 查兄弟
                printf("输入关键字：");
                scanf("%d", &k1);
                BiTNode *sib = GetSibling(nowTree, k1);
                if (sib)
                    printf("兄弟结点：%d,%s\n", sib->data.key, sib->data.others);
                else
                    printf("无兄弟结点\n");
                break;
                
            case 10:  // 插入结点
                printf("父结点关键字 插入位置(-1根/0左/1右)：");
                scanf("%d %d", &k1, &lrFlag);
                printf("新结点关键字 内容：");
                scanf("%d %s", &nodeVal.key, nodeVal.others);
                if (InsertNode(&nowTree, k1, lrFlag, nodeVal))
                {
                    trees[curSel] = nowTree;
                    printf("插入成功\n");
                }
                else
                    printf("插入失败\n");
                break;
                
            case 11:  // 删除结点
                printf("输入删除关键字：");
                scanf("%d", &k1);
                if (DeleteNode(&nowTree, k1))
                {
                    trees[curSel] = nowTree;
                    printf("删除成功\n");
                }
                else
                    printf("删除失败\n");
                break;
                
            case 12:printf("先序遍历：");PreOrderTraverse(nowTree,Visit);printf("\n");break;
            case 13:printf("中序遍历：");InOrderTraverse(nowTree,Visit);printf("\n");break;
            case 14:printf("后序遍历：");PostOrderTraverse(nowTree,Visit);printf("\n");break;
            case 15:printf("层序遍历：");LevelOrderTraverse(nowTree,Visit);printf("\n");break;
            case 16:printf("根到叶子最大路径和：%d\n", MaxPathSum(nowTree));break;
            
            case 17:  // 最近公共祖先
                printf("输入两个关键字：");
                scanf("%d %d", &k1, &k2);
                BiTNode *anc = LowestCommonAncestor(nowTree, k1, k2);
                if (anc)
                    printf("最近公共祖先：%d,%s\n", anc->data.key, anc->data.others);
                else
                    printf("无公共祖先\n");
                break;
                
            case 18: InvertTree(nowTree); printf("二叉树翻转完成\n"); break;
            case 19:  // 保存到文件
                printf("输入保存文件名：");
                scanf("%s", fileStr);
                printf(SaveBiTree(nowTree, fileStr) ? "保存成功\n" : "保存失败\n");
                break;
                
            case 20:  // 从文件加载
                if (treeCnt >= MAX_TREE) {
                    printf("二叉树数量已满！\n");
                    break;
                }
                printf("输入读取文件名：");
                scanf("%s", fileStr);
                BiTree readT = NULL;
                if (LoadBiTree(&readT, fileStr))
                {
                    printf("输入树名：");
                    scanf("%s", treeNames[treeCnt]);
                    trees[treeCnt++] = readT;
                    curSel = treeCnt - 1;
                    printf("读取成功\n");
                }
                else
                    printf("读取失败\n");
                break;
                
            default:
                printf("无效选项，请重新输入\n");
        }
    }
    printf("程序正常退出\n");
    return 0;
}