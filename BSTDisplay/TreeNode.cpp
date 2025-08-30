/***************************************************************************
  文件名称：TreeNode.cpp
  功    能：树节点类构造方法实现
  说    明：
***************************************************************************/
#include "TreeNode.h"

TreeNode::TreeNode(int val, int d) : 
	value(val), left(nullptr), right(nullptr), depth(d) {}