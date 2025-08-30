/***************************************************************************
  文件名称：BSTWindow.cpp
  功    能：二叉搜索树操作窗口类的实现文件
  说    明：实现用户界面布局、操作控制和音效功能
***************************************************************************/

#include "BSTWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>
#include <QSplitter>
#include <QScrollArea>
#include <QTime>
#include <QSlider>
#include <Qvalidator>
#include <Qcoreapplication>
#include <QGroupBox>
#include <QStatusBar>
#include "BSTView.h"

/***************************************************************************
  函数名称：BSTWindow::BSTWindow
  功    能：构造函数，初始化用户界面
  输入参数：parent - 父窗口指针
  返 回 值：
  说    明：创建和布局所有UI组件，连接信号和槽
***************************************************************************/
BSTWindow::BSTWindow(QWidget* parent) : QWidget(parent), bst(this) {
    /* 设置应用程序样式 - 使用深色科技主题*/
    setStyleSheet(R"(
        QWidget {
            background-color: #1e1e2e;
            color: #cdd6f4;
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        
        QLabel {
            background: transparent;
            color: #cdd6f4;
            font-size: 11px;
        }
        
        QLineEdit, QTextEdit {
            background-color: #313244;
            border: 1px solid #45475a;
            border-radius: 4px;
            padding: 4px;
            color: #cdd6f4;
            font-size: 11px;
            selection-background-color: #89b4fa;
            min-height: 20px;
        }
        
        QLineEdit:focus, QTextEdit:focus {
            border-color: #89b4fa;
        }
        
        QPushButton {
            background-color: #313244;
            border: 1px solid #45475a;
            border-radius: 4px;
            padding: 4px 8px;
            color: #cdd6f4;
            font-weight: bold;
            font-size: 12px;
            min-height: 22px;
            min-width: 50px;
        }
        
        QPushButton:hover {
            background-color: #45475a;
            border-color: #89b4fa;
        }
        
        QPushButton:pressed {
            background-color: #585b70;
        }
        
        QPushButton:checked {
            background-color: #89b4fa;
            color: #1e1e2e;
        }
        
        QSlider::groove:horizontal {
            background: #313244;
            height: 4px;
            border-radius: 2px;
        }
        
        QSlider::handle:horizontal {
            background: #89b4fa;
            width: 13px;
            margin: -4px 0;
            border-radius: 6px;
        }
        
        QSlider::sub-page:horizontal {
            background: #89b4fa;
            border-radius: 2px;
        }
        
        QSplitter::handle {
            background: #45475a;
            width: 2px;
        }
        
        QScrollArea {
            border: none;
            background: transparent;
        }
        
        QTextEdit {
            background-color: #1e1e2e;
            border: 1px solid #45475a;
            border-radius: 4px;
            font-family: 'Consolas', 'Courier New', monospace;
            font-size: 12px;
        }
        
        /* 滚动条样式 */
        QScrollBar:vertical {
            border: none;
            background: #313244;
            width: 10px;
            margin: 0px;
        }
        
        QScrollBar::handle:vertical {
            background: #6c7086;
            border-radius: 5px;
            min-height: 20px;
        }
        
        QScrollBar::handle:vertical:hover {
            background: #7f849c;
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none;
        }
        
        QScrollBar:horizontal {
            border: none;
            background: #313244;
            height: 10px;
            margin: 0px;
        }
        
        QScrollBar::handle:horizontal {
            background: #6c7086;
            border-radius: 5px;
            min-width: 20px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background: #7f849c;
        }
        
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }
        
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
            background: none;
        }
        
        /* 特殊按钮样式 */
        #insertBtn {
            background-color: #a6e3a1;
            color: #1e1e2e;
        }
        
        #findBtn {
            background-color: #89b4fa;
            color: #1e1e2e;
        }
        
        #deleteBtn {
            background-color: #f38ba8;
            color: #1e1e2e;
        }
        
        #animateBtn {
            background-color: #cba6f7;
            color: #1e1e2e;
        }
        
        #balanceBtn {
            background-color: #f5c2e7;
            color: #1e1e2e;
        }
        
        #randomBtn {
            background-color: #f2cdcd;
            color: #1e1e2e;
        }
        
        /* 标题样式 */
        .title {
            font-size: 15px;
            font-weight: bold;
            color: #89b4fa;
            padding: 6px 0;
        }
        
        /* 分组框样式 */
        QGroupBox {
            font-weight: bold;
            border: 1px solid #45475a;
            border-radius: 6px;
            margin-top: 2px;
            padding-top: 8px;
            background-color: #313244;
            font-size: 11px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 4px;
            color: #89b4fa;
        }
        
        /* 状态栏样式 */
        QStatusBar {
            background-color: #313244;
            color: #cdd6f4;
            border-top: 1px solid #45475a;
            font-size: 11px;
            padding: 2px;
        }
    )");

    /* 创建标题*/
    QLabel* titleLabel = new QLabel(QString::fromUtf8("🌳 二叉排序树可视化演示"));
    titleLabel->setProperty("class", "title");
    titleLabel->setAlignment(Qt::AlignCenter);

     /* 创建UI组件*/
    valueInput = new QLineEdit;
    valueInput->setPlaceholderText(QString::fromUtf8("输入节点值"));
    valueInput->setMaximumWidth(80);

    /* 创建操作按钮*/
    insertBtn  = new QPushButton(QString::fromUtf8("插入"));
    findBtn    = new QPushButton(QString::fromUtf8("查找"));
    deleteBtn  = new QPushButton(QString::fromUtf8("删除"));
    displayBtn = new QPushButton(QString::fromUtf8("显示"));
    clearBtn   = new QPushButton(QString::fromUtf8("清空"));

    /* 设置对象名称以便应用特定样式*/
    insertBtn->setObjectName("insertBtn");
    findBtn  ->setObjectName("findBtn");
    deleteBtn->setObjectName("deleteBtn");

    /* 创建动画控制按钮*/
    animateFindBtn    = new QPushButton(QString::fromUtf8("动画查找"));
    animateInsertBtn  = new QPushButton(QString::fromUtf8("动画插入"));
    animateDeleteBtn  = new QPushButton(QString::fromUtf8("动画删除"));
    animateBalanceBtn = new QPushButton(QString::fromUtf8("动画平衡"));

    /* 设置动画按钮样式*/
    animateFindBtn   ->setObjectName("animateBtn");
    animateInsertBtn ->setObjectName("animateBtn");
    animateDeleteBtn ->setObjectName("animateBtn");
    animateBalanceBtn->setObjectName("animateBtn");

    /* 音效控制*/
    soundToggleBtn = new QPushButton(QString::fromUtf8("🔇 关闭音效"));
    soundToggleBtn->setCheckable(true);

    /* 动画速度控制*/
    QLabel* speedLabel   = new QLabel(QString::fromUtf8("速度(慢←→快):"));
    animationSpeedSlider = new QSlider(Qt::Horizontal);
    animationSpeedSlider->setRange(50, 2000);
    animationSpeedSlider->setValue(2000);
    animationSpeedSlider->setTickPosition(QSlider::TicksBelow);
    animationSpeedSlider->setTickInterval(100);
    animationSpeedSlider->setMaximumWidth(450);
    

    /* 信息显示区域*/
    infoArea = new QTextEdit;
    infoArea->setReadOnly(true);

    /* 树视图*/
    bstView = new BSTView;
    bstView->setTree(&bst);

    /* 树生成相关组件*/
    /* 随机数量输入框*/
    countInput = new QLineEdit;
    countInput->setText("10");
    countInput->setValidator(new QIntValidator(1, 100, this));
    countInput->setFixedWidth(40);
    /* 随机生成按钮*/
    randomCountBtn = new QPushButton(QString::fromUtf8("随机生成"));
    randomCountBtn->setObjectName("randomBtn");
    /* 输入一组值*/
    valuesInput = new QLineEdit;
    valuesInput->setPlaceholderText(QString::fromUtf8("数字用空格分隔"));
    valuesInput->setMinimumWidth(120);
    /* 构建按钮*/
    buildTreeBtn = new QPushButton(QString::fromUtf8("构建"));

    /* 视图控制按钮*/
    zoomInBtn    = new QPushButton(QString::fromUtf8("放大"));
    zoomOutBtn   = new QPushButton(QString::fromUtf8("缩小"));
    resetViewBtn = new QPushButton(QString::fromUtf8("重置"));
    balanceBtn   = new QPushButton(QString::fromUtf8("平衡"));
    balanceBtn->setObjectName("balanceBtn");
    randomBtn = new QPushButton(QString::fromUtf8("随机"));
    randomBtn->setObjectName("randomBtn");

    /* 创建滚动区域来包含BSTView*/
    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidget(bstView);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumSize(400, 300);

    /* 创建分割器*/
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(scrollArea);
    splitter->addWidget(infoArea);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>() << 700 << 300);

    /* 创建分组框*/
    auto createGroupBox = [](const QString& title) {
        QGroupBox* group = new QGroupBox(title);
        return group;
    };

    /* 创建操作组*/
    QGroupBox* operationGroup = createGroupBox(QString::fromUtf8("节点操作"));
    QHBoxLayout* operationLayout = new QHBoxLayout;
    operationLayout->addWidget(new QLabel(QString::fromUtf8("请输入需要插入或删除的值:")));
    operationLayout->addWidget(valueInput);
    operationLayout->addWidget(insertBtn);
    operationLayout->addWidget(findBtn);
    operationLayout->addWidget(deleteBtn);
    operationLayout->addWidget(displayBtn);
    operationLayout->addWidget(clearBtn);
    operationLayout->setSpacing(4);
    operationLayout->setContentsMargins(8, 12, 8, 8);
    operationGroup ->setLayout(operationLayout);

    /* 创建动画组*/
    QGroupBox* animationGroup = createGroupBox(QString::fromUtf8("动画控制"));
    QHBoxLayout* animationLayout = new QHBoxLayout;
    animationLayout->addWidget(animateFindBtn);
    animationLayout->addWidget(animateInsertBtn);
    animationLayout->addWidget(animateDeleteBtn);
    animationLayout->addWidget(animateBalanceBtn);
    animationLayout->addWidget(speedLabel);
    animationLayout->addWidget(animationSpeedSlider);
    animationLayout->addWidget(soundToggleBtn);
    animationLayout->setSpacing(4);
    animationLayout->setContentsMargins(8, 12, 8, 8);
    animationGroup ->setLayout(animationLayout);
     
    /* 创建树生成组*/
    QGroupBox* treeGenGroup = createGroupBox(QString::fromUtf8("树生成"));
    QHBoxLayout* treeGenLayout = new QHBoxLayout;
    treeGenLayout->addWidget(new QLabel(QString::fromUtf8("节点数:")));
    treeGenLayout->addWidget(countInput);
    treeGenLayout->addWidget(randomCountBtn);
    treeGenLayout->addWidget(new QLabel(QString::fromUtf8("自定义:")));
    treeGenLayout->addWidget(valuesInput);
    treeGenLayout->addWidget(buildTreeBtn);
    treeGenLayout->setSpacing(4);
    treeGenLayout->setContentsMargins(8, 12, 8, 8);
    treeGenGroup ->setLayout(treeGenLayout);

    /* 创建视图控制组*/
    QGroupBox* viewGroup = createGroupBox(QString::fromUtf8("视图控制"));
    QHBoxLayout* viewLayout = new QHBoxLayout;
    viewLayout->addWidget(zoomInBtn);
    viewLayout->addWidget(zoomOutBtn);
    viewLayout->addWidget(resetViewBtn);
    viewLayout->addWidget(balanceBtn);
    viewLayout->addWidget(randomBtn);
    viewLayout->setSpacing(4);
    viewLayout->setContentsMargins(8, 12, 8, 8);
    viewGroup ->setLayout(viewLayout);

    /* 创建状态栏*/
    QStatusBar* statusBar = new QStatusBar;

    /* 主布局*/
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(operationGroup);
    mainLayout->addWidget(animationGroup);
    mainLayout->addWidget(treeGenGroup);
    mainLayout->addWidget(viewGroup);
    mainLayout->addWidget(splitter, 1);
    mainLayout->addWidget(statusBar);
    mainLayout->setSpacing(6);
    mainLayout->setContentsMargins(8, 8, 8, 4);

    setLayout(mainLayout);
    setWindowTitle(QString::fromUtf8("二叉排序树可视化演示"));
    resize(1000, 800);

    /* 初始化音效*/
    initSoundEffects();

    /* 连接信号和槽*/
    connect(insertBtn,      &QPushButton::clicked, this, &BSTWindow::insertValue);
    connect(findBtn,        &QPushButton::clicked, this, &BSTWindow::findValue);
    connect(deleteBtn,      &QPushButton::clicked, this, &BSTWindow::deleteValue);
    connect(displayBtn,     &QPushButton::clicked, this, &BSTWindow::displayTree);
    connect(clearBtn,       &QPushButton::clicked, this, &BSTWindow::clearTree);
    connect(randomBtn,      &QPushButton::clicked, this, &BSTWindow::generateRandomTree);
    connect(balanceBtn,     &QPushButton::clicked, this, &BSTWindow::balanceTree);
    connect(zoomInBtn,      &QPushButton::clicked, this, &BSTWindow::zoomIn);
    connect(zoomOutBtn,     &QPushButton::clicked, this, &BSTWindow::zoomOut);
    connect(resetViewBtn,   &QPushButton::clicked, this, &BSTWindow::resetView);
    connect(randomCountBtn, &QPushButton::clicked, this, &BSTWindow::generateRandomTreeWithCount);
    connect(buildTreeBtn,   &QPushButton::clicked, this, &BSTWindow::buildTreeFromValues);
    connect(soundToggleBtn, &QPushButton::toggled, this, &BSTWindow::toggleSound);

    /* 动画控制连接*/
    connect(animateFindBtn,       &QPushButton::clicked,                this, &BSTWindow::animateFind);
    connect(animateInsertBtn,     &QPushButton::clicked,                this, &BSTWindow::animateInsert);
    connect(animateDeleteBtn,     &QPushButton::clicked,                this, &BSTWindow::animateDelete);
    connect(animateBalanceBtn,    &QPushButton::clicked,                this, &BSTWindow::animateBalance);
    connect(animationSpeedSlider, &QSlider::valueChanged,               this, &BSTWindow::updateAnimationSpeed);
    connect(&bst,                 &BinarySearchTree::animationFinished, this, &BSTWindow::onAnimationFinished);
    connect(bstView,              &BSTView::nodeHighlighted,            this, &BSTWindow::playTouchSound);

    /* 更新状态栏*/
    connect(&bst, &BinarySearchTree::treeChanged, this, [this, statusBar]() {
        QString status = QString::fromUtf8("节点数: %1 | 树高度: %2,可用鼠标进行移动和缩放")
            .arg(countNodes(bst.getRoot()))
            .arg(bst.getHeight());
        statusBar->showMessage(status);
     });

    /* 初始状态*/
    QString status = QString::fromUtf8("节点数: 0 | 树高度: 0,可用鼠标进行移动和缩放");
    statusBar->showMessage(status);

    updateAnimationSpeed(2000);
}

/***************************************************************************
  函数名称：BSTWindow::countNodes
  功    能：辅助函数：计算节点数量
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int BSTWindow::countNodes(TreeNode* node) {
    if (!node) 
        return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}

/***************************************************************************
  函数名称：BSTWindow::insertValue
  功    能：插入值到二叉搜索树
  输入参数：
  返 回 值：
  说    明：从输入框获取值并插入到树中，更新信息显示
***************************************************************************/
void BSTWindow::insertValue() {
    bool ok;
    int value = valueInput->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请输入有效的整数值"));
        return;
    }

    /*同步在数据结构中更新和视图显示*/
    bst.insert(value);
    playTouchSound();
    infoArea->setText(QString::fromUtf8("已插入值: ") + QString::number(value) + QString::fromUtf8("\n当前树: ") + bst.display());
    valueInput->clear();
}

/***************************************************************************
  函数名称：BSTWindow::findValue
  功    能：在二叉搜索树中查找值
  输入参数：
  返 回 值：
  说    明：从输入框获取值并在树中查找，显示查找结果
***************************************************************************/
void BSTWindow::findValue() {
    bool ok;
    int value = valueInput->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请输入有效的整数值"));
        return;
    }

    int depth = 0;
    if (bst.find(value, depth)) {
        infoArea->setText(QString::fromUtf8("值 ") + QString::number(value) +
            QString::fromUtf8(" 在树中，深度为: ") + QString::number(depth));
        playTouchSound();  // 播放触摸音效
    }
    else {
        infoArea->setText(QString::fromUtf8("值 ") + QString::number(value) + QString::fromUtf8(" 不在树中"));
        playTouchSound();
    }
    valueInput->clear();
}

/***************************************************************************
  函数名称：BSTWindow::deleteValue
  功    能：从二叉搜索树中删除值
  输入参数：
  返 回 值：
  说    明：从输入框获取值并从树中删除，更新信息显示
***************************************************************************/
void BSTWindow::deleteValue() {
    bool ok;
    int value = valueInput->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请输入有效的整数值"));
        return;
    }

    if (bst.isEmpty()) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("树为空，无法删除"));
        return;
    }

    int depth = 0;
    if (!bst.find(value, depth)) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("值 ") + QString::number(value) + QString::fromUtf8(" 不在树中，无法删除"));
        return;
    }

    bst.remove(value);
    playTouchSound();  // 播放触摸音效
    infoArea->setText(QString::fromUtf8("已删除值: ") + QString::number(value) + QString::fromUtf8("\n当前树: ") + bst.display());
    valueInput->clear();
}

/***************************************************************************
  函数名称：BSTWindow::displayTree
  功    能：显示树的信息
  输入参数：
  返 回 值：
  说    明：显示树的当前状态和高度信息
***************************************************************************/
void BSTWindow::displayTree() {
    infoArea->setText(QString::fromUtf8("当前树: ") + bst.display() +
        QString::fromUtf8("\n树高度: ") + QString::number(bst.getHeight()));
}

/***************************************************************************
  函数名称：BSTWindow::clearTree
  功    能：清空二叉搜索树
  输入参数：
  返 回 值：
  说    明：清空树中的所有节点，更新信息显示
***************************************************************************/
void BSTWindow::clearTree() {
    if (bst.isEmpty()) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("树已经为空"));
        return;
    }

    // 使用 clear() 方法而不是赋值操作
    bst.clear();
    infoArea->setText(QString::fromUtf8("树已清空"));
}

/***************************************************************************
  函数名称：BSTWindow::generateRandomTree
  功    能：生成随机二叉搜索树
  输入参数：
  返 回 值：
  说    明：生成包含10个随机值的二叉搜索树，更新信息显示
***************************************************************************/
void BSTWindow::generateRandomTree() {
    // 先清空现有树
    bst.clear();

    // 生成10个随机数
    QTime time = QTime::currentTime();
    srand(time.msec() + time.second() * 1000);
    
    const int randomNodeNum = 10;
    const int maxNum        = 100;

    QString values;
    for (int i = 0; i < randomNodeNum; i++) {
        int value = rand() % maxNum;
        bst.insert(value);
        values += QString::number(value) + " ";
    }

    bstView->setTree(&bst);
    infoArea->setText(QString::fromUtf8("生成的随机值: ") + values + QString::fromUtf8("\n当前树: ") + bst.display());
}

/***************************************************************************
  函数名称：BSTWindow::balanceTree
  功    能：平衡二叉搜索树
  输入参数：
  返 回 值：
  说    明：将树转换为平衡二叉搜索树，更新信息显示
***************************************************************************/
void BSTWindow::balanceTree() {
    if (bst.isEmpty()) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("树为空，无法平衡"));
        return;
    }

    bst.balance();
    infoArea->setText(QString::fromUtf8("树已平衡\n当前树: ") + bst.display());
}

/***************************************************************************
  函数名称：BSTWindow::zoomIn
  功    能：放大树视图
  输入参数：
  返 回 值：
  说    明：放大树视图的显示比例
***************************************************************************/
void BSTWindow::zoomIn() {
    bstView->zoomIn();
}

/***************************************************************************
  函数名称：BSTWindow::zoomOut
  功    能：缩小树视图
  输入参数：
  返 回 值：
  说    明：缩小树视图的显示比例
***************************************************************************/
void BSTWindow::zoomOut() {
    bstView->zoomOut();
}

/***************************************************************************
  函数名称：BSTWindow::resetView
  功    能：重置树视图
  输入参数：
  返 回 值：
  说    明：重置树视图的缩放和位置到初始状态
***************************************************************************/
void BSTWindow::resetView() {
    bstView->resetView();
}

/***************************************************************************
  函数名称：BSTWindow::animateFind
  功    能：执行动画查找操作
  输入参数：
  返 回 值：
  说    明：从输入框获取值并执行动画查找过程
***************************************************************************/
void BSTWindow::animateFind() {
    bool ok;
    int value = valueInput->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请输入有效的整数值"));
        return;
    }

    // 禁用按钮，防止在动画过程中进行操作
    setEnabled(false);

    bst.startFindAnimation(value);
    infoArea->setText(QString::fromUtf8("开始查找动画: ") + QString::number(value));
}

/***************************************************************************
  函数名称：BSTWindow::animateInsert
  功    能：执行动画插入操作
  输入参数：
  返 回 值：
  说    明：从输入框获取值并执行动画插入过程
***************************************************************************/
void BSTWindow::animateInsert() {
    bool ok;
    int value = valueInput->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请输入有效的整数值"));
        return;
    }

    int depth = 0;
    if (bst.find(value, depth)) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("值 ") + QString::number(value) + QString::fromUtf8(" 已在树中"));
        return;
    }

    // 禁用按钮，防止在动画过程中进行操作
    setEnabled(false);

    bst.startInsertAnimation(value);
    infoArea->setText(QString::fromUtf8("开始插入动画: ") + QString::number(value));
}

/***************************************************************************
  函数名称：BSTWindow::animateDelete
  功    能：执行动画删除操作
  输入参数：
  返 回 值：
  说    明：从输入框获取值并执行动画删除过程
***************************************************************************/
void BSTWindow::animateDelete() {
    bool ok;
    int value = valueInput->text().toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请输入有效的整数值"));
        return;
    }

    if (bst.isEmpty()) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("树为空，无法删除"));
        return;
    }

    int depth = 0;
    if (!bst.find(value, depth)) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("值 ") + QString::number(value) + QString::fromUtf8(" 不在树中，无法删除"));
        return;
    }

    // 禁用按钮，防止在动画过程中进行操作
    setEnabled(false);

    bst.startDeleteAnimation(value);
    infoArea->setText(QString::fromUtf8("开始删除动画: ") + QString::number(value));
}

/***************************************************************************
  函数名称：BSTWindow::animateBalance
  功    能：执行动画平衡操作
  输入参数：
  返 回 值：
  说    明：执行动画平衡树的过程
***************************************************************************/
void BSTWindow::animateBalance() {
    if (bst.isEmpty()) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("树为空，无法平衡"));
        return;
    }

    // 禁用按钮，防止在动画过程中进行操作
    setEnabled(false);

    bst.startBalanceAnimation();
    infoArea->setText(QString::fromUtf8("开始平衡动画"));
}

/***************************************************************************
  函数名称：BSTWindow::updateAnimationSpeed
  功    能：更新动画速度
  输入参数：speed - 动画速度值
  返 回 值：
  说    明：根据滑块值更新动画速度
***************************************************************************/
void BSTWindow::updateAnimationSpeed(int speed) {
    // 将滑块值反转：值越大，动画速度越快（时间间隔越小）
    int actualSpeed = 2050 - speed; // 2050 = 50 + 2000
    bst.setAnimationSpeed(actualSpeed);
}
/***************************************************************************
  函数名称：BSTWindow::onAnimationFinished
  功    能：动画完成处理
  输入参数：
  返 回 值：
  说    明：动画完成后启用按钮并更新信息显示
***************************************************************************/
void BSTWindow::onAnimationFinished() {
    // 启用按钮
    setEnabled(true);
    playSuccessSound();  // 播放成功音效
    infoArea->setText(QString::fromUtf8("动画完成\n当前树: ") + bst.display());
}

/***************************************************************************
  函数名称：BSTWindow::generateRandomTreeWithCount
  功    能：根据指定数量生成随机树
  输入参数：
  返 回 值：
  说    明：从输入框获取数量并生成包含指定数量随机值的二叉搜索树
***************************************************************************/
void BSTWindow::generateRandomTreeWithCount() {
    // 获取输入的节点数量
    bool ok;
    int count = countInput->text().toInt(&ok);
	const int maxNodes = 100; // 最大节点数限制

    if (!ok || count <= 0 || count > maxNodes) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"),
            QString::fromUtf8("请输入1-100之间的有效整数"));
        return;
    }

    // 先清空现有树
    bst.clear();

    // 生成随机数
    QTime time = QTime::currentTime();
    srand(time.msec() + time.second() * 1000);

    QString values;
    QSet<int> generatedValues;  // 使用集合确保值唯一

    while (generatedValues.size() < count) {
        int value = rand() % 100;  // 生成0-99的随机数
        if (!generatedValues.contains(value)) {
            generatedValues.insert(value);
            bst.insert(value);
            values += QString::number(value) + " ";
        }
    }

    bstView->setTree(&bst);
    infoArea->setText(QString::fromUtf8("生成的随机值: ") + values +
        QString::fromUtf8("\n当前树: ") + bst.display());
}

/***************************************************************************
  函数名称：BSTWindow::buildTreeFromValues
  功    能：根据自定义值构建树
  输入参数：
  返 回 值：
  说    明：从输入框获取自定义值并构建二叉搜索树
***************************************************************************/
void BSTWindow::buildTreeFromValues() {
    // 获取输入的值
    QString input = valuesInput->text().trimmed();
    if (input.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"),
            QString::fromUtf8("请输入有效的数字"));
        return;
    }

    // 解析输入的值
    QStringList valueStrings = input.split(" ", Qt::SkipEmptyParts);
    QVector<int> values;

    for (const QString& str : valueStrings) {
        bool ok;
        int value = str.toInt(&ok);
        if (ok) {
            values.append(value);
        }
        else {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"),
                QString::fromUtf8("无效的数字: ") + str);
            return;
        }
    }

    // 先清空现有树
    bst.clear();

    // 插入所有值
    QString insertedValues;
    for (int value : values) {
        bst.insert(value);
        insertedValues += QString::number(value) + " ";
    }

    bstView->setTree(&bst);
    infoArea->setText(QString::fromUtf8("插入的值: ") + insertedValues +
        QString::fromUtf8("\n当前树: ") + bst.display());

    valuesInput->clear();
}

/***************************************************************************
  函数名称：BSTWindow::initSoundEffects
  功    能：初始化音效
  输入参数：
  返 回 值：
  说    明：初始化背景音乐和音效，设置音量并开始播放
***************************************************************************/
void BSTWindow::initSoundEffects() {
    // 获取应用程序目录
    QString appDir = QCoreApplication::applicationDirPath();

    // 初始化背景音乐
    backgroundMusic = new QMediaPlayer(this);
    audioOutput     = new QAudioOutput(this);
    backgroundMusic->setAudioOutput(audioOutput);
    backgroundMusic->setSource(QUrl::fromLocalFile(appDir + "/sounds/music.mp3"));
    audioOutput->setVolume(0.3);
    backgroundMusic->setLoops(QMediaPlayer::Infinite);

    // 初始化音效
    touchSound      = new QSoundEffect(this);
    touchSound->setSource(QUrl::fromLocalFile(appDir + "/sounds/touch.wav"));
    touchSound->setVolume(0.7f);

    successSound    = new QSoundEffect(this);
    successSound->setSource(QUrl::fromLocalFile(appDir + "/sounds/success.wav"));
    successSound->setVolume(0.7f);

    // 开始播放背景音乐
    backgroundMusic->play();
}

/***************************************************************************
  函数名称：BSTWindow::playTouchSound
  功    能：播放触摸音效
  输入参数：
  返 回 值：
  说    明：如果音效启用，播放触摸音效
***************************************************************************/
void BSTWindow::playTouchSound() {
    if (soundEnabled) {
        touchSound->play();
    }
}

/***************************************************************************
  函数名称：BSTWindow::playSuccessSound
  功    能：播放成功音效
  输入参数：
  返 回 值：
  说    明：如果音效启用，播放成功音效
***************************************************************************/
void BSTWindow::playSuccessSound() {
    if (soundEnabled) {
        successSound->play();
    }
}

/***************************************************************************
  函数名称：BSTWindow::toggleSound
  功    能：切换音效状态
  输入参数：muted - 是否静音
  返 回 值：
  说    明：切换音效的启用状态，更新按钮文本和控制音乐播放
***************************************************************************/
void BSTWindow::toggleSound(bool muted) {
    soundEnabled = !muted;

    if (muted) {
        soundToggleBtn->setText(QString::fromUtf8("🔇 关闭音效"));
        // 暂停背景音乐
        backgroundMusic->pause();
    }
    else {
        soundToggleBtn->setText(QString::fromUtf8("🔊 开启音效"));
        // 恢复背景音乐
        backgroundMusic->play();
    }
}