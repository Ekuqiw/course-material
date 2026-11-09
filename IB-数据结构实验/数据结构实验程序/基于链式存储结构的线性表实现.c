#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 宏定义状态码，用于函数返回值表示操作结果
#define TRUE 1    // 真
#define FALSE 0   // 假
#define OK 1      // 操作成功
#define ERROR 0   // 操作失败
#define OVERFLOW -1 // 内存溢出

typedef int ElemType;  // 数据元素类型（这里定义为 int）
typedef int Status;    // 状态类型，用于返回函数执行状态

// 单链表结点结构定义
typedef struct LNode {
    ElemType data;       // 数据域：存储结点数据
    struct LNode *next;  // 指针域：指向下一个结点
} LNode, *LinkList;

// 多链表管理器：最多管理 10 个链表
#define MAX_LIST 10
typedef struct {
    LinkList lists[MAX_LIST];  // 存储多个链表的头指针
    char names[MAX_LIST][20];  // 每个链表对应的名称
    int count;                 // 当前已创建的链表数量
    int current;               // 当前正在操作的链表下标
} ListManager;

ListManager manager;  // 全局链表管理器

// ==================== 函数声明 ====================
// 基础单链表操作
Status InitList(LinkList *L);               // 初始化链表（创建头结点）
Status DestroyList(LinkList *L);            // 销毁整个链表（含头结点）
Status ClearList(LinkList L);               // 清空链表（保留头结点）
Status ListEmpty(LinkList L);               // 判断链表是否为空
int ListLength(LinkList L);                 // 获取链表长度
Status GetElem(LinkList L, int i, ElemType *e);  // 获取第 i 个元素
int LocateElem(LinkList L, ElemType e, Status (*compare)(ElemType, ElemType));  // 查找元素
Status PriorElem(LinkList L, ElemType cur_e, ElemType *pre_e);    // 求前驱
Status NextElem(LinkList L, ElemType cur_e, ElemType *next_e);    // 求后继
Status ListInsert(LinkList L, int i, ElemType e);   // 在第 i 个位置插入元素
Status ListDelete(LinkList L, int i, ElemType *e);  // 删除第 i 个元素
Status ListTraverse(LinkList L, Status (*visit)(ElemType));  // 遍历链表

// 扩展功能
Status reverseList(LinkList L);             // 翻转链表
Status RemoveNthFromEnd(LinkList L, int n); // 删除倒数第 n 个结点
Status sortList(LinkList L);                // 链表排序（冒泡）
Status SaveListToFile(LinkList L, char *filename);  // 保存链表到文件
Status LoadListFromFile(LinkList L, char *filename); // 从文件加载链表

// 多链表管理
void InitManager();                         // 初始化链表管理器
int AddList(char *name);                    // 新建一个链表
Status DelList(int index);                  // 删除指定序号的链表
void SwitchList(int index);                 // 切换当前操作的链表
void ShowAllLists();                        // 显示所有链表

// 辅助函数
Status visit(ElemType e);                   // 遍历输出函数
Status compare(ElemType a, ElemType b);     // 比较函数（相等判断）
void Menu();                                // 菜单函数

// ==================== 主函数 ====================
int main() {
    InitManager();          // 初始化链表管理器
    AddList("默认表");      // 默认创建一个“默认表”
    int choice, i, pos, n;
    ElemType e, pre_e, next_e;
    char filename[50], name[20];

    // 循环菜单
    while (1) {
        Menu();
        scanf("%d", &choice);
        // 获取当前正在操作的链表
        LinkList L = manager.lists[manager.current];

        switch (choice) {
            case 1:  // 1. 初始化表
                if (L != NULL) {
                    printf("线性表已初始化，无需重复操作！\n");
                } else {
                    if (InitList(&L)) {
                        manager.lists[manager.current] = L;
                        printf("线性表初始化成功！\n");
                    }
                }
                break;

            case 2:  // 2. 销毁表
                if (L == NULL) {
                    printf("线性表已销毁，无需重复操作！\n");
                } else {
                    if (DestroyList(&L)) {
                        manager.lists[manager.current] = NULL;
                        printf("线性表销毁成功！\n");
                    }
                }
                break;

            case 3:  // 3. 清空表
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                } else {
                    if (ClearList(L)) printf("线性表清空成功！\n");
                }
                break;

            case 4:  // 4. 判断空表
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                } else {
                    if (ListEmpty(L)) printf("线性表为空！\n");
                    else printf("线性表非空！\n");
                }
                break;

            case 5:  // 5. 求表长
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                } else {
                    printf("线性表长度：%d\n", ListLength(L));
                }
                break;

            case 6:  // 6. 获取元素
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                    break;
                }
                printf("请输入要获取的元素位置：");
                scanf("%d", &i);
                if (GetElem(L, i, &e)) printf("第%d个元素：%d\n", i, e);
                else printf("位置非法！\n");
                break;

            case 7:  // 7. 查找元素
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                    break;
                }
                printf("请输入要查找的元素：");
                scanf("%d", &e);
                pos = LocateElem(L, e, compare);
                if (pos) printf("元素%d的位置：%d\n", e, pos);
                else printf("未找到该元素！\n");
                break;

            case 8:  // 8. 求前驱
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                    break;
                }
                printf("请输入元素：");
                scanf("%d", &e);
                if (PriorElem(L, e, &pre_e)) printf("前驱元素：%d\n", pre_e);
                else printf("无前驱元素！\n");
                break;

            case 9:  // 9. 求后继
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                    break;
                }
                printf("请输入元素：");
                scanf("%d", &e);
                if (NextElem(L, e, &next_e)) printf("后继元素：%d\n", next_e);
                else printf("无后继元素！\n");
                break;

            case 10:  // 10. 插入元素
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                    break;
                }
                printf("请输入插入位置和元素（空格分隔）：");
                scanf("%d %d", &i, &e);
                if (ListInsert(L, i, e)) printf("插入成功！\n");
                else printf("插入失败！\n");
                break;

            case 11:  // 11. 删除元素
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                    break;
                }
                printf("请输入删除位置：");
                scanf("%d", &i);
                if (ListDelete(L, i, &e)) printf("删除成功，元素值：%d\n", e);
                else printf("删除失败！\n");
                break;

            case 12:  // 12. 遍历表
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                    break;
                }
                printf("链表元素：");
                ListTraverse(L, visit);
                printf("\n");
                break;

            case 13:  // 13. 翻转链表
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                    break;
                }
                reverseList(L);
                printf("链表翻转完成！\n");
                break;

            case 14:  // 14. 删除倒数第n个结点
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                    break;
                }
                printf("请输入n值：");
                scanf("%d", &n);
                if (RemoveNthFromEnd(L, n)) printf("删除成功！\n");
                else printf("删除失败！\n");
                break;

            case 15:  // 15. 链表排序
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                    break;
                }
                sortList(L);
                printf("链表排序完成（升序）！\n");
                break;

            case 16:  // 16. 保存到文件
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                    break;
                }
                printf("请输入文件名：");
                scanf("%s", filename);
                if (SaveListToFile(L, filename)) printf("保存成功！\n");
                else printf("保存失败！\n");
                break;

            case 17:  // 17. 从文件加载
                if (L == NULL) {
                    printf("错误：线性表未初始化！\n");
                    break;
                }
                printf("请输入文件名：");
                scanf("%s", filename);
                ClearList(L);
                if (LoadListFromFile(L, filename)) printf("加载成功！\n");
                else printf("加载失败！\n");
                break;

            case 18:  // 18. 添加线性表
                printf("请输入线性表名称：");
                scanf("%s", name);
                if (AddList(name)) printf("创建成功！\n");
                else printf("创建失败！\n");
                break;

            case 19:  // 19. 删除线性表
                ShowAllLists();
                printf("请输入要删除的表序号：");
                scanf("%d", &i);
                if (DelList(i)) printf("删除成功！\n");
                else printf("删除失败！\n");
                break;

            case 20:  // 20. 切换线性表
                ShowAllLists();
                printf("请输入要切换的表序号：");
                scanf("%d", &i);
                SwitchList(i);
                break;

            case 0:  // 0. 退出程序
                printf("程序退出！\n");
                return 0;

            default:
                printf("输入错误，请重新选择！\n");
        }
    }
}

// 初始化单链表（创建头结点，不存放数据）
Status InitList(LinkList *L) {
    *L = (LinkList)malloc(sizeof(LNode));  // 分配头结点空间
    if (!(*L)) exit(OVERFLOW);             // 分配失败则退出
    (*L)->next = NULL;                     // 头结点指针域置空
    return OK;
}

// 销毁整个链表（释放所有结点，包括头结点）
Status DestroyList(LinkList *L) {
    LinkList p, q;
    p = *L;             // p 指向头结点
    while (p) {         // 遍历释放所有结点
        q = p->next;
        free(p);
        p = q;
    }
    *L = NULL;          // 头指针置空
    return OK;
}

// 清空链表（只删除数据结点，保留头结点）
Status ClearList(LinkList L) {
    LinkList p = L->next, q;
    while (p) {
        q = p->next;
        free(p);
        p = q;
    }
    L->next = NULL;  // 头结点指针置空
    return OK;
}

// 判断链表是否为空（空返回 TRUE，非空返回 FALSE）
Status ListEmpty(LinkList L) {
    if (L && L->next == NULL) return TRUE;
    return FALSE;
}

// 获取链表长度（数据结点个数）
int ListLength(LinkList L) {
    int len = 0;
    LinkList p = L->next;
    while (p) {
        len++;
        p = p->next;
    }
    return len;
}

// 获取第 i 个元素，存入 e
Status GetElem(LinkList L, int i, ElemType *e) {
    int j = 1;
    LinkList p = L->next;  // p 指向第一个数据结点
    // 找到第 i 个结点
    while (p && j < i) {
        p = p->next;
        j++;
    }
    if (!p || j > i) return ERROR;  // 位置非法
    *e = p->data;
    return OK;
}

// 查找元素 e，返回位序，找不到返回 0
int LocateElem(LinkList L, ElemType e, Status (*compare)(ElemType, ElemType)) {
    int pos = 1;
    LinkList p = L->next;
    while (p) {
        if (compare(p->data, e)) return pos;
        p = p->next;
        pos++;
    }
    return 0;
}

// 求 cur_e 的前驱，存入 pre_e
Status PriorElem(LinkList L, ElemType cur_e, ElemType *pre_e) {
    LinkList p = L->next, q = L;
    // p 指向目标结点，q 指向 p 的前驱
    while (p && p->data != cur_e) {
        q = p;
        p = p->next;
    }
    if (!p || q == L) return ERROR;  // 无前驱
    *pre_e = q->data;
    return OK;
}

// 求 cur_e 的后继，存入 next_e
Status NextElem(LinkList L, ElemType cur_e, ElemType *next_e) {
    LinkList p = L->next;
    while (p && p->data != cur_e) p = p->next;
    if (!p || !p->next) return ERROR;  // 无后继
    *next_e = p->next->data;
    return OK;
}

// 在第 i 个位置插入元素 e
Status ListInsert(LinkList L, int i, ElemType e) {
    LinkList p = L, s;
    int j = 0;
    // 找到第 i-1 个结点
    while (p && j < i - 1) {
        p = p->next;
        j++;
    }
    if (!p || j > i - 1) return ERROR;  // 位置非法
    // 创建新结点
    s = (LinkList)malloc(sizeof(LNode));
    s->data = e;
    s->next = p->next;
    p->next = s;
    return OK;
}

// 删除第 i 个结点，将值存入 e
Status ListDelete(LinkList L, int i, ElemType *e) {
    LinkList p = L, q;
    int j = 0;
    // 找到第 i-1 个结点
    while (p->next && j < i - 1) {
        p = p->next;
        j++;
    }
    if (!p->next || j > i - 1) return ERROR;  // 位置非法
    q = p->next;         // q 为待删除结点
    *e = q->data;        // 保存被删结点数据
    p->next = q->next;   // 移除结点
    free(q);             // 释放空间
    return OK;
}

// 遍历链表，调用 visit 函数输出
Status ListTraverse(LinkList L, Status (*visit)(ElemType)) {
    LinkList p = L->next;
    while (p) {
        visit(p->data);
        p = p->next;
    }
    return OK;
}

// 翻转链表（双指针法）
Status reverseList(LinkList L) {
    LinkList pre = NULL, cur = L->next, next;
    while (cur) {
        next = cur->next;
        cur->next = pre;
        pre = cur;
        cur = next;
    }
    L->next = pre;  // 头结点指向翻转后的第一个结点
    return OK;
}

// 删除倒数第 n 个结点（快慢指针法）
Status RemoveNthFromEnd(LinkList L, int n) {
    LinkList fast = L, slow = L;
    int len = ListLength(L);
    if (n < 1 || n > len) return ERROR;
    // 快指针先走 n 步
    for (int i = 0; i < n; i++) fast = fast->next;
    // 快慢指针一起走
    while (fast->next) {
        fast = fast->next;
        slow = slow->next;
    }
    // 删除慢指针的下一个结点
    LinkList del = slow->next;
    slow->next = del->next;
    free(del);
    return OK;
}

// 链表排序（冒泡排序，交换数据域）
Status sortList(LinkList L) {
    int len = ListLength(L);
    if (len <= 1) return OK;
    LinkList p, q;
    ElemType temp;
    for (int i = 0; i < len - 1; i++) {
        p = L->next;
        for (int j = 0; j < len - i - 1; j++) {
            q = p->next;
            if (p->data > q->data) {
                // 交换数据
                temp = p->data;
                p->data = q->data;
                q->data = temp;
            }
            p = p->next;
        }
    }
    return OK;
}

// 将链表保存到文本文件
Status SaveListToFile(LinkList L, char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return ERROR;
    LinkList p = L->next;
    while (p) {
        fprintf(fp, "%d ", p->data);
        p = p->next;
    }
    fclose(fp);
    return OK;
}

// 从文本文件加载数据到链表
Status LoadListFromFile(LinkList L, char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return ERROR;
    ElemType e;
    LinkList tail = L;
    // 读取数据并尾插法建表
    while (fscanf(fp, "%d", &e) != EOF) {
        LinkList s = (LinkList)malloc(sizeof(LNode));
        s->data = e;
        s->next = NULL;
        tail->next = s;
        tail = s;
    }
    fclose(fp);
    return OK;
}

// 初始化链表管理器
void InitManager() {
    manager.count = 0;
    manager.current = -1;
    for (int i = 0; i < MAX_LIST; i++) manager.lists[i] = NULL;
}

// 新建一个链表，加入管理器
int AddList(char *name) {
    if (manager.count >= MAX_LIST) return 0;
    manager.lists[manager.count] = NULL;
    strcpy(manager.names[manager.count], name);
    manager.current = manager.count;
    manager.count++;
    return 1;
}

// 删除指定序号的链表
Status DelList(int index) {
    if (index < 0 || index >= manager.count) return ERROR;
    DestroyList(&manager.lists[index]);
    // 后面的链表前移
    for (int i = index; i < manager.count - 1; i++) {
        manager.lists[i] = manager.lists[i + 1];
        strcpy(manager.names[i], manager.names[i + 1]);
    }
    manager.count--;
    manager.current = manager.count > 0 ? 0 : -1;
    return OK;
}

// 切换当前操作的链表
void SwitchList(int index) {
    if (index >= 0 && index < manager.count) {
        manager.current = index;
        printf("已切换到表：%s\n", manager.names[index]);
    } else printf("序号非法！\n");
}

// 显示所有链表列表
void ShowAllLists() {
    printf("当前所有线性表：\n");
    for (int i = 0; i < manager.count; i++) {
        if (i == manager.current) printf("[%d] %s（当前）\n", i, manager.names[i]);
        else printf("[%d] %s\n", i, manager.names[i]);
    }
}

// 遍历输出函数
Status visit(ElemType e) {
    printf("%d ", e);
    return OK;
}

// 比较函数：判断两个元素是否相等
Status compare(ElemType a, ElemType b) {
    return a == b;
}

// 菜单界面
void Menu() {
    printf("\n=============== 单链表功能菜单 ===============\n");
    printf("1. 初始化表    2. 销毁表    3. 清空表    4. 判断空表\n");
    printf("5. 求表长      6. 获取元素  7. 查找元素  8. 求前驱\n");
    printf("9. 求后继     10. 插入元素 11. 删除元素 12. 遍历表\n");
    printf("13. 翻转链表  14. 删除倒数第n个结点  15. 链表排序\n");
    printf("16. 保存到文件  17. 从文件加载  18. 添加线性表\n");
    printf("19. 删除线性表  20. 切换线性表  0. 退出程序\n");
    printf("==============================================\n");
    printf("当前操作：%s，请输入功能序号：", manager.names[manager.current]);
}