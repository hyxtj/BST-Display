/***************************************************************************
  文件名称：TreeNode.h
  功    能：树节点数据结构声明
  说    明：
***************************************************************************/
#ifndef TREENODE_H
#define TREENODE_H

// 二叉树节点定义
struct TreeNode {
    int       value;
    TreeNode* left;
    TreeNode* right;
    int       depth; // 节点深度（从根节点开始计算，根节点深度为1）

    TreeNode(int val, int d);
};

#endif // TREENODE_H