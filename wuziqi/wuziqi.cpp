#include<iostream>
#include<cstdlib>
#include<string>
#include<cstring>
#include<vector>
#include<set>
#include<deque>
#include<memory.h>
#include<algorithm>
using namespace std;

class GameTree//定义博弈树，全部的函数及变量都封装在GameTree中，方便外部调用
{
private:
    class Node//Node类表示节点
    {
    public:
        int32_t value;//记录估值
        uint32_t depth;//记录当前节点的深度
        Node* father;//记录当前节点的父节点
        set<Node*> children;//记录当前节点的子节点
        uint8_t cntx, cnty;//记录当前棋局最后一步落子的x,y轴坐标
        uint8_t board[15][15]{};//记录当前棋局

        Node()//无参构造函数
        {
            father = nullptr;
            children.clear();
            value = INT32_MIN;
            depth = cntx = cnty = 0;
            memset(board, 0, sizeof(board));
        }

        Node(Node* node, uint8_t opex, uint8_t opey)//有参构造函数
        {
            depth = node->depth + 1;
            value = is_max_node() ? INT32_MIN : INT32_MAX;
            father = node;
            children.clear();
            cntx = opex;
            cnty = opey;
            memcpy(board, node->board, sizeof(board));
            board[cntx][cnty] = (depth & 1u) ? 'B' : 'W';
        }

        bool is_max_node()//判断当前节点是否为MAX节点
        {
            return (depth & 1u) ^ 1u;
        }

        static int32_t evaluate_white(string& s)//对含白五元组进行得分量化
        {
            string patterns[31] = {
                "W0000","0W000","00W00","000W0","0000W",
                "WW000","0WW00","00WW0","000WW","W0W00",
                "0W0W0","00W0W","W00W0","0W00W","W000W",
                "WWW00","0WWW0","00WWW","WW0W0","0WW0W",
                "W0WW0","0W0WW","WW00W","W00WW","W0W0W",
                "WWWW0","WWW0W","WW0WW","W0WWW","0WWWW",
                "WWWWW"
            };
            int32_t scores[31] = {
                1,1,1,1,1,
                10,10,10,10,10,
                10,10,10,10,10,
                1000,2000,1000,1000,1000,//"0WWW0"的得分分值较高是因为这种摆法持白一方可轻易赢下比赛胜利
                1000,1000,1000,1000,1000,
                100000,100000,100000,100000,100000,
                10000000
            };
            for (uint8_t i = 0; i < 31; i++)
            {
                if (s == patterns[i])
                {
                    return scores[i];
                }
            }
            return 0;
        }

        static int32_t evaluate_black(string& s)//对含黑五元组进行得分量化
        {
            string patterns[31] = {
                "B0000","0B000","00B00","000B0","0000B",
                "BB000","0BB00","00BB0","000BB","B0B00",
                "0B0B0","00B0B","B00B0","0B00B","B000B",
                "BBB00","0BBB0","00BBB","BB0B0","0BB0B",
                "B0BB0","0B0BB","BB00B","B00BB","B0B0B",
                "BBBB0","BBB0B","BB0BB","B0BBB","0BBBB",
                "BBBBB"
            };
            int32_t scores[31] = {
                1,1,1,1,1,
                10,10,10,10,10,
                10,10,10,10,10,
                100,100,100,100,100,
                100,100,100,100,100,
                10000,10000,10000,10000,10000,
                1000000
            };
            for (uint8_t i = 0; i < 31; i++)
            {
                if (s == patterns[i])
                {
                    return scores[i];
                }
            }
            return 0;
        }

        static string convert(uint8_t pos)
        {
            if (pos == 0) return "0";
            else if (pos == 66) return "B";
            else return "W";
        }

        uint8_t board_identify()//判断当前棋局是否已经分出胜负，若黑方获胜返回66（B的ascii码），白方获胜返回87（W的ascii码）
        {
            for (uint8_t i = 0; i < 15; i++)
            {
                for (uint8_t j = 0; j < 15; j++)
                {
                    if (j < 11)
                    {
                        string s;
                        for (uint8_t k = 0; k < 5; k++)
                        {
                            s += convert(board[i][j + k]);
                        }
                        if (s == "BBBBB") return 66;
                        if (s == "WWWWW") return 87;
                    }
                    if (i < 11)
                    {
                        string s;
                        for (uint8_t k = 0; k < 5; k++)
                        {
                            s += convert(board[i + k][j]);
                        }
                        if (s == "BBBBB") return 66;
                        if (s == "WWWWW") return 87;
                    }
                    if (i < 11 && j < 11)
                    {
                        string s;
                        for (uint8_t k = 0; k < 5; k++)
                        {
                            s += convert(board[i + k][j + k]);
                        }
                        if (s == "BBBBB") return 66;
                        if (s == "WWWWW") return 87;
                    }
                    if (i < 11 && j >= 4)
                    {
                        string s;
                        for (uint8_t k = 0; k < 5; k++)
                        {
                            s += convert(board[i + k][j - k]);
                        }
                        if (s == "BBBBB") return 66;
                        if (s == "WWWWW") return 87;
                    }
                }
            }
            return 0;
        }

        void evaluate()//估价函数
        {
            value = 0;
            for (uint8_t i = 0; i < 15; i++)
            {
                for (uint8_t j = 0; j < 15; j++)
                {
                    if (j < 11)
                    {
                        string s;
                        for (uint8_t k = 0; k < 5; k++)
                        {
                            s += convert(board[i][j + k]);
                        }
                        value += evaluate_black(s) - evaluate_white(s);//计算横向五元组的得分
                    }
                    if (i < 11)
                    {
                        string s;
                        for (uint8_t k = 0; k < 5; k++)
                        {
                            s += convert(board[i + k][j]);
                        }
                        value += evaluate_black(s) - evaluate_white(s);//计算竖向五元组的得分
                    }
                    if (i < 11 && j < 11)
                    {
                        string s;
                        for (uint8_t k = 0; k < 5; k++)
                        {
                            s += convert(board[i + k][j + k]);
                        }
                        value += evaluate_black(s) - evaluate_white(s);//计算斜向右下五元组的得分
                    }
                    if (i < 11 && j >= 4)
                    {
                        string s;
                        for (uint8_t k = 0; k < 5; k++)
                        {
                            s += convert(board[i + k][j - k]);
                        }
                        value += evaluate_black(s) - evaluate_white(s);//计算斜向左下五元组的得分
                    }
                }
            }
        }

        void printdata()
        {
            cout << this << " depth=" << depth << " value=" << value << " father=" << father << " children=(";
            for (auto child : children)
            {
                cout << child << ",";
            }
            cout << ")" << endl;
            for (auto& i : board)
            {
                cout << "    ";
                for (uint8_t j : i)
                {
                    if (j == 'B') cout << "B";
                    if (j == 'W') cout << "W";
                    if (j == 0) cout << "+";
                }
                cout << endl;
            }
        }
    };

    uint8_t expandradius = 2;//搜索半径
    uint32_t maxdepth = 5;//最大深度
    Node* noderoot = new Node();//指向根节点的指针
    Node* nodenext = nullptr;//指向最佳节点的指针
    deque<Node*> opentable;//存放待扩展节点的指针的双向队列
    deque<Node*> closedtable;//存放已扩展节点的指针的双向队列

    vector< pair<uint8_t, uint8_t> >getsearchnodes(Node* node)//返回当前棋局的待扩展点坐标集合（vector容器），容器中每个元素都是一个点的坐标
    {
        bool haschess = false, newboard[15][15];
        memset(newboard, false, sizeof(newboard));
        for (uint8_t i = 0; i < 15; i++)
        {
            for (uint8_t j = 0; j < 15; j++)
            {
                if (node->board[i][j] == 0) continue;
                haschess = true;
                uint8_t x1 = max(0, i - expandradius), x2 = min(14, i + expandradius);
                uint8_t y1 = max(0, j - expandradius), y2 = min(14, j + expandradius);
                for (uint8_t x = x1; x <= x2; x++)
                {
                    for (uint8_t y = y1; y <= y2; y++)
                    {
                        if (node->board[x][y] == 0) newboard[x][y] = true;
                    }
                }
            }
        }
        vector< pair<uint8_t, uint8_t> > mask;
        if (!haschess) mask.emplace_back(pair<uint8_t, uint8_t>(7, 8));
        else
        {
            for (uint8_t i = 0; i < 15; i++)
            {
                for (uint8_t j = 0; j < 15; j++)
                {
                    if (newboard[i][j]) mask.emplace_back(pair<uint8_t, uint8_t>(i, j));
                }
            }
        }
        return mask;
    }

    uint8_t expandchildrennodes(Node* node)//扩展node节点，生成node节点的所有子节点
    {
        vector< pair<uint8_t, uint8_t> > mask = getsearchnodes(node);
        for (auto pos : mask)
        {
            Node* n = new Node(node, pos.first, pos.second);
            node->children.insert(n);
            opentable.push_front(n);
        }
        return mask.size();
    }

    static bool isalphabetacut(Node* node)//判断节点node是否能a-b剪枝
    {
        if (node == nullptr || node->father == nullptr) return false;
        if (node->is_max_node() && node->value > node->father->value) return true;
        if (!node->is_max_node() && node->value < node->father->value) return true;
        return isalphabetacut(node->father);
    }

    static void update_value_from_node(Node* node)//更新某个叶节点的所有父节点的a值或b值
    {
        if (node == nullptr) return;
        if (node->children.empty())
        {
            update_value_from_node(node->father);
            return;
        }
        if (node->is_max_node())
        {
            int32_t cntvalue = INT32_MIN;
            for (Node* n : node->children)
            {
                if (n->value != INT32_MAX) cntvalue = max(cntvalue, n->value);
                if (cntvalue > node->value)
                {
                    node->value = cntvalue;
                    update_value_from_node(node->father);
                }
            }
        }
        else
        {
            int32_t cntvalue = INT32_MAX;
            for (Node* n : node->children)
            {
                if (n->value != INT32_MIN) cntvalue = min(cntvalue, n->value);
                if (cntvalue < node->value)
                {
                    node->value = cntvalue;
                    update_value_from_node(node->father);
                }
            }
        }
    }

    void setnextpos()//寻找下一步的最佳落子点，即根节点的最佳子节点
    {
        nodenext = *noderoot->children.begin();
        for (Node* n : noderoot->children)
        {
            if (n->value > nodenext->value) nodenext = n;
        }
    }

    static void recursiveprint(Node* nodefatherpt)
    {
        nodefatherpt->printdata();
        for (Node* nodechildpt : nodefatherpt->children) recursiveprint(nodechildpt);
    }

    void debugprint()
    {
        noderoot->printdata();
        for (Node* nodechild : noderoot->children)
        {
            recursiveprint(nodechild);
        }
        cout << endl;
    }

public:
    GameTree() = default;

    explicit GameTree(uint32_t maxdepth, uint8_t expandradius) : maxdepth(maxdepth), expandradius(expandradius) {}

    explicit GameTree(uint32_t maxdepth, uint8_t expandradius, uint8_t(&board)[15][15]) : maxdepth(maxdepth), expandradius(expandradius)
    {
        memcpy(noderoot->board, board, sizeof(board));
    }

    uint8_t game()//博弈控制函数
    {
        uint8_t result = noderoot->board_identify();
        if (result == 'B') return 'B';
        if (result == 'W') return 'W';
        opentable.push_back(noderoot);
        while (!opentable.empty())
        {
            Node* node = opentable.front();
            opentable.pop_front();
            closedtable.push_back(node);
            if (isalphabetacut(node->father)) continue;
            if (node->depth < maxdepth)
            {
                uint8_t numexpand = expandchildrennodes(node);
                if (numexpand != 0) continue;
            }
            node->evaluate();
            update_value_from_node(node);
        }
        setnextpos();
        return 0;
    }

    pair<uint8_t, uint8_t> getnextpos()//返回下一步落子点的x,y轴坐标
    {
        if (nodenext == nullptr) return pair<uint8_t, uint8_t>(255, 255);
        else return pair<uint8_t, uint8_t>(nodenext->cntx, nodenext->cnty);
    }

    void shownextpos()//显示下一步落子点的坐标
    {
        if (nodenext == nullptr) cout << "(255,255)" << endl;
        else cout << "(" << (uint32_t)nodenext->cntx << "," << (uint32_t)nodenext->cnty << ")" << endl;
    }

    void showboard(bool reverse)//显示当前棋局
    {
        if (nodenext == nullptr) nodenext = noderoot;
        uint8_t row = 0;
        cout << "   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4" << endl;
        for (uint8_t i = 0; i < 15; i++)
        {
            if (row < 10) cout << " ";
            cout << (uint32_t)(row++) << " ";
            for (uint8_t j = 0; j < 15; j++)
            {
                if (j != 0) cout << "-";
                if (nodenext->board[i][j] == 'B')
                {
                    if (reverse) cout << "W";
                    else cout << "B";
                    continue;
                }
                if (nodenext->board[i][j] == 'W')
                {
                    if (reverse) cout << "B";
                    else cout << "W";
                    continue;
                }
                cout << "+";
            }
            cout << endl;
        }
        cout << endl;
    }
};
bool isMagicSquare(uint8_t board[15][15]) {
    int n = 15; // 棋盘大小
    int magicSum = n * (n * n + 1) / 2; // 魔术和

    // 检查行和列的和是否相等
    for (int i = 0; i < n; ++i) {
        int rowSum = 0;
        int colSum = 0;
        for (int j = 0; j < n; ++j) {
            rowSum += board[i][j];
            colSum += board[j][i];
        }
        if (rowSum != magicSum || colSum != magicSum) {
            return false;
        }
    }

    // 检查主对角线的和
    int diagSum1 = 0;
    for (int i = 0; i < n; ++i) {
        diagSum1 += board[i][i];
    }
    if (diagSum1 != magicSum) {
        return false;
    }

    // 检查副对角线的和
    int diagSum2 = 0;
    for (int i = 0; i < n; ++i) {
        diagSum2 += board[i][n - 1 - i];
    }
    if (diagSum2 != magicSum) {
        return false;
    }

    return true; // 棋盘是魔术方阵
}
void machine_human_play() {
    cout << endl;
    uint32_t x = 0, y = 0;
    uint8_t board[15][15]{};
    for (uint8_t k = 0; k < 225; k++) {
        GameTree gt = GameTree(9, 2, board);
        uint8_t result = gt.game();
        if (result == 'B') {
            cout << "持黑一方胜利" << endl;
            gt.showboard(false);
            return;
        }
        if (result == 'W') {
            gt.showboard(false);
            cout << "持白一方胜利" << endl;
            return;
        }

        gt.shownextpos();
        gt.showboard(false);
        auto pos = gt.getnextpos();
        if (pos.first != 225 && pos.second != 225) board[pos.first][pos.second] = 'B';

        // 魔术方阵检查
        if (isMagicSquare(board)) {
            cout << "魔术方阵达成！" << endl;
            return;
        }

        do {
            cin >> x >> y;
            if (board[x][y] != 0) {
                cout << "该位置已经有棋子，请重新选择：" << endl;
            }
        } while (board[x][y] != 0);
        board[x][y] = 'W';
    }
}

void machine_machine_play()//机机对弈过程
{
    cout << endl;
    uint8_t turn = 'B', board[15][15]{}, inputboard[15][15]{};
    for (uint8_t k = 0; k < 225; k++)
    {
        cout << "[" << k + 1 << "]";
        memcpy(inputboard, board, sizeof(board));
        if (turn == 'W')
        {
            for (uint8_t i = 0; i < 15; i++)
            {
                for (uint8_t j = 0; j < 15; j++)
                {
                    if (board[i][j] == 'W') inputboard[i][j] = 'B';
                    if (board[i][j] == 'B') inputboard[i][j] = 'W';
                }
            }
        }
        GameTree gt = GameTree(8, 2, inputboard);
        uint8_t result = gt.game();
        if (result == 'B')
        {
            cout << "持黑一方胜利" << endl;
            gt.showboard(false);
            return;
        }
        if (result == 'W')
        {
            cout << "持白一方胜利" << endl;
            gt.showboard(true);
            return;
        }
        auto pos = gt.getnextpos();
        if (turn == 'B')
        {
            turn = 'W';
            board[pos.first][pos.second] = 'B';
            cout << "持黑一方 ";
            gt.shownextpos();
            gt.showboard(false);
        }
        else
        {
            turn = 'B';
            board[pos.first][pos.second] = 'W';
            cout << "持白一方 ";
            gt.shownextpos();
            gt.showboard(true);
        }
    }

    // 检查最终结果
    GameTree final_gt = GameTree(8, 2, board);
    uint8_t final_result = final_gt.game();
    if (final_result == 'B')
    {
        cout << "持黑一方胜利" << endl;
        final_gt.showboard(false);
    }
    else if (final_result == 'W')
    {
        cout << "持白一方胜利" << endl;
        final_gt.showboard(true);
    }
    else
    {
        cout << "平局" << endl;
        final_gt.showboard(false);
    }
}





int main()
{
    //machine_machine_play();
    machine_human_play();
    return 0;
}
