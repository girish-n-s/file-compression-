#include<iostream>
#include<vector>
#include<unordered_map>
#include<algorithm>
#include<fstream>
#include<cstdint>
#include<string>
#include<sstream>
using namespace std;

//////template of a node of huffman tree
struct node{
    char ch;
    int freq;
    node* left,*right;
    node(){}
    node(char c,int f){
        ch=c;
        freq=f;
        left=right=NULL;
    }
};
///this struct is created to support sorting of codes based on length
struct codes{
    char ch;
    string code;
    codes(){}
    codes(char c,string s){
        ch=c;
        code=s;
    }
};
//node * getnode(char c,int f);
node* createTree(node ** leaf,int n);
node * addnode(node * a,node * b);
bool compfreq(node  *a, node *b) {
    return a->freq<b->freq;
}
bool compcode(codes &a,codes &b){
    return a.code.length()>b.code.length();
}
void encode(node * tree,string str,unordered_map<char,string>& codes);
void writefile(int ,codes*,int,vector<uint8_t>,int ,vector<uint8_t>);
void freeTree(node* root);
vector<uint8_t> compress(string);
int main(){
    ifstream file("test cases/test2.txt");
    stringstream buffer;
    buffer<<file.rdbuf();
    string text= buffer.str();
    ///hashmap to map characters with its corresponding frequencies
    unordered_map<char,int> frequency;
    //calculating frquency of characters
    for(char a : text)
        frequency[a]++;
    //charCount stores the number of different characters present in the text
    int charCount=frequency.size();
    //cout<<charCount;
    /////an array node* to store address of leaf node the size of array is increased by one to construct tree easyly
    node * leaf[charCount+1];
    int i=0;

    //creating leaf nodes
    for(auto a : frequency){
       // leaf[i++]=getnode((char)a.first,(int)a.second);
       leaf[i++]=new node(a.first,a.second);
    }
    ///root holds the address of the  huffman tree
    node * root=NULL;
    root=createTree(leaf,charCount);
    ///map to store character with its repective code
    unordered_map<char,string> huffmanCodemap;
    encode(root,"",huffmanCodemap);
    string binaryText="";
    for(char a : text)
        binaryText+=huffmanCodemap[a];
    vector<uint8_t> byteArray;
    byteArray=compress(binaryText);
    int binaryTextpadding = (8-(binaryText.length()%8))%8;
    codes huffmanCode[huffmanCodemap.size()];
    i=0;
    for(auto a : huffmanCodemap) {
        huffmanCode[i].ch=a.first;
        huffmanCode[i].code=a.second;
        i++;
    }
    sort(huffmanCode,huffmanCode+charCount,compcode);
    string binaryCode="";
    for(codes a: huffmanCode)
        binaryCode+=a.code;
    int binaryCodePadding=(8-(binaryCode.length()%8))%8;
    vector<uint8_t> codeArray=compress(binaryCode);
    writefile(charCount,huffmanCode,binaryCodePadding,codeArray,binaryTextpadding,byteArray);
    freeTree(root);

    ///releasing the memory of leaf node acquired dynamically
    return 0;
}

node * createTree(node ** leaf,int n) {
    while(n>1) {
        sort(leaf,leaf+n,compfreq);
        leaf[n]=addnode(leaf[0],leaf[1]);
        for(int i=0; i<n-1; i++) 
        leaf[i]=leaf[i+2];
        n--;
    }
    return leaf[0];
}

node * addnode(node * a,node * b) {
    node * n= new node;
    n->ch='\0';
    n->freq=a->freq+b->freq;
    n->left=a;
    n->right=b;
    return n;
}

void encode(node * tree,string str,unordered_map<char,string>& codes) {
    if(!tree)
        return;
    if(!tree->left && !tree->right)
        codes[tree->ch]=str;
    else {
        encode(tree->left,str+'0',codes);
        encode(tree->right,str+'1',codes);
    }
}
vector<uint8_t> compress(string s){
    vector<uint8_t> array;
    uint8_t byte=0;
    int bitCount=0;
    for(char a : s){
        if(a=='1'){
            byte|=(1<<(7-bitCount));
        }
        bitCount++;
        if(bitCount==8){
            array.push_back(byte);
            bitCount=0;
            byte=0;
        }
    }
    if(bitCount)
        array.push_back(byte);

    return array;
}


void writefile(int charCount ,codes* huffmanCodes,int codePadding,vector<uint8_t> codeArray,int textPadding ,vector<uint8_t> textArray){

ofstream compressedfile("test cases/test2.bin",ios::binary);
uint8_t cc,cp,tp;
cc=(uint8_t)charCount;
cp=(uint8_t)codePadding;
tp=(uint8_t)textPadding;
cout<<(int)tp;
uint8_t codelength;
if(compressedfile.is_open()){
compressedfile.write(reinterpret_cast<char*>(&cc),sizeof(cc));
for(int i=0;i<charCount;i++){
    codelength=(uint8_t)huffmanCodes[i].code.length();
    compressedfile.write(reinterpret_cast<char*>(&huffmanCodes[i].ch), sizeof(huffmanCodes[i].ch));
    compressedfile.write(reinterpret_cast<char*>(&codelength),sizeof(codelength));
}
compressedfile.write(reinterpret_cast<char*>(&cp),sizeof(cp));
uint8_t NoOfByteCode=(uint8_t)codeArray.size();
compressedfile.write(reinterpret_cast<char*>(&NoOfByteCode),sizeof(NoOfByteCode));
compressedfile.write(reinterpret_cast<char*>(codeArray.data()),codeArray.size());
compressedfile.write(reinterpret_cast<char*>(&tp),sizeof(tp));
compressedfile.write(reinterpret_cast<char*>(textArray.data()),textArray.size());
compressedfile.close();
}
else
cout<<"opening file failure"<<endl;
}

void freeTree(node* root) {
    if (!root) return;

    // Recursively delete left and right subtrees
    freeTree(root->left);
    freeTree(root->right);

    // Delete current node
    delete root;
}
