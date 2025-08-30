/***************************************************************************
  文件名称：BSTView.h
  功    能：二叉搜索树可视化视图类的头文件
  说    明：定义树的可视化显示、交互控制和动画效果接口
***************************************************************************/

#ifndef BSTVIEW_H
#define BSTVIEW_H

#include <QWidget>
#include <QScrollArea>
#include "BinarySearchTree.h"

class BSTView : public QWidget {
    Q_OBJECT

public:
    explicit BSTView(QWidget* parent = nullptr); // 构造函数
    void     setTree(BinarySearchTree* tree);    // 设置要显示的树
    QSize    sizeHint() const override;          // 设置视图大小

    // 视图控制方法
    void zoomIn();    // 放大视图
    void zoomOut();   // 缩小视图
    void resetView(); // 重置视图

    // 动画控制
    void setHighlightedNode(int value); // 设置高亮节点
    void clearHighlight();              // 清除高亮

public slots:
    void onTreeChanged();                                            // 树变化响应
    void onAnimationStep(QString description, int highlightedValue); // 动画步骤响应
    void onHighlightNode(int value);                                 // 高亮节点响应
    void onClearHighlights();                                        // 清除高亮响应

protected:
    void paintEvent(QPaintEvent* event) override;        // 绘制事件
    void mousePressEvent(QMouseEvent* event) override;   // 鼠标按下事件
    void mouseMoveEvent(QMouseEvent* event) override;    // 鼠标移动事件
    void mouseReleaseEvent(QMouseEvent* event) override; // 鼠标释放事件
    void wheelEvent(QWheelEvent* event) override;        // 滚轮事件

private:
    BinarySearchTree* bst; // 二叉搜索树指针

    // 缩放和滚动参数
    double zoomFactor; // 缩放因子
    int xOffset;       // X轴偏移
    int yOffset;       // Y轴偏移

    // 拖拽相关变量
    bool   isDragging;  // 是否正在拖拽
    QPoint lastDragPos; // 上次拖拽位置

    // 动画相关
    int     highlightedValue;     // 高亮节点值
    QString currentAnimationStep; // 当前动画步骤描述

    // 节点位置信息
    struct NodePosition {
        TreeNode* node; // 节点指针
        int x, y;       // 节点坐标
        int size;       // 节点大小
    };

    QList<NodePosition> nodePositions; // 节点位置列表

    // 布局计算相关方法
    void calculatePositions();                                            // 计算节点位置
    int  calculateSubtreeWidth(TreeNode* node, int level);                // 计算子树宽度
    void positionNode(TreeNode* node, int x, int y, int level, int hGap); // 定位节点
    int  getTreeDepth(TreeNode* node);                                    // 获取树深度

    // 树的总宽度和高度
    int treeWidth;  // 树宽度
    int treeHeight; // 树高度

    // 动态调整节点大小
    int calculateNodeSize(); // 计算节点大小

    // 调整视图
    void adjustZoom(); // 调整缩放

    // 根节点位置
    int rootX, rootY; // 根节点坐标

    // 获取节点位置
    bool getNodePosition(int value, int& x, int& y, int& size); // 获取节点位置信息

    // 高亮路径相关
    QVector<int> highlightedPath;                    // 高亮路径
    QVector<QPair<int, int>> highlightedConnections; // 高亮连接

    // 高亮路径辅助方法
    void updateHighlightedConnections();                                 // 更新高亮连接
    bool isConnectionHighlighted(int parentValue, int childValue) const; // 检查连接是否高亮
    void onHighlightPath(const QVector<int>& path);                      // 高亮路径响应

    // 布局算法辅助方法
    bool isNodeInSubtree(TreeNode* node, TreeNode* root); // 检查节点是否在子树中
    void shiftSubtree(TreeNode* node, int shift);         // 移动子树

    // 布局算法参数
    int levelSpacing;       // 层级间距
    int siblingSpacing;     // 兄弟节点间距
    int nodeSpacing;        // 节点间最小间距

    // 布局算法辅助方法
    void calculateTreeLayout(TreeNode* node, int level, int& x, int& minX); // 计算树布局
    void adjustTreeLayout();                                                // 调整树布局
    int  calculateSubtreeWidth(TreeNode* node);                             // 计算子树宽度
    void centerChildNodes(TreeNode* node, int parentX);                     // 居中子节点

    // 对称布局方法
    void calculateSymmetricLayout();                                     // 计算对称布局
    int  calculateSymmetricSubtreeWidth(TreeNode* node);                 // 计算对称子树宽度
    void positionSymmetricNode(TreeNode* node, int x, int y, int level); // 对称定位节点

    QVector<QPair<TreeNode*, QPoint>> tempPositions; // 临时位置存储

    // 节点位置辅助方法
    QPoint getNodePosition(TreeNode* node) const;                   // 获取节点位置
    void   setNodePosition(TreeNode* node, const QPoint& position); // 设置节点位置

    // 绘制方法
    void drawTree(QPainter* painter); // 绘制树

signals:
    void nodeHighlighted(); // 节点高亮信号
};

#endif // BSTVIEW_H