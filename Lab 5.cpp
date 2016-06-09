// Ronen Burd
//Lab 5: The encoding part
//CIS 28
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <istream>
#include <ostream>
#include <fstream>
#include <regex>
#include <bitset>
#include <memory>
#include <sstream>

using namespace std;

class HuffmanNode {
private:
	string key;
	int frequency;
	
public:
	virtual bool isBranchNode() {
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

	HuffmanNode() {

	}
	~HuffmanNode() {

	}
};

class HuffmanBranchNode : public HuffmanNode {
private:
	HuffmanNode* left;
	HuffmanNode* right;
public:
	virtual bool isBranchNode() {
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

struct NodeLess{
	bool operator()(HuffmanNode* node1, HuffmanNode* node2) {
		if (!node1)
			return true;
		if (!node2)
			return false;
		return node1->getFrequency() < node2->getFrequency();
	}
};


class XMLReader {
private:
	regex symbolRgx;
	const string symbolTag = "<(.?)>";
	regex freqRgx;
	const string freqTag = "([0-9]+)<";

	multiset <HuffmanNode*, NodeLess> leafNodes;

	void createSymbolNodeFromLine(const string& text);

public:
	void readFileIntoNodeQueue(istream& istr){
		string text;
		while (getline(istr, text)) {
			createSymbolNodeFromLine(text);
			}
	}

	const multiset<HuffmanNode*, NodeLess>& getLeafNodes() {
		return leafNodes;
	}

	XMLReader() {
		symbolRgx = symbolTag;
		freqRgx = freqTag;
	}
};

void XMLReader::createSymbolNodeFromLine(const string& text){
	HuffmanNode* node = new HuffmanNode();

	smatch res;
	regex_search(text, res, symbolRgx);
	string symbolResult = res[1];
	node->setKey(symbolResult);

	regex_search(text, res, freqRgx);
	int frequency = atoi(res[1].str().c_str());
	node->setFrequency(frequency);

	int sizeBefore = leafNodes.size();
	leafNodes.insert(node);
	if (sizeBefore == leafNodes.size()) {
		cout << endl << "SleafNode size did not change after insertion!" << endl;
	}
}


class HuffmanTree {
private:
	multiset<HuffmanNode*, NodeLess> treeSet;
	map<char, string> symbolEncodings;

public:
	void encodeAllSymbols(){
		if (treeSet.empty()) {
			return;
		}

		string pattern;
		HuffmanNode* root = *(treeSet.rbegin());
		_encodeAllSymbols(pattern, root);
	}

	void _encodeAllSymbols(string& encodingPattern, HuffmanNode* node);

	string getEncodingForSymbol(char symbol) {
		map<char, string> :: iterator it = symbolEncodings.find(symbol);
		if (it == symbolEncodings.end()) {
			cout << "Nope" << endl;
			return "";
		}
		return it->second;
	}

	char decodeCharFromBits(const string& bits) {
		map<char, string>::iterator it;
		for (it = symbolEncodings.begin(); it != symbolEncodings.end(); it++) {
			if ((*it).second == bits){
				return it->first;
			}
		}
		cout << endl << "Error! Not found encoding!" << endl;
		return ' ';
	}
	bool containsEncodingPattern(string pattern) {
		map<char,string>::iterator it;
		for (it = symbolEncodings.begin(); it != symbolEncodings.end(); ++it) {
			if ((*it).second == pattern){
				return true;
			}
		}
		return false;
	}
	
	void buildTreeFromLeafNodes(multiset<HuffmanNode*, NodeLess>& nodes);

	void processMinimumNode(multiset<HuffmanNode*, NodeLess>& nodes, string& key, int& freq, HuffmanNode*& min){
		min = *(nodes.begin());
		key = min->getKey();
		freq = min->getFrequency();
		treeSet.insert(min);
		nodes.erase(nodes.begin());
	}

	HuffmanTree(){
	}

	~HuffmanTree() {
	//	multiset<HuffmanNode*> :: iterator it;
	//	for (it = treeSet.begin(); it != treeSet.end(); ++it) {
	//		delete *it;
	//		treeSet.erase(it);
	//	}
	}
};

void HuffmanTree::_encodeAllSymbols(string& encodingPattern, HuffmanNode* node) {
	if (!node) return;
	if (node->isBranchNode()) {
		HuffmanBranchNode* bn = dynamic_cast<HuffmanBranchNode*>(node);
		encodingPattern += '0';
		_encodeAllSymbols(encodingPattern, bn->getLeftNode());
		encodingPattern = encodingPattern.substr(0, encodingPattern.length() - 1);

		encodingPattern += '1';
		_encodeAllSymbols(encodingPattern, bn->getRightNode());
		encodingPattern = encodingPattern.substr(0, encodingPattern.length() - 1);
	}
	else {
		pair<char, string> encodingPair;
		string key = node->getKey();
		encodingPair.first = key[0];
		encodingPair.second = encodingPattern;		
		symbolEncodings.insert(encodingPair);
	}
}

void HuffmanTree::buildTreeFromLeafNodes(multiset<HuffmanNode*, NodeLess>& nodes) {
	while (nodes.size() > 1) {
		string key1;
		string key2;
		int freq1;
		int freq2;
		HuffmanNode* left = nullptr;
		HuffmanNode* right = nullptr;

		processMinimumNode(nodes, key1, freq1, left);
		processMinimumNode(nodes, key2, freq2, right);

		HuffmanBranchNode* newNode = new HuffmanBranchNode();
		newNode->setKey(key1 + key2);
		newNode->setFrequency(freq1 + freq2);
		newNode->setLeftNode(left);
		newNode->setRightNode(right);
		nodes.insert(newNode);
	}

	HuffmanNode* root = *(nodes.begin());
	treeSet.insert(root);
	nodes.erase(root);
}

class TextEncryptor{
private:
	ostream& binaryOutput;
	HuffmanTree* ht;
public:
	void processInputStream(istream& input);

	TextEncryptor(HuffmanTree* huffman, ostream& os) : binaryOutput(os) {
		ht = huffman;
	}
	~TextEncryptor() {
	}
};

void TextEncryptor::processInputStream(istream& input) {
	char c;
	bitset<8> outputSymbol;
	string pattern;
	int patternlength;
	//ostringstream ostr;

	while (input >> c) {
		pattern += ht->getEncodingForSymbol(c);
		patternlength = pattern.length();

		while (patternlength >= 8) {
			outputSymbol = bitset<8>(pattern.substr(0, 8));
			char i = (char)outputSymbol.to_ulong();
			// binaryOutput << i;
			operator << (binaryOutput, i);

			pattern.erase(0, 8);
			patternlength -= 8;
		}
	}

	if (pattern.length() > 0) {
		while (pattern.length() < 8) {
			pattern += "0";
		}
		outputSymbol = bitset<8>(pattern);
		char i = (char)outputSymbol.to_ulong();
		//ostr << (char)i;
		//binaryOutput << i;
		operator << (binaryOutput, i);
	}
}

class BinaryDecryptor {
private:
	ostream& textOutput;
	HuffmanTree* ht;
public:
	void processInputStream(istream& input);

	BinaryDecryptor(HuffmanTree* huffmantree, ostream& output) : textOutput(output) {
		ht = huffmantree;
	}
	~BinaryDecryptor() {
	}
};

void BinaryDecryptor::processInputStream(istream& input) {
	string currentEncoding;
	char desiredSymbol;
	char inputChar;

	while (input >> inputChar) {

		//bitset<8> inputBit(inputChar);
		for (size_t i = 0; i < sizeof(inputChar) * 8; ++i) {
			char filter = 1 << (sizeof(inputChar) * 8 - i - 1);
			if ( (inputChar & filter) ) {
				currentEncoding += '1';
			}
			else {
				currentEncoding += '0';
			}

			if (ht->containsEncodingPattern(currentEncoding)) {
				desiredSymbol = ht->decodeCharFromBits(currentEncoding);
				textOutput << desiredSymbol;
				currentEncoding.clear();
			}
		}
	}
}

////////////DEBUG METHOD///////////////////////////////
void enableDebug(bool bvalue)
{
	if (!bvalue) return;

	int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// Turn on leak-checking bit.
	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

	// Turn off CRT block checking bit.
	tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;

	// Set flag to the new value.
	_CrtSetDbgFlag(tmpFlag);
}

void main() {
	////////////DEBUG STATEMENT////////////////////////
	enableDebug(true);

	unique_ptr<XMLReader> reader(new XMLReader());
	ifstream is;
	is.open("Frequencies.txt");
	reader->readFileIntoNodeQueue(is);

	multiset<HuffmanNode*, NodeLess> leafNodes = reader->getLeafNodes();
	unique_ptr<HuffmanTree> ht (new HuffmanTree());
	ht->buildTreeFromLeafNodes(leafNodes);
	ht->encodeAllSymbols();

	ofstream os;
	string convBinName = "ConvertedBinary.txt";
	os.open(convBinName, ios_base::out | ios_base::binary);
	unique_ptr<TextEncryptor> encryptor ( new TextEncryptor(ht.get(), os) );
	ifstream is2;
	is2.open("Input Text File.txt");
	encryptor->processInputStream(is2);
	os.flush();
	os.close();
	is2.close();

	ifstream is3;
	is3.open(convBinName, ios_base::in | ios_base::binary);
	ofstream os2;
	os2.open("ConvertedAlphanumeric.txt");
	unique_ptr<BinaryDecryptor> decryptor ( new BinaryDecryptor(ht.get(), os2) );
	if (is3.is_open()){
		decryptor->processInputStream(is3);
		is3.close();
	}
	os2.close();

	system("pause");
}