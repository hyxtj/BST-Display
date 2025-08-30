/***************************************************************************
  文件名称：BSTWindow.h
  功    能：二叉搜索树操作窗口类的头文件
  说    明：定义用户界面和操作控制接口，集成树操作和视图显示
***************************************************************************/

#ifndef BSTWINDOW_H
#define BSTWINDOW_H

#include <QWidget>
#include "BinarySearchTree.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>

class QLineEdit;
class QTextEdit;
class QPushButton;
class QSlider;
class BSTView;

class BSTWindow : public QWidget {
    Q_OBJECT

private:
    BinarySearchTree bst;           // 二叉搜索树实例

    //视图组件
    QLineEdit*   valueInput;        // 值输入框
    QTextEdit*   infoArea;          // 信息显示区域
    BSTView*     bstView;           // 树视图

    //数据操作按钮
    QPushButton* insertBtn;         // 插入按钮
    QPushButton* findBtn;           // 查找按钮
    QPushButton* deleteBtn;         // 删除按钮
    QPushButton* displayBtn;        // 显示按钮
    QPushButton* clearBtn;          // 清空按钮
    QPushButton* randomBtn;         // 随机生成按钮
    QPushButton* balanceBtn;        // 平衡按钮
    QPushButton* zoomInBtn;         // 放大按钮
    QPushButton* zoomOutBtn;        // 缩小按钮
    QPushButton* resetViewBtn;      // 重置视图按钮

    // 动画控制按钮
    QPushButton* animateFindBtn;        // 动画查找按钮
    QPushButton* animateInsertBtn;      // 动画插入按钮
    QPushButton* animateDeleteBtn;      // 动画删除按钮
    QPushButton* animateBalanceBtn;     // 动画平衡按钮
    QSlider*     animationSpeedSlider;  // 动画速度滑块

    QLineEdit*   countInput;        // 随机节点数量输入框
    QPushButton* randomCountBtn;    // 根据数量生成随机树按钮
    QLineEdit*   valuesInput;       // 自定义值输入框
    QPushButton* buildTreeBtn;      // 构建树按钮

    // 音效控制
    QMediaPlayer* backgroundMusic;  // 背景音乐播放器
    QAudioOutput* audioOutput;      // 音频输出
    QSoundEffect* touchSound;       // 触摸音效
    QSoundEffect* successSound;     // 成功音效
    bool          soundEnabled;     // 音效启用状态
    QPushButton*  soundToggleBtn;   // 音效切换按钮

    // 音效控制函数
    void initSoundEffects();      // 初始化音效
    void playTouchSound();        // 播放触摸音效
    void playSuccessSound();      // 播放成功音效
    void toggleSound(bool muted); // 切换音效状态

public:
    BSTWindow(QWidget* parent = nullptr); // 构造函数

private slots:
    // 树操作相关方法
    void insertValue();        // 插入值
    void findValue();          // 查找值
    void deleteValue();        // 删除值
    void displayTree();        // 显示树信息
    void clearTree();          // 清空树
    void generateRandomTree(); // 生成随机树
    void balanceTree();        // 平衡树

    // 视图控制相关方法
    void zoomIn();      // 放大视图
    void zoomOut();     // 缩小视图
    void resetView();   // 重置视图

    // 动画控制相关方法
    void animateFind();                   // 动画查找
    void animateInsert();                 // 动画插入
    void animateDelete();                 // 动画删除
    void animateBalance();                // 动画平衡
    void updateAnimationSpeed(int speed); // 更新动画速度
    void onAnimationFinished();           // 动画完成处理

    // 树生成相关方法
    void generateRandomTreeWithCount(); // 根据数量生成随机树
    void buildTreeFromValues();         // 根据自定义值构建树
};

#endif // BSTWINDOW_H