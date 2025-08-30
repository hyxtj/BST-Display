/***************************************************************************
  文件名称：BinarySearchTree.h
  功    能：二叉搜索树类的声明文件，包含节点的插入、删除、查找、平衡等操作
  说    明：
***************************************************************************/

#ifndef BINARYSEARCHTREE_H
#define BINARYSEARCHTREE_H

#include <QString>
#include <QVector>
#include <QTimer>
#include <QObject>
#include "TreeNode.h"

/***************************************************************************
  类名称：BinarySearchTree
  功    能：二叉搜索树数据结构实现
  说    明：支持插入、删除、查找、平衡等操作，包含动画功能
***************************************************************************/
class BinarySearchTree : public QObject {
    Q_OBJECT

public:
    explicit BinarySearchTree(QObject* parent = nullptr); // 构造函数
    ~BinarySearchTree();                                  // 析构函数

    //基本操作
    void    insert(int value);                            // 插入节点
    bool    find(int value, int& depth);                  // 查找节点
    void    remove(int value);                            // 删除节点
    QString display();                                    // 显示树内容
    bool    isEmpty();                                    // 检查树是否为空

    void    clear();                                      // 清空树

    TreeNode* getRoot() const { return root; }            // 返回根节点

    void    balance();                                    // 平衡树操作

    int     getHeight();                                  // 获取树的高度

    // 动画控制
    void startFindAnimation(int value);                            // 开始查找动画
    void startInsertAnimation(int value);                          // 开始插入动画
    void startDeleteAnimation(int value);                          // 开始删除动画
    void startBalanceAnimation();                                  // 开始平衡动画
    void setAnimationSpeed(int speed) { animationSpeed = speed; }  // 设置动画速度
    void stopAnimation();                                          // 停止动画

    bool isAnimating() const { return isAnimationRunning; }        // 获取动画状态

signals:
    void treeChanged();                                                 // 树结构变化信号
    void animationStep(QString description, int highlightedValue = -1); // 动画步骤信号
    void animationFinished();                                           // 动画完成信号
    void highlightNode(int value);                                      // 高亮节点信号
    void clearHighlights();                                             // 清除高亮信号
    void highlightPath(const QVector<int>& path);                       // 高亮路径信号

private:
    TreeNode* root;          // 根节点指针
    int animationSpeed;      // 动画速度
    bool isAnimationRunning; // 动画运行状态标志

    // 动画相关
    QTimer* animationTimer;                      // 动画定时器
    QVector<QPair<QString, int>> animationSteps; // 动画步骤列表
    int currentStep;                             // 当前动画步骤索引

    // 待操作的值
    int pendingInsertValue;                                     // 待插入的值
    int pendingDeleteValue;                                     // 待删除的值

    TreeNode* insertNode(TreeNode* node, int value, int depth);                    // 递归插入节点
    bool      findNode(TreeNode* node, int value, int& depth, QVector<int>& path); // 递归查找节点
    TreeNode* deleteNode(TreeNode* node, int value);                               // 递归删除节点
    void      inOrderTraversal(TreeNode* node, QString& result);                   // 中序遍历
    void      clearTree(TreeNode* node);                                           // 递归清空树

    // 平衡相关方法
    void storeNodesInOrder(TreeNode* node, QVector<int>& values);                     // 按顺序存储节点值
    TreeNode* buildBalancedTree(QVector<int>& values, int start, int end, int depth); // 构建平衡树

    // 高度更新方法
    void updateDepths(TreeNode* node, int depth); // 更新节点深度
    int  calculateHeight(TreeNode* node);         // 计算树高度

    // 动画步骤
    void processNextAnimationStep();    // 处理下一个动画步骤
    void animateFind(int value);        // 生成查找动画步骤
    void animateInsertion(int value);   // 生成插入动画步骤
    void animateDeletion(int value);    // 生成删除动画步骤
    void animateBalancing();            // 生成平衡动画步骤

    // 查找路径
    bool findNodeWithPath(TreeNode* node, int value, QVector<int>& path); // 查找节点并记录路径

    QVector<int> currentPath; // 当前动画路径
    void storeNodesInOrderForAnimation(TreeNode* node, QVector<int>& values); // 为动画按顺序存储节点
};

#endif // BINARYSEARCHTREE_H