// Ronen Burd
//Lab 5: The encoding part
//CIS 28
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <queue>
#include <istream>
#include <regex>
#include <bitset>

using namespace std;

class HuffmanNode {
private:
	string key;
	int frequency;
	
public:
	friend bool operator < (const HuffmanNode& node1, const HuffmanNode& node2) {
		HuffmanNode& first = const_cast<HuffmanNode&>(node1);
		HuffmanNode& second = const_cast<HuffmanNode&>(node2);
		return first.getFrequency() < second.getFrequency();
	}

	bool isBranchNode() {
		return false;
	}

	void setKey(string strKey) {
		key = strKey;
	}
	string getKey(){
		return key;
	}

	void setFrequency(int numFrequency) {
		frequency = numFrequency;
	}
	int getFrequency() {
		return frequency;
	}

	HuffmanNode();
	~HuffmanNode();
};

class HuffmanBranchNode : public HuffmanNode {
private:
	HuffmanNode* left;
	HuffmanNode* right;
public:
	bool isBranchNode() {
		return true;
	}

	void setLeftNode(HuffmanNode* leftNode) {
		left = leftNode;
	}
	HuffmanNode* getLeftNode() {
		return left;
	}

	void setRightNode(HuffmanNode* rightNode) {
		right = rightNode;
	}
	HuffmanNode* getRightNode() {
		return right;
	}

	HuffmanBranchNode() {
		left = nullptr;
		right = nullptr;
	}
	~HuffmanBranchNode() {
		delete left;
		delete right;
	}
};
class XMLReader {
private:
	regex symbolRgx;
	const string symbolTag = "<.?>";
	regex freqRgx;
	const string freqTag = "[0-9]+<";

	set <HuffmanNode*, NodeLess> leafNodes;

	void createSymbolNodeFromLine(const string& text) {
		HuffmanNode* node;
		node = new HuffmanNode();
	
		smatch res; 
		regex_search(text, res, symbolRgx);
		string symbolResult = res[1];
		node->setKey(symbolResult);

		regex_search(text, res, freqRgx);
		int frequency = atoi(res[1].str().c_str());
		node->setFrequency(frequency);

		leafNodes.insert(node);
	}

public:
	void readFileIntoNodeQueue(istream& istr){
		string text;
		while (getline(istr,text)) {
			createSymbolNodeFromLine(text);
		}
	}
	const set<HuffmanNode*, NodeLess> getLeafNodes() {
		return leafNodes;
	}
	XMLReader() {
		symbolRgx = symbolTag;
		freqRgx = freqTag;
	}
};



struct NodeLess{
	bool operator()(const HuffmanNode& node1, HuffmanNode& node2) {
		return node1 < node2;
	}
};

class HuffmanTree {
private:
	set<HuffmanNode*, NodeLess> treeSet;
	map<char, string> symbolEncodings;

public:
	void encodeAllSymbols(){
		string pattern;
		HuffmanNode* root = *(treeSet.begin);
		_encodeAllSymbols(pattern, root);
	}

	void _encodeAllSymbols(string& encodingPattern, HuffmanNode* node) {
		if (node->isBranchNode()) {
			HuffmanBranchNode* bn = dynamic_cast<HuffmanBranchNode*>(node);
			encodingPattern += '0';
			_encodeAllSymbols(encodingPattern, bn->getLeftNode());
			encodingPattern += '1';
			_encodeAllSymbols(encodingPattern, bn->getRightNode());
		}
		else {
			pair<char, string> encodingPair;
			string key = node->getKey();
			encodingPair.first = key[0];
			encodingPair.second = encodingPattern;
			
			int length = encodingPattern.length();
			encodingPattern.resize(length - 1);
		}
	}

	string getEncodingForSymbol(char symbol) {
		map<char, string> :: iterator it = symbolEncodings.find(symbol);
		return (*it).second;
	}

	char decodeCharFromBits(const string& bits);
	
	void buildTreeFromLeafNodes(set<HuffmanNode*>& nodes) {
		while (nodes.size() > 1) {
			string key1;
			string key2;
			int freq1;
			int freq2;
			HuffmanNode* left;
			HuffmanNode* right;

			processMinimumNode(nodes, key1, freq1, left);
			processMinimumNode(nodes, key2, freq2, right);

			HuffmanBranchNode* newNode;
			newNode->setKey(key1 + key2);
			newNode->setFrequency(freq1 + freq2);
			newNode->setLeftNode(left);
			newNode->setRightNode(right);
			nodes.insert(newNode);
						
			treeSet.erase(left);
			treeSet.erase(right);
		}

		HuffmanNode* root = *(nodes.begin());
		treeSet.insert(root);
		nodes.erase(root);
	}

	void processMinimumNode(set<HuffmanNode*>& nodes, string& key, int& freq, HuffmanNode* min){
		HuffmanNode* left = *(nodes.begin());
		key = min->getKey();
		freq = min->getFrequency;
		treeSet.insert(min);
	}

	HuffmanTree();

	~HuffmanTree() {
		set<HuffmanNode*> :: iterator it;
		for (it = treeSet.begin(); it != treeSet.end(); it++) {
			delete *it;
		}
	}
};

class TextFileEncryptor{
private:
	ostream& output;
	HuffmanTree* ht;
public:
	void processInputStream(istream& input) {
		char c;
		bitset<8> outputSymbol;
		string pattern;

		while (input >> c) {
			pattern += ht->getEncodingForSymbol(c);
			int plength = pattern.length;
			
			while (plength >= 8) {
				outputSymbol = bitset<8> (pattern.substr(0, 8));
				unsigned long i = outputSymbol.to_ulong();
				output << static_cast <unsigned char> (i);

				pattern.erase(0, 8);
				plength -= 8;
			}
		}
	}

	TextFileEncryptor(HuffmanTree* huffman, ostream& os) : output(os) {
		ht = huffman;
	}
};
