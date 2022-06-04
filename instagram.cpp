#include <iostream>
#include <algorithm>
using namespace std;

const int MAXM = 100000;
const int BTree_order = 3;


struct BTree {
    BTree **childs;
    int* data;
    int size;
    bool is_leaf;
} *root=NULL;


BTree* new_node() {
    BTree* res = new BTree;

    res->size = 0;
    res->data = new int[BTree_order];
    res->childs = new BTree *[BTree_order + 1];
    res->is_leaf = true;

    return res;
}

void print(BTree* node) {
    cout<<"print node:"<<' ';
    for(int i = 0; i < node->size; i++)
        cout<<node->data[i]<<' ';
    cout<<endl;
}

int find_child(int x, BTree* node, bool move_ahead=false) {
    int child = node->size - 1;

    while(child >= 0 && node->data[child] > x) {
        if(move_ahead) 
            node->data[child + 1] = node->data[child];
        child--;
    }
    return child + 1;
}

void printTree(BTree* node=root) {
    if(root == NULL)
        return;
    cout<<"Node:"<<' '<<node<<' ';
    print(node);
    int i;
    for (i = 0; i <= node->size; i++) {
        if (node->is_leaf == false)
            printTree(node->childs[i]);
    }
    cout<<"BYE"<<endl;
}

void split_child(BTree* node, BTree* child, int child_index) {
    int half = BTree_order / 2;
    BTree* tmp = new_node();
    tmp->is_leaf = child->is_leaf;
    tmp->size = half;

    for(int j = 0; j < tmp->size; j++)
        tmp->data[j] = child->data[j + half + 1];

    if(tmp->is_leaf == false) {
        for(int j = 0; j < half + 1; j++) {
            tmp->childs[j] = child->childs[j + half + 1];
        }
    }

    child->size = half;

    for(int j = node->size; j >= child_index + 1; j--)
        node->childs[j + 1] = node->childs[j];

    node->childs[child_index + 1] = tmp;

    for(int j = node->size - 1; j >= child_index; j--)
        node->data[j + 1] = node->data[j]; 

    node->data[child_index] = child->data[half];

    node->size++;
}

void insert(int x, BTree* node) {
    if(node->is_leaf == true) {
        int child = find_child(x, node, true);

        node->data[child] = x;
        node->size++;

        return;
    }

    
    int child = find_child(x, node);

    if(node->childs[child]->size == BTree_order) {
        split_child(node, node->childs[child], child);
        if(node->data[child] < x)
            child++;
    }

    insert(x, node->childs[child]);
}

void insert_root(int x) {
    if (root == NULL) {
        root = new_node();
        root->data[0] = x;
        root->size++;
        return;
    }
    if (root->size == BTree_order) {
        BTree* new_root_node = new_node();
        new_root_node->childs[0] = root;
        new_root_node->is_leaf = false;
        split_child(new_root_node, root, 0);

        int child = (new_root_node->data[0] < x ? 1 : 0);

        insert(x, new_root_node->childs[child]);
        root = new_root_node;
        return;
    } 

    return insert(x, root);
}

bool search(int x, BTree* node=root) {
    if(root == NULL)
        return false;

    int i;
    for(i = 0; i < node->size; i++)
        if(x <= node->data[i])
            break;

    if(node->data[i] == x)
        return true;
    
    if(node->is_leaf)
        return false;
    
    return search(x, node->childs[i]);
}

void clear_tree(BTree* node=root) {
    for(int i = 0; i <= node->size; i++) {
        if(node->is_leaf == false)
            clear_tree(node->childs[i]);
        delete node->childs[i];
    }
    
    if(node == root) {
        delete node;
        root = NULL;
    }

}

string hashed_table[MAXM];

int base_hsh(string x) {
    int res = 0;
    for(int i = 0; i < x.size(); i++) {
        res = (res * 2) % MAXM + x[i];
        res = res % MAXM;
    }
    return res;
}

int hsh(string x) {
    int index = base_hsh(x);
    while(hashed_table[index] != x) {
        index = (index + 1) % MAXM;
        if(hashed_table[index] == "")
            hashed_table[index] = x;
    }
    return index;
}

bool is_visited(int x) {
    return search(x);
}

void visit_string(int x) {
    insert_root(x);
}

void clear_visited() {
    clear_tree();
}

struct Cell {
    Cell* last = NULL;
    string value;
};

struct Stack{
    int size = 0;
    Cell* tp = NULL;

    bool empty() {
        return size == 0;
    }

    void push(string x) {
        Cell* tmp = new Cell;
        tmp->last = tp;
        tmp->value = x;
        tp = tmp;
        size++;
    }

    void pop() {
        Cell* tmp = tp;
        tp = tp->last;
        delete tmp;
        size--;
    }

    string top() {
        return tp->value;
    }

    void clear() {
        while(!empty()) {
            pop();
        }
    }

} dfs_order, scc;

struct Node {
    string value;
    Node *next;
};

struct List {
    Node *first = NULL;
    Node *last = first;

    void add(string x) {
        Node *tmp = new Node;
        tmp->next = NULL;
        tmp->value = x;
        if(last != NULL)
            last->next = tmp;
        last = tmp;
        if(first == NULL)
            first = last;
    }

    void clear() {
        while(last != NULL) {
            Node *tmp = first;
            first = first->next;
            delete tmp;
        }
    }

    string get_min() {
        Node *tmp = first;
        string res = "";
        while(tmp != NULL) {
            if(tmp->value < res || res == "")
                res = tmp->value;
            tmp = tmp->next;
        }
        return res;
    }

} follows[MAXM], followed[MAXM], result[MAXM];



void dfs(string s) {
    int x = hsh(s);
    if(is_visited(x)) {
        return;
    }
    visit_string(x);

    Node *j = follows[x].first;
    while(j != NULL) {
        dfs(j->value);
        j = j->next;
    }
    dfs_order.push(s);
}

void dfs_util(string s) {
    int x = hsh(s);
    if(is_visited(x))
        return;

    visit_string(x);
    scc.push(s);

    Node *j = followed[x].first;
    while(j != NULL) {
        dfs_util(j->value);
        j = j->next;
    }
}


void add_edge(string x, string y) {
    int x_hash = hsh(x), y_hash = hsh(y);

    follows[x_hash].add(y);
    followed[y_hash].add(x);
}


int order[MAXM];
string minString[MAXM];
bool cmp(int x, int y) {
    return minString[x] < minString[y];
}

bool cmp2(Node x, Node y) {
    return x.value < y.value;
}

int main() {
    int n;
    cin>>n;

    string names[MAXM];
    string tmp_name, new_name, name_stream;

    getline(cin, tmp_name);
    for(int i = 0; i < n; i++) {
        getline(cin, tmp_name);
        name_stream = "";
        for(int j = 0; j < tmp_name.size(); j++) {
            if(tmp_name[j] == ':') {
                new_name = name_stream;
                names[i] = new_name;
                name_stream = "";
            }
            else if(tmp_name[j] == ' ') {
                add_edge(names[i], name_stream);
                name_stream = "";
            }
            else {
                name_stream = name_stream + tmp_name[j];
            }
            if(j == tmp_name.size() - 1) {
                add_edge(names[i], name_stream);
                name_stream = "";
            }

        }
    }

    for(int i = 0; i < n; i++) {
        dfs(names[i]);
    }


    int result_size[MAXM];
    int result_count = 0;

    clear_visited();
    while(!dfs_order.empty()) {
        string top = dfs_order.top();
        dfs_order.pop();
        if(is_visited(hsh(top)))
            continue;

        dfs_util(top);
        
        result_size[result_count] = 0;
        while(!scc.empty()) {
            result[result_count].add(scc.top());
            result_size[result_count]++;
            scc.pop();
        }
        result_count++;
    }

    scc.clear();

    for(int i = 0; i < result_count; i++) {
        order[i] = i;
        minString[i] = result[i].get_min();
    }
    sort(order, order + result_count, cmp);

    string for_sort[MAXM];
    for(int i = 0; i < result_count; i++) {
        int siz = 0;
        Node *j = result[order[i]].first;
        while(j != NULL) {
            for_sort[siz] = j->value;
            siz++;
            j = j->next;
        }
        sort(for_sort, for_sort + siz);
        for(int j = 0; j < siz; j++) {
            cout<<for_sort[j];
            if(j != siz - 1)
                cout<<' ';
        }
        cout<<endl;
    }
}