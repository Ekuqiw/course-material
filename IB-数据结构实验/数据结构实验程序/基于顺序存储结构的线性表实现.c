#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 宏定义：状态码，用于函数返回值表示操作结果
#define TRUE        1       // 真
#define FALSE       0       // 假
#define OK          1       // 操作成功
#define ERROR       0       // 操作出错
#define INFEASIBLE  -1      // 操作不可行（如未初始化）
#define OVERFLOW    -2      // 内存溢出

typedef int status;         // 状态类型，用于返回函数执行状态
typedef int ElemType;       // 数据元素类型（这里设为int）

// 顺序表初始分配容量与增量
#define LIST_INIT_SIZE 100  // 初始容量
#define LISTINCREMENT  10   // 扩容增量

// 顺序表结构定义
typedef struct{
    ElemType *elem;         // 指向数据存储空间的基地址
    int length;             // 当前表中元素个数
    int listsize;           // 当前分配的存储容量
}SqList;

// 多线性表管理结构（可同时管理多张顺序表）
typedef struct{
    struct {
        char name[30];      // 表名
        SqList L;           // 对应的顺序表
    } elem[10];             // 最多管理10张表
    int length;             // 当前管理表的数量
}LISTS;

LISTS Lists;                 // 全局多表管理器
int current = 0;             // 当前正在操作的表的下标

//======================== 函数声明 ================================
// 基础顺序表操作
status InitList(SqList *L);                     // 初始化顺序表
status DestroyList(SqList *L);                   // 销毁顺序表
status ClearList(SqList *L);                     // 清空表
status ListEmpty(SqList L);                      // 判断是否为空表
status ListLength(SqList L);                     // 获取表长度
status GetElem(SqList L,int i,ElemType *e);     // 获取第i个元素
int LocateElem(SqList L,ElemType e);             // 查找元素e第一次出现的位置
status PriorElem(SqList L,ElemType e,ElemType *pre_e);    // 求前驱
status NextElem(SqList L,ElemType e,ElemType *next_e);    // 求后继
status ListInsert(SqList *L,int i,ElemType e);   // 在第i个位置插入e
status ListDelete(SqList *L,int i,ElemType *e);  // 删除第i个元素
status ListTraverse(SqList L);                   // 遍历输出表

// 扩展功能
int MaxSubArray(SqList L);                       // 最大子数组和（Kadane算法）
int SubArrayNum(SqList L,int k);                 // 和为k的子数组数量
void sortList(SqList *L);                        // 冒泡排序
status SaveList(SqList L,char FileName[]);       // 保存到文件
status LoadList(SqList *L,char FileName[]);      // 从文件加载

// 多表管理
status AddList(LISTS *Lists,char ListName[]);    // 新建一张表
status RemoveList(LISTS *Lists,char ListName[]);  // 删除一张表
int LocateList(LISTS Lists,char ListName[]);      // 按表名查找
void ShowAllLists(LISTS Lists);                  // 显示所有表
status SwitchList(LISTS *Lists, char *name);     // 切换当前操作表

//======================== 函数实现 ================================

// 初始化顺序表：分配内存，设置初始状态
status InitList(SqList *L)
{
    // 如果表已经初始化过，不允许重复初始化
    if(L->elem != NULL){
        return INFEASIBLE;
    }
    // 动态分配初始内存
    L->elem = (ElemType*)malloc(sizeof(ElemType)*LIST_INIT_SIZE);
    if(!L->elem) return OVERFLOW;    // 分配失败
    L->length = 0;                   // 初始元素个数为0
    L->listsize = LIST_INIT_SIZE;    // 初始容量
    return OK;
}

// 销毁顺序表：释放内存，重置表状态
status DestroyList(SqList *L)
{
    if(L->elem == NULL){
        return INFEASIBLE;
    }
    free(L->elem);           // 释放动态分配的内存
    L->elem = NULL;          // 指针置空
    L->length = 0;           // 长度清零
    L->listsize = 0;         // 容量清零
    return OK;
}

// 清空表：只清空数据，不释放内存
status ClearList(SqList *L)
{
    if (L->elem == NULL) return INFEASIBLE;
    L->length = 0;
    return OK;
}

// 判断表是否为空
status ListEmpty(SqList L)
{
    if(L.elem == NULL) return INFEASIBLE;
    return (L.length == 0) ? TRUE : FALSE;
}

// 返回表的长度
status ListLength(SqList L)
{
    if(L.elem == NULL) return INFEASIBLE;
    return L.length;
}

// 获取第i个元素，存入e
status GetElem(SqList L,int i,ElemType *e)
{
    if(L.elem == NULL) return INFEASIBLE;
    if(i < 1 || i > L.length) return ERROR;   // 位置不合法
    *e = L.elem[i-1];                         // 取元素
    return OK;
}

// 查找元素e，返回第一次出现的位序，找不到返回0
int LocateElem(SqList L,ElemType e)
{
    int i;
    if(L.elem == NULL) return INFEASIBLE;
    // 遍历查找
    for(i=0; i<L.length && L.elem[i]!=e; i++);
    return (i == L.length) ? 0 : i+1;
}

// 求元素e的前驱，存入pre_e
status PriorElem(SqList L,ElemType e,ElemType *pre_e)
{
    if(L.elem == NULL) return INFEASIBLE;
    int i;
    for(i=0; i<L.length && L.elem[i]!=e; i++);
    // 第一个元素无前驱，或没找到e
    if(i == 0 || i >= L.length) return ERROR;
    *pre_e = L.elem[i-1];
    return OK;
}

// 求元素e的后继
status NextElem(SqList L,ElemType e,ElemType *next_e)
{
    if(L.elem == NULL) return INFEASIBLE;
    int i;
    for(i=0; i<L.length && L.elem[i]!=e; i++);
    // 最后一个元素无后继
    if(i >= L.length-1) return ERROR;
    *next_e = L.elem[i+1];
    return OK;
}

// 在第i个位置插入元素e
status ListInsert(SqList *L,int i,ElemType e)
{
    if (L->elem == NULL) return INFEASIBLE;
    if (i < 1 || i > L->length+1) return ERROR;

    // 空间不足则扩容
    if (L->length >= L->listsize){
        ElemType *newbase = (ElemType*)realloc(L->elem, sizeof(ElemType)*(L->listsize+LISTINCREMENT));
        if(!newbase) return OVERFLOW;
        L->elem = newbase;
        L->listsize += LISTINCREMENT;
    }

    // 元素后移，腾出位置i
    for (int j = L->length-1; j >= i-1; j--)
        L->elem[j+1] = L->elem[j];

    L->elem[i-1] = e;       // 插入新元素
    L->length++;            // 长度+1
    return OK;
}

// 删除第i个元素，将值存入e
status ListDelete(SqList *L,int i,ElemType *e)
{
    if (L->elem == NULL) return INFEASIBLE;
    if (i < 1 || i > L->length) return ERROR;
    *e = L->elem[i-1];                  // 取出被删元素
    for (int j = i-1; j < L->length-1; j++)  // 元素前移
        L->elem[j] = L->elem[j+1];
    L->length--;                        // 长度-1
    return OK;
}

// 遍历输出顺序表所有元素
status ListTraverse(SqList L)
{
    if (L.elem == NULL) return INFEASIBLE;
    if(L.length == 0) printf("空线性表");
    else{
        for(int i=0; i<L.length; i++){
            printf("%d", L.elem[i]);
            if(i != L.length-1) printf(" ");
        }
    }
    printf("\n");
    return OK;
}

// 最大子数组和（Kadane算法）
int MaxSubArray(SqList L)
{
    if(L.elem == NULL || L.length == 0) return INFEASIBLE;
    int maxSum = L.elem[0], curSum = L.elem[0];
    for(int i=1; i<L.length; i++){
        curSum = (curSum > 0) ? curSum + L.elem[i] : L.elem[i];
        maxSum = (maxSum > curSum) ? maxSum : curSum;
    }
    return maxSum;
}

// 求和为k的子数组个数
int SubArrayNum(SqList L,int k)
{
    if(L.elem == NULL || L.length == 0) return INFEASIBLE;
    int cnt = 0;
    for(int i=0; i<L.length; i++){
        int sum = 0;
        for(int j=i; j<L.length; j++){
            sum += L.elem[j];
            if(sum == k) cnt++;
        }
    }
    return cnt;
}

// 冒泡排序（升序）
void sortList(SqList *L)
{
    if(L->elem == NULL || L->length <= 1) return;
    for(int i=0; i<L->length-1; i++){
        for(int j=0; j<L->length-i-1; j++){
            if(L->elem[j] > L->elem[j+1]){
                ElemType t = L->elem[j];
                L->elem[j] = L->elem[j+1];
                L->elem[j+1] = t;
            }
        }
    }
}

// 将顺序表保存到文件（二进制）
status SaveList(SqList L,char FileName[])
{
    if(L.elem == NULL) return INFEASIBLE;
    FILE *fp = fopen(FileName,"wb");
    if(!fp) return INFEASIBLE;
    fwrite(&L.length, sizeof(int), 1, fp);          // 保存长度
    fwrite(L.elem, sizeof(ElemType), L.length, fp); // 保存数据
    fclose(fp);
    return OK;
}

// 从文件加载顺序表
status LoadList(SqList *L,char FileName[])
{
    // 如果已有数据，先释放
    if(L->elem != NULL)
    {
        free(L->elem);
        L->elem = NULL;
    }

    FILE* fp = fopen(FileName,"rb");
    if(!fp) return INFEASIBLE;

    int len;
    fread(&len, sizeof(int), 1, fp);               // 读取长度
    L->elem = (ElemType*)malloc(sizeof(ElemType)*len);
    L->length = len;
    L->listsize = len;
    fread(L->elem, sizeof(ElemType), len, fp);     // 读取数据
    fclose(fp);
    return OK;
}

//======================== 多表管理函数 ================================

// 新建一张线性表，加入表管理器
status AddList(LISTS *Lists,char ListName[])
{
    if (Lists->length >= 10) return ERROR;
    strcpy(Lists->elem[Lists->length].name, ListName);
    Lists->elem[Lists->length].L.elem = NULL;
    Lists->elem[Lists->length].L.length = 0;
    Lists->elem[Lists->length].L.listsize = 0;
    Lists->length++;
    return OK;
}

// 删除指定名称的线性表
status RemoveList(LISTS *Lists,char ListName[])
{
    int i,j;
    for(i = 0; i < Lists->length; i++)
    {
        if(strcmp(ListName, Lists->elem[i].name) == 0)
        {
            // 默认表不允许删除
            if(strcmp(ListName, "默认表") == 0)
            {
                printf("无法删除默认表！\n");
                return ERROR;
            }

            DestroyList(&Lists->elem[i].L);
            // 后面的表前移
            for(j = i; j < Lists->length - 1; j++)
            {
                Lists->elem[j] = Lists->elem[j+1];
            }
            Lists->length--;
            // 调整当前表指针
            if (i == current) current = 0;
            else if (i < current) current--;
            return OK;
        }
    }
    return ERROR;
}

// 根据表名查找位置
int LocateList(LISTS Lists,char ListName[])
{
    int i;
    for(i = 0; i < Lists.length; i++)
    {
        if(strcmp(ListName, Lists.elem[i].name) == 0)
        {
            return i;
        }
    }
    return -1;
}

// 切换当前操作表
status SwitchList(LISTS *Lists, char *name)
{
    int idx = LocateList(*Lists, name);
    if (idx < 0) return ERROR;
    current = idx;
    return OK;
}

// 显示所有表及其内容
void ShowAllLists(LISTS Lists)
{
    printf("\n===== 所有线性表及其数据 =====\n");
    if(Lists.length == 0)
    {
        printf("暂无任何线性表\n");
        return;
    }
    for(int i = 0; i < Lists.length; i++)
    {
        printf("%d. 表名：%s  数据：", i+1, Lists.elem[i].name);
        if(Lists.elem[i].L.elem == NULL)
            printf("未初始化");
        else{
            if(Lists.elem[i].L.length == 0) printf("空线性表");
            else{
                for(int j=0; j<Lists.elem[i].L.length; j++)
                    printf("%d ", Lists.elem[i].L.elem[j]);
            }
        }
        if (i == current) printf("← 当前操作");
        printf("\n");
    }
    printf("\n");
}

//======================== 菜单与主函数 ================================

// 菜单界面
void Menu()
{
    printf("\n=============================================== 顺序表操作菜单 ===============================================\n");
    printf("1.初始化表\t\t2.销毁表\t\t3.清空表\t\t4.判定空表\n");
    printf("5.求表长\t\t6.获得元素\t\t7.查找元素\t\t8.获得前驱\n");
    printf("9.获得后继\t\t10.插入元素\t\t11.删除元素\t\t12.遍历表\n");
    printf("13.最大子数组和\t\t14.和为K子数组数\t15.顺序表排序\t\t16.保存到文件\t\t17.从文件加载\n");
    printf("18.新建线性表\t\t19.删除线性表\t\t20.按名查找表\t\t21.显示所有表\t\t22.切换当前表\n");
    printf("0.退出\n");
    printf("请选择:");
}

// 主函数：程序入口
int main()
{
    Lists.length = 0;
    AddList(&Lists, "默认表");    // 默认创建一张表
    current = 0;                  // 默认操作第一张表

    int op,i,k,pos;
    ElemType e;
    char fname[30],lname[30];

    while(1){
        Menu();
        scanf("%d",&op);
        SqList *now = &Lists.elem[current].L;  // 当前操作表

        switch(op){
            case 1:
            {
                status res = InitList(now);
                if(res == OK) printf("初始化成功\n");
                else if(res == INFEASIBLE) printf("已经初始化，无需重复操作\n");
                else printf("初始化失败\n");
                break;
            }
            case 2:
            {
                // 默认表不能销毁
                if(strcmp(Lists.elem[current].name, "默认表") == 0)
                {
                    printf("默认表不能销毁！\n");
                    break;
                }
                status res = DestroyList(now);
                if(res == OK) printf("销毁成功\n");
                else if(res == INFEASIBLE) printf("已经销毁，无需重复操作\n");
                else printf("销毁失败\n");
                break;
            }
            case 3:
            {
                status res = ClearList(now);
                if(res == OK) printf("清空成功\n");
                else printf("表未初始化，无法清空\n");
                break;
            }
            case 4:
            {
                status res = ListEmpty(*now);
                if(res == TRUE) printf("空表\n");
                else if(res == FALSE) printf("非空表\n");
                else printf("表未初始化\n");
                break;
            }
            case 5:
            {
                status res = ListLength(*now);
                if(res != INFEASIBLE) printf("表长：%d\n",res);
                else printf("表未初始化\n");
                break;
            }
            case 6:
                printf("输入位置："); scanf("%d",&i);
                if(GetElem(*now,i,&e)==OK) printf("元素：%d\n",e);
                else printf("失败\n"); break;
            case 7:
                printf("输入值："); scanf("%d",&e);
                pos=LocateElem(*now,e);
                pos?printf("位置：%d\n",pos):printf("未找到\n"); break;
            case 8:
                printf("输入元素："); scanf("%d",&e);
                if(PriorElem(*now,e,&i)==OK) printf("前驱：%d\n",i);
                else printf("无\n"); break;
            case 9:
                printf("输入元素："); scanf("%d",&e);
                if(NextElem(*now,e,&i)==OK) printf("后继：%d\n",i);
                else printf("无\n"); break;
            case 10:
                printf("位置 值："); scanf("%d%d",&i,&e);
                if(ListInsert(now,i,e)==OK) printf("成功\n");
                else printf("失败\n"); break;
            case 11:
                printf("位置："); scanf("%d",&i);
                if(ListDelete(now,i,&e)==OK) printf("删除：%d\n",e);
                else printf("失败\n"); break;
            case 12:
                if(ListTraverse(*now) == INFEASIBLE) printf("表未初始化\n");
                break;
            case 13:
            {
                int sum = MaxSubArray(*now);
                if(sum != INFEASIBLE) printf("最大子数组和：%d\n",sum);
                else printf("表未初始化或为空\n");
                break;
            }
            case 14:
                printf("输入k："); scanf("%d",&k);
                {
                    int num = SubArrayNum(*now,k);
                    if(num != INFEASIBLE) printf("个数：%d\n",num);
                    else printf("表未初始化或为空\n");
                }
                break;
            case 15:
                sortList(now);
                printf("排序完成\n");
                break;
            case 16:
                printf("文件名："); scanf("%s",fname);
                if(SaveList(*now,fname)==OK) printf("保存成功\n");
                else printf("失败\n"); break;
            case 17:
                printf("文件名："); scanf("%s",fname);
                if(LoadList(now,fname)==OK) printf("加载成功\n");
                else printf("失败\n"); break;
            case 18:
            {
                int data;
                printf("输入新表名：");
                scanf("%s", lname);
                AddList(&Lists, lname);

                SqList *cur = &Lists.elem[Lists.length-1].L;
                InitList(cur);

                printf("输入数据（以0结束）：");
                while(1)
                {
                    scanf("%d", &data);
                    if(data == 0) break;
                    ListInsert(cur, cur->length+1, data);
                }

                printf("%s ", lname);
                for(int j=0; j<cur->length; j++)
                    printf("%d ", cur->elem[j]);
                printf("\n");
                break;
            }
            case 19:
                printf("输入要删除的表名：");
                scanf("%s", lname);
                if(RemoveList(&Lists, lname))
                    printf("删除成功！\n");
                else
                    printf("删除失败\n");
                break;
            case 20:
                printf("输入要查找的表名：");
                scanf("%s", lname);
                pos = LocateList(Lists, lname);
                if(pos >= 0)
                    printf("该表是第 %d 个\n", pos+1);
                else
                    printf("未找到\n");
                break;
            case 21:
                ShowAllLists(Lists);
                break;

            case 22:
                printf("输入表名：");
                scanf("%s", lname);
                if (SwitchList(&Lists, lname))
                    printf("已切换到表：%s\n", lname);
                else
                    printf("表不存在\n");
                break;

            case 0:
                // 退出前销毁所有表
                for (int i=0; i<Lists.length; i++)
                    DestroyList(&Lists.elem[i].L);
                printf("退出程序\n");
                return 0;
            default:
                printf("输入错误\n");
        }
    }
    return 0;
}