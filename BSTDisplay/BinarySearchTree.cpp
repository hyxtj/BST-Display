/***************************************************************************
  文件名称：BinarySearchTree.cpp
  功    能：二叉搜索树类的实现文件，包含节点的插入、删除、查找、平衡等操作
  说    明：包含动画效果的实现，支持可视化操作过程
***************************************************************************/

#include "BinarySearchTree.h"
#include <algorithm>
#include <queue>
#include <QDebug>

/***************************************************************************
  函数名称：BinarySearchTree::BinarySearchTree
  功    能：构造函数，初始化二叉搜索树
  输入参数：parent - 父对象指针
  返 回 值：
  说    明：初始化根节点为空，设置动画速度，连接定时器信号与槽
***************************************************************************/
BinarySearchTree::BinarySearchTree(QObject* parent) : 
    QObject(parent)      , root(nullptr),
    animationSpeed(1000) , isAnimationRunning(false),
    pendingInsertValue(0), pendingDeleteValue(0)
{
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &BinarySearchTree::processNextAnimationStep);
}

/***************************************************************************
  函数名称：BinarySearchTree::~BinarySearchTree
  功    能：析构函数，清理树节点和停止动画
  输入参数：
  返 回 值：
  说    明：递归删除所有节点，停止动画定时器
***************************************************************************/
BinarySearchTree::~BinarySearchTree() {
    clearTree(root); //删除二叉树数据
    stopAnimation();
}

/***************************************************************************
  函数名称：BinarySearchTree::clear
  功    能：清空树中的所有节点
  输入参数：
  返 回 值：
  说    明：删除所有节点并将根节点设为空，发出树变化信号
***************************************************************************/
void BinarySearchTree::clear() {
    clearTree(root);
    root = nullptr;
    emit treeChanged(); //发送树改变信号
}

/***************************************************************************
  函数名称：BinarySearchTree::insert
  功    能：插入新节点到二叉搜索树中
  输入参数：value - 要插入的值
  返 回 值：
  说    明：如果树为空则创建根节点，否则递归插入，更新节点深度并发出树变化信号
***************************************************************************/
void BinarySearchTree::insert(int value) {
    if (root == nullptr) {
        root = new TreeNode(value, 1);
    }
    else {
        root = insertNode(root, value, 1);
    }

    updateDepths(root, 1); //更新所有节点的高度
    emit treeChanged();
}

/***************************************************************************
  函数名称：BinarySearchTree::find
  功    能：在二叉搜索树中查找指定值
  输入参数：value - 要查找的值，depth - 用于返回节点深度
  返 回 值：bool - 是否找到节点
  说    明：递归查找节点，如果找到则设置深度并返回true
***************************************************************************/
bool BinarySearchTree::find(int value, int& depth) {
    QVector<int> path;
    return findNode(root, value, depth, path);
}

/***************************************************************************
  函数名称：BinarySearchTree::remove
  功    能：从二叉搜索树中删除指定值的节点
  输入参数：value - 要删除的值
  返 回 值：
  说    明：递归删除节点，更新节点深度并发出树变化信号
***************************************************************************/
void BinarySearchTree::remove(int value) {
    root = deleteNode(root, value);
    updateDepths(root, 1);
    emit treeChanged();
}

/***************************************************************************
  函数名称：BinarySearchTree::display
  功    能：获取二叉搜索树的字符串表示
  输入参数：
  返 回 值：QString - 树的字符串表示
  说    明：返回中序遍历结果，包含节点值和深度，如果树为空则返回相应提示
***************************************************************************/
QString BinarySearchTree::display() {
    QString result;
    inOrderTraversal(root, result); //中序遍历
    if (result.isEmpty()) {
        return QString::fromUtf8("树为空");
    }
    return result;
}

/***************************************************************************
  函数名称：BinarySearchTree::isEmpty
  功    能：检查二叉搜索树是否为空
  输入参数：
  返 回 值：bool - 树是否为空
  说    明：如果根节点为空则返回true
***************************************************************************/
bool BinarySearchTree::isEmpty() {
    return root == nullptr;
}

/***************************************************************************
  函数名称：BinarySearchTree::getHeight
  功    能：获取二叉搜索树的高度
  输入参数：
  返 回 值：int - 树的高度
  说    明：递归计算树的最大深度
***************************************************************************/
int BinarySearchTree::getHeight() {
    return calculateHeight(root); 
}

/***************************************************************************
  函数名称：BinarySearchTree::insertNode
  功    能：递归插入节点到二叉搜索树中
  输入参数：node - 当前节点指针，value - 要插入的值，depth - 当前深度
  返 回 值：TreeNode* - 插入后的节点指针
  说    明：根据值大小递归插入到左子树或右子树，如果值已存在则不插入
***************************************************************************/
TreeNode* BinarySearchTree::insertNode(TreeNode* node, int value, int depth) {
    if (node == nullptr) {
        return new TreeNode(value, depth);
    }

    if (value < node->value) {
        node->left = insertNode(node->left, value, depth + 1);
    }
    else if (value > node->value) {
        node->right = insertNode(node->right, value, depth + 1);
    }
    else {
        return node;        // 值已存在，不插入
    }

    return node;
}

/***************************************************************************
  函数名称：BinarySearchTree::findNode
  功    能：递归查找二叉搜索树中的节点
  输入参数：node - 当前节点指针，value - 要查找的值，depth - 用于返回节点深度，path - 用于记录查找路径
  返 回 值：bool - 是否找到节点
  说    明：根据值大小递归查找左子树或右子树，记录路径并在找到时设置深度
***************************************************************************/
bool BinarySearchTree::findNode(TreeNode* node, int value, int& depth, QVector<int>& path) {
    if (node == nullptr) {
        return false;
    }

    // 记录路径
    path.append(node->value);

    if (value == node->value) {
        depth = node->depth;
        return true;
    }
    else if (value < node->value) {
        return findNode(node->left, value, depth, path);
    }
    else {
        return findNode(node->right, value, depth, path);
    }
}

/***************************************************************************
  函数名称：BinarySearchTree::findNodeWithPath
  功    能：递归查找节点并记录查找路径
  输入参数：node - 当前节点指针，value - 要查找的值，path - 用于记录查找路径
  返 回 值：bool - 是否找到节点
  说    明：根据值大小递归查找左子树或右子树，记录路径但不设置深度
***************************************************************************/
bool BinarySearchTree::findNodeWithPath(TreeNode* node, int value, QVector<int>& path) {
    if (node == nullptr) {
        return false;
    }

    // 记录路径
    path.append(node->value);

    if (value == node->value) {
        return true;
    }
    else if (value < node->value) {
        return findNodeWithPath(node->left, value, path);
    }
    else {
        return findNodeWithPath(node->right, value, path);
    }
}

/***************************************************************************
  函数名称：BinarySearchTree::deleteNode
  功    能：递归删除二叉搜索树中的节点
  输入参数：node - 当前节点指针，value - 要删除的值
  返 回 值：TreeNode* - 删除后的节点指针
  说    明：根据值大小递归查找要删除的节点，处理三种删除情况：无子节点、一个子节点、两个子节点
***************************************************************************/
TreeNode* BinarySearchTree::deleteNode(TreeNode* node, int value) {
    if (node == nullptr) {
        return nullptr;
    }

    if (value < node->value) {
        node->left = deleteNode(node->left, value);
    }
    else if (value > node->value) {
        node->right = deleteNode(node->right, value);
    }
    else {
        if (node->left == nullptr && node->right == nullptr) {
            delete node;
            node = nullptr;
        }
        else if (node->left == nullptr) {
            TreeNode* temp = node;
            node = node->right;
            delete temp;
        }
        else if (node->right == nullptr) {
            TreeNode* temp = node;
            node = node->left;
            delete temp;
        }
        else {
            TreeNode* temp = node->right;
            while (temp->left != nullptr) {
                temp = temp->left;
            }
            node->value = temp->value;
            node->right = deleteNode(node->right, temp->value);
        }
    }

    return node;
}

/***************************************************************************
  函数名称：BinarySearchTree::inOrderTraversal
  功    能：递归进行中序遍历
  输入参数：node - 当前节点指针，result - 用于存储遍历结果的字符串
  返 回 值：
  说    明：按左子树-根节点-右子树的顺序遍历，将节点值和深度添加到结果字符串
***************************************************************************/
void BinarySearchTree::inOrderTraversal(TreeNode* node, QString& result) {
    if (node != nullptr) {
        inOrderTraversal(node->left, result);
        result += QString::number(node->value) + "(" + QString::number(node->depth) + ") ";
        inOrderTraversal(node->right, result);
    }
}

/***************************************************************************
  函数名称：BinarySearchTree::clearTree
  功    能：递归清空二叉搜索树
  输入参数：node - 当前节点指针
  返 回 值：
  说    明：递归删除左子树、右子树和当前节点
***************************************************************************/
void BinarySearchTree::clearTree(TreeNode* node) {
    if (node != nullptr) {
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
}

/***************************************************************************
  函数名称：BinarySearchTree::storeNodesInOrder
  功    能：递归按顺序存储节点值
  输入参数：node - 当前节点指针，values - 用于存储节点值的向量
  返 回 值：
  说    明：递归进行中序遍历，将节点值按顺序添加到向量中
***************************************************************************/
void BinarySearchTree::storeNodesInOrder(TreeNode* node, QVector<int>& values) {
    if (node == nullptr) {
        return;
    }

    storeNodesInOrder(node->left, values);
    values.append(node->value);
    storeNodesInOrder(node->right, values);
}

/***************************************************************************
  函数名称：BinarySearchTree::buildBalancedTree
  功    能：递归构建平衡二叉搜索树
  输入参数：values - 有序节点值向量，start - 起始索引，end - 结束索引，depth - 当前深度
  返 回 值：TreeNode* - 构建的平衡树根节点
  说    明：使用有序数组的中间值作为根节点，递归构建左右子树
***************************************************************************/
TreeNode* BinarySearchTree::buildBalancedTree(QVector<int>& values, int start, int end, int depth) {
    if (start > end) {
        return nullptr;
    }

    int mid = (start + end) / 2;
    TreeNode* node = new TreeNode(values[mid], depth);

    node->left = buildBalancedTree(values, start, mid - 1, depth + 1);
    node->right = buildBalancedTree(values, mid + 1, end, depth + 1);

    return node;
}

/***************************************************************************
  函数名称：BinarySearchTree::updateDepths
  功    能：递归更新节点深度
  输入参数：node - 当前节点指针，depth - 当前深度
  返 回 值：
  说    明：递归设置节点深度，并更新左右子树的深度
***************************************************************************/
void BinarySearchTree::updateDepths(TreeNode* node, int depth) {
    if (node == nullptr) {
        return;
    }

    node->depth = depth;
    updateDepths(node->left, depth + 1);
    updateDepths(node->right, depth + 1);
}

/***************************************************************************
  函数名称：BinarySearchTree::calculateHeight
  功    能：递归计算树高度
  输入参数：node - 当前节点指针
  返 回 值：int - 树的高度
  说    明：递归计算左右子树的高度，返回较大值加1
***************************************************************************/
int BinarySearchTree::calculateHeight(TreeNode* node) {
    if (node == nullptr) {
        return 0;
    }

    int leftHeight  = calculateHeight(node->left);
    int rightHeight = calculateHeight(node->right);

    return 1 + std::max(leftHeight, rightHeight);
}

/***************************************************************************
  函数名称：BinarySearchTree::balance
  功    能：平衡二叉搜索树
  输入参数：
  返 回 值：
  说    明：将树转换为有序数组，然后从中构建平衡树，发出树变化信号
***************************************************************************/
void BinarySearchTree::balance() {
    if (root == nullptr)
        return;

    // 将树转换为有序数组
    QVector<int> values;
    storeNodesInOrder(root, values);

    // 清空原树
    clearTree(root);

    // 从有序数组构建平衡树
    root = buildBalancedTree(values, 0, values.size() - 1, 1);

    emit treeChanged();
}

/***************************************************************************
  函数名称：BinarySearchTree::startFindAnimation
  功    能：开始查找动画
  输入参数：value - 要查找的值
  返 回 值：
  说    明：停止当前动画，生成查找动画步骤，启动动画定时器
***************************************************************************/
void BinarySearchTree::startFindAnimation(int value) {
    stopAnimation();
    isAnimationRunning = true;
    animationSteps.clear();
    animateFind(value);
    currentStep = 0;
    animationTimer->start(animationSpeed);
}

/***************************************************************************
  函数名称：BinarySearchTree::startInsertAnimation
  功    能：开始插入动画
  输入参数：value - 要插入的值
  返 回 值：
  说    明：停止当前动画，保存待插入值，生成插入动画步骤，启动动画定时器
***************************************************************************/
void BinarySearchTree::startInsertAnimation(int value) {
    stopAnimation();
    isAnimationRunning = true;
    animationSteps.clear();
    pendingInsertValue = value;
    animateInsertion(value);
    currentStep = 0;
    animationTimer->start(animationSpeed);
}

/***************************************************************************
  函数名称：BinarySearchTree::startDeleteAnimation
  功    能：开始删除动画
  输入参数：value - 要删除的值
  返 回 值：
  说    明：停止当前动画，保存待删除值，生成删除动画步骤，启动动画定时器
***************************************************************************/
void BinarySearchTree::startDeleteAnimation(int value) {
    stopAnimation();
    isAnimationRunning = true;
    animationSteps.clear();
    pendingDeleteValue = value;
    animateDeletion(value);
    currentStep = 0;
    animationTimer->start(animationSpeed);
}

/***************************************************************************
  函数名称：BinarySearchTree::startBalanceAnimation
  功    能：开始平衡动画
  输入参数：
  返 回 值：
  说    明：停止当前动画，生成平衡动画步骤，启动动画定时器
***************************************************************************/
void BinarySearchTree::startBalanceAnimation() {
    stopAnimation();
    isAnimationRunning = true;
    animationSteps.clear();
    animateBalancing();
    currentStep = 0;
    animationTimer->start(animationSpeed);
}

/***************************************************************************
  函数名称：BinarySearchTree::stopAnimation
  功    能：停止动画
  输入参数：
  返 回 值：
  说    明：停止动画定时器，清除高亮状态，设置动画运行状态为false
***************************************************************************/
void BinarySearchTree::stopAnimation() {
    if (animationTimer->isActive()) {
        animationTimer->stop();
    }
    isAnimationRunning = false;
    emit clearHighlights();
}

/***************************************************************************
  函数名称：BinarySearchTree::animateFind
  功    能：生成查找动画步骤
  输入参数：value - 要查找的值
  返 回 值：
  说    明：生成查找路径的每一步动画，包括查找过程和结果
***************************************************************************/
void BinarySearchTree::animateFind(int value) {
    currentPath.clear();  // 清空当前路径
    QVector<int> path;
    bool found = findNodeWithPath(root, value, path);

    // 添加查找路径步骤
    for (int i = 0; i < path.size(); i++) {
        // 更新当前路径
        QVector<int> stepPath;
        for (int j = 0; j <= i; j++) {
            stepPath.append(path[j]);
        }
        currentPath = stepPath;

        animationSteps.append(qMakePair(
            QString::fromUtf8("查找步骤 %1: 访问节点 %2").arg(i + 1).arg(path[i]),
            path[i]
        ));

        // 发送路径高亮信号
        emit highlightPath(stepPath);
    }

    if (found) {
        animationSteps.append(qMakePair(
            QString::fromUtf8("找到节点 %1").arg(value),
            value
        ));
    }
    else {
        animationSteps.append(qMakePair(
            QString::fromUtf8("未找到节点 %1").arg(value),
            -1
        ));
    }

    currentPath.clear();  // 清空当前路径
    emit highlightPath(QVector<int>());  // 发送空路径清除高亮
}

/***************************************************************************
  函数名称：BinarySearchTree::animateInsertion
  功    能：生成插入动画步骤
  输入参数：value - 要插入的值
  返 回 值：
  说    明：生成插入过程的动画步骤，包括查找插入位置和插入操作
***************************************************************************/
void BinarySearchTree::animateInsertion(int value) {
    currentPath.clear();  // 清空当前路径
    int depth = 0;
    QVector<int> path;
    bool exists = findNode(root, value, depth, path);

    if (exists) {
        animationSteps.append(qMakePair(
            QString::fromUtf8("节点 %1 已存在，无需插入").arg(value),
            value
        ));
        return;
    }

    // 添加查找插入位置的步骤
    for (int i = 0; i < path.size(); i++) {
        // 更新当前路径
        QVector<int> stepPath;
        for (int j = 0; j <= i; j++) {
            stepPath.append(path[j]);
        }
        currentPath = stepPath;

        animationSteps.append(qMakePair(
            QString::fromUtf8("查找插入位置步骤 %1: 访问节点 %2").arg(i + 1).arg(path[i]),
            path[i]
        ));

        // 发送路径高亮信号
        emit highlightPath(stepPath);
    }

    animationSteps.append(qMakePair(
        QString::fromUtf8("在正确位置插入新节点 %1").arg(value),
        value
    ));

    animationSteps.append(qMakePair(
        QString::fromUtf8("更新节点高度"),
        -1
    ));

    animationSteps.append(qMakePair(
        QString::fromUtf8("插入完成"),
        -1
    ));

    currentPath.clear();  // 清空当前路径
    emit highlightPath(QVector<int>());  // 发送空路径清除高亮
}

/***************************************************************************
  函数名称：BinarySearchTree::animateDeletion
  功    能：生成删除动画步骤
  输入参数：value - 要删除的值
  返 回 值：
  说    明：生成删除过程的动画步骤，包括查找节点和删除操作
***************************************************************************/
void BinarySearchTree::animateDeletion(int value) {
    currentPath.clear();  // 清空当前路径
    int depth = 0;
    QVector<int> path;
    bool exists = findNode(root, value, depth, path);

    if (!exists) {
        animationSteps.append(qMakePair(
            QString::fromUtf8("节点 %1 不存在，无法删除").arg(value),
            -1
        ));
        return;
    }

    // 添加查找节点的步骤
    for (int i = 0; i < path.size(); i++) {
        // 更新当前路径
        QVector<int> stepPath;
        for (int j = 0; j <= i; j++) {
            stepPath.append(path[j]);
        }
        currentPath = stepPath;

        animationSteps.append(qMakePair(
            QString::fromUtf8("查找删除节点步骤 %1: 访问节点 %2").arg(i + 1).arg(path[i]),
            path[i]
        ));

        // 发送路径高亮信号
        emit highlightPath(stepPath);
    }

    animationSteps.append(qMakePair(
        QString::fromUtf8("删除节点 %1").arg(value),
        value
    ));

    animationSteps.append(qMakePair(
        QString::fromUtf8("调整树结构"),
        -1
    ));

    animationSteps.append(qMakePair(
        QString::fromUtf8("更新节点高度"),
        -1
    ));

    animationSteps.append(qMakePair(
        QString::fromUtf8("删除完成"),
        -1
    ));

    currentPath.clear();  // 清空当前路径
    emit highlightPath(QVector<int>());  // 发送空路径清除高亮
}

/***************************************************************************
  函数名称：BinarySearchTree::animateBalancing
  功    能：生成平衡动画步骤
  输入参数：
  返 回 值：
  说    明：生成平衡过程的动画步骤，包括中序遍历和构建平衡树
***************************************************************************/
void BinarySearchTree::animateBalancing() {
    currentPath.clear();  // 清空当前路径
    if (root == nullptr) {
        animationSteps.append(qMakePair(
            QString::fromUtf8("树为空，无需平衡"),
            -1
        ));
        return;
    }

    animationSteps.append(qMakePair(
        QString::fromUtf8("开始平衡树"),
        -1
    ));

    animationSteps.append(qMakePair(
        QString::fromUtf8("中序遍历获取所有节点"),
        -1
    ));

    // 添加中序遍历步骤
    QVector<int> inOrderValues;
    storeNodesInOrderForAnimation(root, inOrderValues);

    for (int i = 0; i < inOrderValues.size(); i++) {
        QVector<int> stepPath;
        stepPath.append(inOrderValues[i]);
        currentPath = stepPath;

        animationSteps.append(qMakePair(
            QString::fromUtf8("中序遍历步骤 %1: 访问节点 %2").arg(i + 1).arg(inOrderValues[i]),
            inOrderValues[i]
        ));

        // 发送路径高亮信号
        emit highlightPath(stepPath);
    }

    animationSteps.append(qMakePair(
        QString::fromUtf8("构建平衡树"),
        -1
    ));

    animationSteps.append(qMakePair(
        QString::fromUtf8("更新节点高度"),
        -1
    ));

    animationSteps.append(qMakePair(
        QString::fromUtf8("平衡完成"),
        -1
    ));

    currentPath.clear();  // 清空当前路径
    emit highlightPath(QVector<int>());  // 发送空路径清除高亮
}

/***************************************************************************
  函数名称：BinarySearchTree::storeNodesInOrderForAnimation
  功    能：为动画按顺序存储节点值
  输入参数：node - 当前节点指针，values - 用于存储节点值的向量
  返 回 值：
  说    明：递归进行中序遍历，将节点值存储到向量中，用于动画显示
***************************************************************************/
void BinarySearchTree::storeNodesInOrderForAnimation(TreeNode* node, QVector<int>& values) {
    if (node == nullptr)
        return;

    storeNodesInOrderForAnimation(node->left, values);
    values.append(node->value);
    storeNodesInOrderForAnimation(node->right, values);
}

/***************************************************************************
  函数名称：BinarySearchTree::processNextAnimationStep
  功    能：处理下一个动画步骤
  输入参数：
  返 回 值：
  说    明：从动画步骤队列中取出下一个步骤并执行，如果所有步骤完成则执行实际操作
***************************************************************************/
void BinarySearchTree::processNextAnimationStep() {
    if (currentStep < animationSteps.size()) {
        QString description = animationSteps[currentStep].first;
        int highlightedValue = animationSteps[currentStep].second;

        emit animationStep(description, highlightedValue);

        // 如果这一步有高亮值，发送高亮信号
        if (highlightedValue != -1) {
            emit highlightNode(highlightedValue);
        }
        else {
            emit clearHighlights();
        }

        currentStep++;
    }
    else {
        stopAnimation();
        emit animationFinished();

        // 执行实际的操作
        if (animationSteps.size() > 0) {
            QString firstStep = animationSteps[0].first;

            if (firstStep.contains(QString::fromUtf8("插入"))) {
                insert(pendingInsertValue);
            }
            else if (firstStep.contains(QString::fromUtf8("删除"))) {
                remove(pendingDeleteValue);
            }
            else if (firstStep.contains(QString::fromUtf8("平衡"))) {
                balance();
            }
        }
    }
}