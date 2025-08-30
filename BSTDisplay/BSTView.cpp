/***************************************************************************
  文件名称：BSTView.cpp
  功    能：二叉搜索树可视化视图类的实现文件
  说    明：实现树的绘制、交互控制和动画效果展示
***************************************************************************/

#include "BSTView.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QQueue>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <cmath>
#include <algorithm>

/***************************************************************************
  函数名称：BSTView::BSTView
  功    能：构造函数，初始化视图参数
  输入参数：parent - 父窗口指针
  返 回 值：
  说    明：初始化布局参数、缩放因子和交互状态
***************************************************************************/
BSTView::BSTView(QWidget* parent) :
    QWidget(parent)  , bst(nullptr),
    zoomFactor(1.0)  , xOffset(0), yOffset(0),
    isDragging(false), lastDragPos(0, 0),
    treeWidth(0)     , treeHeight(0), 
    rootX(0)         , rootY(0),
    highlightedValue(-1)
{
    setMinimumSize(400, 300);
    setCursor(Qt::OpenHandCursor);

    // 初始化布局参数
    nodeSpacing  = 60;    // 节点间最小间距
    levelSpacing = 80;   // 层级间距
}

/***************************************************************************
  函数名称：BSTView::setTree
  功    能：设置要显示的二叉搜索树
  输入参数：tree - 二叉搜索树指针
  返 回 值：
  说    明：连接树的变化信号到视图的槽函数，并重新计算布局
***************************************************************************/
void BSTView::setTree(BinarySearchTree* tree) {
    if (bst) {
        disconnect(bst, &BinarySearchTree::treeChanged,     this, &BSTView::onTreeChanged);
        disconnect(bst, &BinarySearchTree::animationStep,   this, &BSTView::onAnimationStep);
        disconnect(bst, &BinarySearchTree::highlightNode,   this, &BSTView::onHighlightNode);
        disconnect(bst, &BinarySearchTree::clearHighlights, this, &BSTView::onClearHighlights);
        disconnect(bst, &BinarySearchTree::highlightPath,   this, &BSTView::onHighlightPath);
    }

    bst = tree;

    if (bst) {
        connect(bst, &BinarySearchTree::treeChanged,     this, &BSTView::onTreeChanged);
        connect(bst, &BinarySearchTree::animationStep,   this, &BSTView::onAnimationStep);
        connect(bst, &BinarySearchTree::highlightNode,   this, &BSTView::onHighlightNode);
        connect(bst, &BinarySearchTree::clearHighlights, this, &BSTView::onClearHighlights);
        connect(bst, &BinarySearchTree::highlightPath,   this, &BSTView::onHighlightPath);
    }

    calculatePositions();
    resetView();
    update();
}

/***************************************************************************
  函数名称：BSTView::sizeHint
  功    能：返回视图的建议大小
  输入参数：
  返 回 值：QSize - 建议大小
  说    明：返回默认的视图大小
***************************************************************************/
QSize BSTView::sizeHint() const {
    return QSize(800, 600);
}

/***************************************************************************
  函数名称：BSTView::paintEvent
  功    能：绘制事件处理函数
  输入参数：event - 绘制事件
  返 回 值：
  说    明：绘制树的可视化表示，包括节点和连接线
***************************************************************************/
void BSTView::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    /* 设置背景色为黑色*/
    painter.fillRect(rect(), Qt::black);

    // 显示当前动画步骤 - 改为白色文字
    if (!currentAnimationStep.isEmpty()) {
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setPointSize(12);
        painter.setFont(font);
        painter.drawText(10, 30, currentAnimationStep);
    }

    if (!bst || bst->isEmpty()) {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, QString::fromUtf8("树为空"));
        return;
    }

    // 应用缩放和滚动偏移
    painter.translate(width() / 2, height() / 2);
    painter.scale(zoomFactor, zoomFactor);
    painter.translate(-xOffset, -yOffset);

    drawTree(&painter);
}

/***************************************************************************
  函数名称：BSTView::getNodePosition
  功    能：获取指定值节点的位置信息
  输入参数：value - 节点值，x, y, size - 用于返回位置和大小
  返 回 值：bool - 是否找到节点
  说    明：在节点位置列表中查找指定值的节点
***************************************************************************/
bool BSTView::getNodePosition(int value, int& x, int& y, int& size) {
    for (const NodePosition& pos : nodePositions) {
        if (pos.node->value == value) {
            x = pos.x;
            y = pos.y;
            size = pos.size;
            return true;
        }
    }
    return false;
}

/***************************************************************************
  函数名称：BSTView::mousePressEvent
  功    能：鼠标按下事件处理
  输入参数：event - 鼠标事件
  返 回 值：
  说    明：开始拖拽操作，设置光标状态
***************************************************************************/
void BSTView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        lastDragPos = event->pos();
        setCursor(Qt::ClosedHandCursor);

        // 点击时清除高亮
        onClearHighlights();

        event->accept();
    }
}

/***************************************************************************
  函数名称：BSTView::mouseMoveEvent
  功    能：鼠标移动事件处理
  输入参数：event - 鼠标事件
  返 回 值：
  说    明：处理拖拽操作，更新视图偏移
***************************************************************************/
void BSTView::mouseMoveEvent(QMouseEvent* event) {
    if (isDragging) {
        QPoint delta = event->pos() - lastDragPos;
        xOffset -= delta.x() / zoomFactor;
        yOffset -= delta.y() / zoomFactor;
        lastDragPos = event->pos();
        update();
        event->accept();
    }
}

/***************************************************************************
  函数名称：BSTView::mouseReleaseEvent
  功    能：鼠标释放事件处理
  输入参数：event - 鼠标事件
  返 回 值：
  说    明：结束拖拽操作，恢复光标状态
***************************************************************************/
void BSTView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && isDragging) {
        isDragging = false;
        setCursor(Qt::OpenHandCursor);
        event->accept();
    }
}

/***************************************************************************
  函数名称：BSTView::wheelEvent
  功    能：滚轮事件处理
  输入参数：event - 滚轮事件
  返 回 值：
  说    明：处理缩放操作，以鼠标位置为中心进行缩放
***************************************************************************/
void BSTView::wheelEvent(QWheelEvent* event) {
    // 保存鼠标位置（在视图坐标系中）
    QPoint mousePos = event->position().toPoint();

    // 计算鼠标位置在树坐标系中的位置
    double treeX = (mousePos.x() - width() / 2) / zoomFactor + xOffset;
    double treeY = (mousePos.y() - height() / 2) / zoomFactor + yOffset;

    // 根据滚轮方向调整缩放因子
    const double zoomChange = 1.1;
    if (event->angleDelta().y() > 0) {
        // 放大
        zoomFactor *= zoomChange;
        zoomFactor = std::min(zoomFactor, 3.0);
    }
    else {
        // 缩小
        zoomFactor /= zoomChange;
        zoomFactor = std::max(zoomFactor, 0.3);
    }

    // 计算新的偏移量，使鼠标位置在树坐标系中保持不变
    xOffset = treeX - (mousePos.x() - width() / 2) / zoomFactor;
    yOffset = treeY - (mousePos.y() - height() / 2) / zoomFactor;

    update();
    event->accept();
}

/***************************************************************************
  函数名称：BSTView::zoomIn
  功    能：放大视图
  输入参数：
  返 回 值：
  说    明：以视图中心为基准放大视图
***************************************************************************/
void BSTView::zoomIn() {
    // 以视图中心为缩放中心
    double treeX = xOffset;
    double treeY = yOffset;

    // 放大
    const double zoomChange = 1.1;
    zoomFactor *= zoomChange;
    zoomFactor = std::min(zoomFactor, 3.0);

    // 计算新的偏移量，使视图中心保持不变
    xOffset = treeX;
    yOffset = treeY;

    update();
}

/***************************************************************************
  函数名称：BSTView::zoomOut
  功    能：缩小视图
  输入参数：
  返 回 值：
  说    明：以视图中心为基准缩小视图
***************************************************************************/
void BSTView::zoomOut() {
    // 以视图中心为缩放中心
    double treeX = xOffset;
    double treeY = yOffset;

    // 缩小
    const double zoomChange = 1.1;
    zoomFactor /= zoomChange;
    zoomFactor = std::max(zoomFactor, 0.3);

    // 计算新的偏移量，使视图中心保持不变
    xOffset = treeX;
    yOffset = treeY;

    update();
}

/***************************************************************************
  函数名称：BSTView::resetView
  功    能：重置视图
  输入参数：
  返 回 值：
  说    明：重新计算节点位置并重置视图到初始状态
***************************************************************************/
void BSTView::resetView() {
    if (!bst || bst->isEmpty()) 
        return;

    // 重新计算位置
    calculatePositions();

    // lambda表达式匿名函数:找到根节点的位置
    auto rootIt = std::find_if(nodePositions.begin(), nodePositions.end(),
        [this](const NodePosition& np) { 
            return np.node == bst->getRoot(); 
        });

    if (rootIt != nodePositions.end()) {
        rootX = rootIt->x;
        rootY = rootIt->y;

        // 重置偏移量为根节点位置
        xOffset = rootX;
        yOffset = rootY;
    }

    // 自动调整缩放
    adjustZoom();
}

/***************************************************************************
  函数名称：BSTView::adjustZoom
  功    能：自动调整缩放
  输入参数：
  返 回 值：
  说    明：根据树的大小自动调整缩放因子
***************************************************************************/
void BSTView::adjustZoom() {
    if (!bst || bst->isEmpty()) 
        return;

    // 根据树的大小自动调整缩放
    double widthRatio = static_cast<double>(width()) / treeWidth;
    double heightRatio = static_cast<double>(height()) / treeHeight;

    zoomFactor = std::min(widthRatio, heightRatio) * 0.8;
    zoomFactor = std::max(0.3, std::min(zoomFactor, 1.0));

    update();
}

/***************************************************************************
  函数名称：BSTView::calculatePositions
  功    能：计算节点位置
  输入参数：
  返 回 值：
  说    明：使用对称布局算法计算所有节点的位置
***************************************************************************/
void BSTView::calculatePositions() {
    nodePositions.clear();

    if (!bst || bst->isEmpty()) 
        return;

    // 使用对称布局算法
    calculateSymmetricLayout();

    //lambda匿名函数:找到根节点的位置
    auto rootIt = std::find_if(nodePositions.begin(), nodePositions.end(),
        [this](const NodePosition& np) { 
            return np.node == bst->getRoot();
        });

    if (rootIt != nodePositions.end()) {
        rootX = rootIt->x;
        rootY = rootIt->y;
    }
}

/***************************************************************************
  函数名称：BSTView::calculateNodeSize
  功    能：计算节点大小
  输入参数：
  返 回 值：int - 节点大小
  说    明：根据树的深度和节点数量动态计算节点大小
***************************************************************************/
int BSTView::calculateNodeSize() {
    if (!bst || bst->isEmpty()) 
        return 30;

    int maxDepth = getTreeDepth(bst->getRoot());
    int nodeCount = nodePositions.size();

    // 根据树的深度和节点数量动态计算节点大小
    const int baseSize = 40;
    int size = baseSize - std::min(10, maxDepth) - std::min(5, nodeCount / 10);

    return std::max(20, size); // 确保最小大小为20
}

/***************************************************************************
  函数名称：BSTView::calculateSubtreeWidth
  功    能：计算子树宽度
  输入参数：node - 子树根节点，level - 当前层级
  返 回 值：int - 子树宽度
  说    明：递归计算子树的宽度，用于布局计算
***************************************************************************/
int BSTView::calculateSubtreeWidth(TreeNode* node, int level) {
    if (!node) 
        return 0;

    // 为每个节点分配固定宽度
    int nodeWidth = calculateNodeSize() * 2;

    if (!node->left && !node->right) {
        return nodeWidth;
    }

    int leftWidth  = calculateSubtreeWidth(node->left, level + 1);
    int rightWidth = calculateSubtreeWidth(node->right, level + 1);

    return leftWidth + nodeWidth + rightWidth;
}

/***************************************************************************
  函数名称：BSTView::positionNode
  功    能：定位节点
  输入参数：node - 要定位的节点，x, y - 节点坐标，level - 当前层级，hGap - 水平间距
  返 回 值：
  说    明：递归定位节点及其子节点
***************************************************************************/
void BSTView::positionNode(TreeNode* node, int x, int y, int level, int hGap) {
    if (!node) 
        return;

    // 计算当前节点的水平偏移量
    int offset = hGap / (level * 2);

    // 计算节点大小
    int nodeSize = calculateNodeSize();

    // 记录节点位置
    NodePosition pos;
    pos.node = node;
    pos.x    = x;
    pos.y    = y;
    pos.size = nodeSize;
    nodePositions.append(pos);

    // 递归定位左右子树
    int verticalGap = nodeSize * 2;
    if (node->left) {
        positionNode(node->left , x - offset, y + verticalGap, level + 1, hGap);
    }

    if (node->right) {
        positionNode(node->right, x + offset, y + verticalGap, level + 1, hGap);
    }
}

/***************************************************************************
  函数名称：BSTView::getTreeDepth
  功    能：获取树深度
  输入参数：node - 树根节点
  返 回 值：int - 树深度
  说    明：递归计算树的最大深度
***************************************************************************/
int BSTView::getTreeDepth(TreeNode* node) {
    if (!node) 
        return 0;

    int leftDepth  = getTreeDepth(node->left);
    int rightDepth = getTreeDepth(node->right);

    return 1 + std::max(leftDepth, rightDepth);
}

/***************************************************************************
  函数名称：BSTView::drawTree
  功    能：绘制树
  输入参数：painter - 绘制器指针
  返 回 值：
  说    明：绘制树的所有节点和连接线，包括高亮效果
***************************************************************************/

void BSTView::drawTree(QPainter* painter) {
    // 先绘制所有连接线
    for (const NodePosition& pos : nodePositions) {
        TreeNode* node = pos.node;

        // 绘制左子节点连接线
        if (node->left) {
            auto leftIt = std::find_if(nodePositions.begin(), nodePositions.end(),
                [node](const NodePosition& np) {
                    return np.node == node->left;
                });

            if (leftIt != nodePositions.end()) {
                // 检查这条连线是否在高亮路径上
                bool isHighlighted = isConnectionHighlighted(node->value, node->left->value);

                if (isHighlighted) {
                    painter->setPen(QPen(QColor(0, 255, 255), 3, Qt::SolidLine, Qt::RoundCap)); // 青色高亮
                }
                else {
                    painter->setPen(QPen(QColor(100, 100, 100), 2, Qt::SolidLine, Qt::RoundCap)); // 灰色
                }

                // 绘制直线
                painter->drawLine(pos.x, pos.y + pos.size / 2,
                    leftIt->x, leftIt->y + leftIt->size / 2);
            }
        }

        // 绘制右子节点连接线
        if (node->right) {
            auto rightIt = std::find_if(nodePositions.begin(), nodePositions.end(),
                [node](const NodePosition& np) {
                    return np.node == node->right;
                });

            if (rightIt != nodePositions.end()) {
                // 检查这条连线是否在高亮路径上
                bool isHighlighted = isConnectionHighlighted(node->value, node->right->value);

                if (isHighlighted) {
                    painter->setPen(QPen(QColor(0, 255, 255), 3, Qt::SolidLine, Qt::RoundCap)); // 青色高亮
                }
                else {
                    painter->setPen(QPen(QColor(100, 100, 100), 2, Qt::SolidLine, Qt::RoundCap)); // 灰色
                }

                // 绘制直线
                painter->drawLine(pos.x, pos.y + pos.size / 2,
                    rightIt->x, rightIt->y + rightIt->size / 2);
            }
        }
    }

    // 绘制所有节点
    for (const NodePosition& pos : nodePositions) {
        // 绘制节点圆形
        QPen pen(Qt::white, 2);
        painter->setPen(pen);

        // 根据节点深度选择颜色 - 使用更亮的颜色以适应黑色背景
        int depth = pos.node->depth;
        int hue = (depth * 30) % 360;
        QColor nodeColor = QColor::fromHsv(hue, 200, 255); // 增加饱和度和亮度

        // 如果是高亮节点，使用不同的颜色
        if (pos.node->value == highlightedValue || highlightedPath.contains(pos.node->value)) {
            nodeColor = QColor(255, 100, 100); // 红色高亮
        }

        QBrush brush(nodeColor);
        painter->setBrush(brush);

        painter->drawEllipse(pos.x - pos.size / 2, pos.y - pos.size / 2, pos.size, pos.size);

        // 绘制节点值 - 使用黑色文字
        painter->setPen(Qt::black);
        QFont font = painter->font();
        font.setPointSize(pos.size / 3);
        font.setBold(true);
        painter->setFont(font);
        painter->drawText(QRect(pos.x - pos.size / 2, pos.y - pos.size / 2, pos.size, pos.size),
            Qt::AlignCenter, QString::number(pos.node->value));

        // 绘制节点深度 - 使用浅灰色文字
        painter->setPen(QColor(200, 200, 200));
        font.setPointSize(pos.size / 5);
        font.setBold(false);
        painter->setFont(font);
        painter->drawText(QRect(pos.x - pos.size / 2, pos.y + pos.size / 2, pos.size, 20),
            Qt::AlignCenter, "d:" + QString::number(pos.node->depth));
    }
}

/***************************************************************************
  函数名称：BSTView::onTreeChanged
  功    能：树变化响应
  输入参数：
  返 回 值：
  说    明：当树结构变化时重新计算节点位置并更新视图
***************************************************************************/
void BSTView::onTreeChanged() {
    calculatePositions();
    update();
}

/***************************************************************************
  函数名称：BSTView::onAnimationStep
  功    能：动画步骤响应
  输入参数：description - 步骤描述，highlightedValue - 高亮节点值
  返 回 值：
  说    明：更新当前动画步骤并设置高亮节点
***************************************************************************/
void BSTView::onAnimationStep(QString description, int highlightedValue) {
    currentAnimationStep = description;
    setHighlightedNode(highlightedValue);
    update();
}

/***************************************************************************
  函数名称：BSTView::onHighlightNode
  功    能：高亮节点响应
  输入参数：value - 要高亮的节点值
  返 回 值：
  说    明：设置指定节点为高亮状态
***************************************************************************/
void BSTView::onHighlightNode(int value) {
    setHighlightedNode(value);
    update();
}

/***************************************************************************
  函数名称：BSTView::onClearHighlights
  功    能：清除高亮响应
  输入参数：
  返 回 值：
  说    明：清除所有节点的高亮状态
***************************************************************************/
void BSTView::onClearHighlights() {
    clearHighlight();
    update();
}

/***************************************************************************
  函数名称：BSTView::setHighlightedNode
  功    能：设置高亮节点
  输入参数：value - 要高亮的节点值
  返 回 值：
  说    明：设置高亮节点值并发出高亮信号
***************************************************************************/
void BSTView::setHighlightedNode(int value) {
    highlightedValue = value;
    if (value != -1) {
        emit nodeHighlighted();  // 发射高亮信号
    }
}

/***************************************************************************
  函数名称：BSTView::clearHighlight
  功    能：清除高亮
  输入参数：
  返 回 值：
  说    明：清除高亮节点值
***************************************************************************/
void BSTView::clearHighlight() {
    highlightedValue = -1;
}

/***************************************************************************
  函数名称：BSTView::onHighlightPath
  功    能：高亮路径响应
  输入参数：path - 要高亮的路径
  返 回 值：
  说    明：设置高亮路径并更新高亮连接
***************************************************************************/
void BSTView::onHighlightPath(const QVector<int>& path) {
    highlightedPath = path;
    updateHighlightedConnections();
    update();
}

/***************************************************************************
  函数名称：BSTView::updateHighlightedConnections
  功    能：更新高亮连接
  输入参数：
  返 回 值：
  说    明：将高亮路径转换为连接对
***************************************************************************/
void BSTView::updateHighlightedConnections() {
    highlightedConnections.clear();

    if (highlightedPath.size() < 2)
        return;

    // 将路径转换为连接对
    for (int i = 0; i < highlightedPath.size() - 1; i++) {
        highlightedConnections.append(qMakePair(highlightedPath[i], highlightedPath[i + 1]));
    }
}

/***************************************************************************
  函数名称：BSTView::isConnectionHighlighted
  功    能：检查连接是否高亮
  输入参数：parentValue - 父节点值，childValue - 子节点值
  返 回 值：bool - 连接是否高亮
  说    明：检查指定连接是否在高亮连接列表中
***************************************************************************/
bool BSTView::isConnectionHighlighted(int parentValue, int childValue) const {
    for (const auto& connection : highlightedConnections) {
        if ((connection.first == parentValue && connection.second == childValue) ||
            (connection.first == childValue && connection.second == parentValue)) {
            return true;
        }
    }
    return false;
}

/***************************************************************************
  函数名称：BSTView::calculateTreeLayout
  功    能：计算树布局
  输入参数：node - 当前节点，level - 当前层级，x - 当前X坐标，minX - 最小X坐标
  返 回 值：
  说    明：递归计算树的布局，避免节点重叠
***************************************************************************/
void BSTView::calculateTreeLayout(TreeNode* node, int level, int& x, int& minX) {
    if (!node) 
        return;

    // 递归处理左子树
    int leftX = x;
    int leftMinX = INT_MAX;
    calculateTreeLayout(node->left, level + 1, leftX, leftMinX);

    // 处理当前节点
    if (node->left) {
        x = leftX + siblingSpacing;
    }

    // 记录当前节点的位置
    setNodePosition(node, QPoint(x, level * levelSpacing));

    // 更新最小X坐标
    minX = std::min(minX, x);

    // 递归处理右子树
    int rightX = x + siblingSpacing;
    int rightMinX = INT_MAX;
    calculateTreeLayout(node->right, level + 1, rightX, rightMinX);

    // 调整右子树位置，避免与左子树重叠
    if (node->left && node->right) {
        int leftMaxX = leftX;
        for (const auto& pair : tempPositions) {
            if (isNodeInSubtree(pair.first, node->left)) {
                leftMaxX = std::max(leftMaxX, pair.second.x());
            }
        }

        int rightShift = (leftMaxX + siblingSpacing) - rightMinX;
        if (rightShift > 0) {
            shiftSubtree(node->right, rightShift);
            rightX += rightShift;
        }
    }

    // 更新X坐标
    x = rightX;
}

/***************************************************************************
  函数名称：BSTView::isNodeInSubtree
  功    能：检查节点是否在子树中
  输入参数：node - 要检查的节点，root - 子树根节点
  返 回 值：bool - 节点是否在子树中
  说    明：递归检查指定节点是否在给定子树中
***************************************************************************/
bool BSTView::isNodeInSubtree(TreeNode* node, TreeNode* root) {
    if (!root) 
        return false;

    if (node == root) 
        return true;

    return isNodeInSubtree(node, root->left) || 
               isNodeInSubtree(node, root->right);
}

/***************************************************************************
  函数名称：BSTView::shiftSubtree
  功    能：移动子树
  输入参数：node - 子树根节点，shift - 移动距离
  返 回 值：
  说    明：递归移动子树中的所有节点
***************************************************************************/
void BSTView::shiftSubtree(TreeNode* node, int shift) {
    if (!node) 
        return;

    QPoint pos = getNodePosition(node);
    if (!pos.isNull()) {
        setNodePosition(node, QPoint(pos.x() + shift, pos.y()));
    }

    shiftSubtree(node->left, shift);
    shiftSubtree(node->right, shift);
}

/***************************************************************************
  函数名称：BSTView::calculateSubtreeWidth
  功    能：计算子树宽度
  输入参数：node - 子树根节点
  返 回 值：int - 子树宽度
  说    明：递归计算子树的宽度
***************************************************************************/
int BSTView::calculateSubtreeWidth(TreeNode* node) {
    if (!node) 
        return 0;

    int leftWidth  = calculateSubtreeWidth(node->left);
    int rightWidth = calculateSubtreeWidth(node->right);

    return leftWidth + siblingSpacing + rightWidth;
}

/***************************************************************************
  函数名称：BSTView::centerChildNodes
  功    能：居中子节点
  输入参数：node - 父节点，parentX - 父节点X坐标
  返 回 值：
  说    明：调整子节点位置使其相对于父节点居中
***************************************************************************/
void BSTView::centerChildNodes(TreeNode* node, int parentX) {
    if (!node) 
        return;

    QPoint nodePos = getNodePosition(node);
    if (nodePos.isNull()) 
        return;

    if (node->left && node->right) {
        QPoint leftPos  = getNodePosition(node->left);
        QPoint rightPos = getNodePosition(node->right);

        if (!leftPos.isNull() && !rightPos.isNull()) {
            int centerX = (leftPos.x() + rightPos.x()) / 2;

            if (centerX != parentX) {
                int shift = parentX - centerX;
                shiftSubtree(node, shift);
            }
        }
    }
    else if (node->left) {
        QPoint leftPos = getNodePosition(node->left);
        if (!leftPos.isNull() && abs(leftPos.x() - parentX) > siblingSpacing / 2) {
            int shift = parentX - leftPos.x();
            shiftSubtree(node->left, shift);
        }
    }
    else if (node->right) {
        QPoint rightPos = getNodePosition(node->right);
        if (!rightPos.isNull() && abs(rightPos.x() - parentX) > siblingSpacing / 2) {
            int shift = parentX - rightPos.x();
            shiftSubtree(node->right, shift);
        }
    }

    // 递归处理子节点
    if (node->left) {
        QPoint leftPos = getNodePosition(node->left);
        if (!leftPos.isNull()) {
            centerChildNodes(node->left, leftPos.x());
        }
    }

    if (node->right) {
        QPoint rightPos = getNodePosition(node->right);
        if (!rightPos.isNull()) {
            centerChildNodes(node->right, rightPos.x());
        }
    }
}

/***************************************************************************
  函数名称：BSTView::adjustTreeLayout
  功    能：调整树布局
  输入参数：
  返 回 值：
  说    明：调整树布局使整体居中
***************************************************************************/
void BSTView::adjustTreeLayout() {
    if (!bst || !bst->getRoot()) 
        return;

    // 计算树的高度
    int treeDepth = getTreeDepth(bst->getRoot());

    // 第一次遍历：计算初始位置
    int x = 0;
    int minX = INT_MAX;
    tempPositions.clear();
    calculateTreeLayout(bst->getRoot(), 0, x, minX);

    // 第二次遍历：居中子节点
    centerChildNodes(bst->getRoot(), getNodePosition(bst->getRoot()).x());

    // 计算整体偏移，使树居中
    int minTreeX = INT_MAX;
    int maxTreeX = INT_MIN;
    for (const auto& pair : tempPositions) {
        minTreeX = std::min(minTreeX, pair.second.x());
        maxTreeX = std::max(maxTreeX, pair.second.x());
    }

    int centerOffset = (minTreeX + maxTreeX) / 2;
    for (auto& pair : tempPositions) {
        pair.second = QPoint(pair.second.x() - centerOffset, pair.second.y());
    }

    // 更新节点位置
    nodePositions.clear();
    for (const auto& pair : tempPositions) {
        NodePosition pos;
        pos.node = pair.first;
        pos.x    = pair.second.x();
        pos.y    = pair.second.y();
        pos.size = calculateNodeSize();
        nodePositions.append(pos);
    }

    // 计算树的总宽度和高度
    treeWidth  = maxTreeX - minTreeX + calculateNodeSize();
    treeHeight = treeDepth * levelSpacing + calculateNodeSize();
}

/***************************************************************************
  函数名称：BSTView::getNodePosition
  功    能：获取节点位置
  输入参数：node - 要获取位置的节点
  返 回 值：QPoint - 节点位置
  说    明：从临时位置列表中获取指定节点的位置
***************************************************************************/
QPoint BSTView::getNodePosition(TreeNode* node) const {
    for (const auto& pair : tempPositions) {
        if (pair.first == node) {
            return pair.second;
        }
    }
    return QPoint(); // 返回空点表示未找到
}

/***************************************************************************
  函数名称：BSTView::setNodePosition
  功    能：设置节点位置
  输入参数：node - 要设置位置的节点，position - 节点位置
  返 回 值：
  说    明：设置指定节点的位置，如果节点不存在则添加新条目
***************************************************************************/
void BSTView::setNodePosition(TreeNode* node, const QPoint& position) {
    for (auto& pair : tempPositions) {
        if (pair.first == node) {
            pair.second = position;
            return;
        }
    }
    // 如果节点不存在，添加新条目
    tempPositions.append(qMakePair(node, position));
}

/***************************************************************************
  函数名称：BSTView::calculateSymmetricLayout
  功    能：计算对称布局
  输入参数：
  返 回 值：
  说    明：使用对称布局算法计算节点位置
***************************************************************************/
void BSTView::calculateSymmetricLayout() {
    nodePositions.clear();

    if (!bst || bst->isEmpty()) 
        return;

    // 计算树的高度
    int treeDepth = getTreeDepth(bst->getRoot());

    // 计算每个子树所需的宽度
    int treeWidth = calculateSymmetricSubtreeWidth(bst->getRoot());

    // 定位根节点（居中）
    int rootX = 0;
    int rootY = 0;

    // 定位所有节点
    positionSymmetricNode(bst->getRoot(), rootX, rootY, 0);

    // 更新树的总宽度和高度
    this->treeWidth  = treeWidth;
    this->treeHeight = treeDepth * levelSpacing;
}

/***************************************************************************
  函数名称：BSTView::calculateSymmetricSubtreeWidth
  功    能：计算对称子树宽度
  输入参数：node - 子树根节点
  返 回 值：int - 子树宽度
  说    明：递归计算对称布局中子树的宽度
***************************************************************************/
int BSTView::calculateSymmetricSubtreeWidth(TreeNode* node) {
    if (!node) 
        return 0;

    // 计算左右子树的宽度
    int leftWidth  = calculateSymmetricSubtreeWidth(node->left);
    int rightWidth = calculateSymmetricSubtreeWidth(node->right);

    // 返回最大宽度或组合宽度
    if (leftWidth == 0 && rightWidth == 0) {
        return nodeSpacing;
    }

    return leftWidth + rightWidth + nodeSpacing;
}

/***************************************************************************
  函数名称：BSTView::positionSymmetricNode
  功    能：对称定位节点
  输入参数：node - 要定位的节点，x, y - 节点坐标，level - 当前层级
  返 回 值：
  说    明：使用对称布局算法递归定位节点及其子节点
***************************************************************************/
void BSTView::positionSymmetricNode(TreeNode* node, int x, int y, int level) {
    if (!node) 
        return;

    // 计算节点大小
    int nodeSize = calculateNodeSize();

    // 记录节点位置
    NodePosition pos;
    pos.node = node;
    pos.x    = x;
    pos.y    = y;
    pos.size = nodeSize;
    nodePositions.append(pos);

    // 计算左右子树的宽度
    int leftWidth  = calculateSymmetricSubtreeWidth(node->left);
    int rightWidth = calculateSymmetricSubtreeWidth(node->right);

    // 计算子节点的位置
    int childY = y + levelSpacing;

    if (node->left) {
        // 左子节点的x坐标 = 当前节点x坐标 - (右子树宽度 + 节点间距)/2
        int leftX = x - (rightWidth + nodeSpacing) / 2;
        positionSymmetricNode(node->left, leftX, childY, level + 1);
    }

    if (node->right) {
        // 右子节点的x坐标 = 当前节点x坐标 + (左子树宽度 + 节点间距)/2
        int rightX = x + (leftWidth + nodeSpacing) / 2;
        positionSymmetricNode(node->right, rightX, childY, level + 1);
    }
}