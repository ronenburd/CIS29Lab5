// Ronen Burd
// Partner: Maryna Kapurova
//Lab 5
//Part that I was supposed to do: encoding
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

//by default I treat HuffmanNode as the leaf node even though it's the parent class
class HuffmanNode {
private:
	string key;
	int frequency;
	
public:
	virtual bool isBranchNode() {
		return false;
	}

	void setKey(const string& strKey) {
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

	HuffmanNode() { frequency = 0;  }
	virtual ~HuffmanNode() { }
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
		if(left) delete left;
		if(right) delete right;
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
	void readFileIntoLeafNodes(istream& istr){
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

	leafNodes.insert(node);
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
	string getEncodingForSymbol(char symbol);	
	bool containsEncodingPattern(const string& pattern);
	char getCharFromPattern(const string& pattern);
	void buildTreeFromLeafNodes(multiset<HuffmanNode*, NodeLess>& nodes);
	void processMinimumNode(multiset<HuffmanNode*, NodeLess>& nodes, string& key, int& freq, HuffmanNode*& min);

	HuffmanTree() {}

	~HuffmanTree() {
		delete *(treeSet.rbegin());
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

string HuffmanTree::getEncodingForSymbol(char symbol) {
	map<char, string> ::iterator it = symbolEncodings.find(symbol);
	if (it == symbolEncodings.end()) {
		cout << "Failed to find encoding for symbol: \'" << symbol << "\'" << endl;
		return "";
	}
	return it->second;
}

bool HuffmanTree::containsEncodingPattern(const string& pattern) {
	map<char, string>::iterator it;
	for (it = symbolEncodings.begin(); it != symbolEncodings.end(); ++it) {
		if ((*it).second == pattern){
			return true;
		}
	}
	return false;
}

char HuffmanTree::getCharFromPattern(const string& pattern) {
	map<char, string>::iterator it;
	for (it = symbolEncodings.begin(); it != symbolEncodings.end(); ++it) {
		if ((*it).second == pattern){
			return it->first;
		}
	}
	cout << "Error! Hasn't found encoding!" << endl;
	return ' ';
}

void HuffmanTree::processMinimumNode(multiset<HuffmanNode*, NodeLess>& nodes, string& key, int& freq, HuffmanNode*& min){
	min = *(nodes.begin());
	key = min->getKey();
	freq = min->getFrequency();
	treeSet.insert(min);
	nodes.erase(nodes.begin());
}

void HuffmanTree::buildTreeFromLeafNodes(multiset<HuffmanNode*, NodeLess>& nodes) {
	while (nodes.size() > 1) {
		string key1;
		string key2;
		int freq1 = 0;
		int freq2 = 0;
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
	while (input.get(c)) {
		pattern += ht->getEncodingForSymbol(c);

		while ( pattern.length() >= 8) {
			outputSymbol = bitset<8>(pattern.substr(0, 8));
			char i = (char)outputSymbol.to_ulong();
			operator << (binaryOutput, i);

			pattern.erase(0, 8);
		}
	}

	// If last byte is not completely filled up,
	//   we append bits from white space sysmbol until the end of the last byte...
	if (pattern.length() > 0) {
		string& strWhiteSpace = ht->getEncodingForSymbol(' ');
		size_t curSpaceBit = 0;
		while (pattern.length() < 8) {
			pattern += strWhiteSpace[ curSpaceBit++ % strWhiteSpace.length() ];
		}
		outputSymbol = bitset<8>(pattern);
		char i = (char)outputSymbol.to_ulong();
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
	~BinaryDecryptor() {}
};

void BinaryDecryptor::processInputStream(istream& input) {
	string currentEncoding;
	char desiredSymbol;
	char inputChar;
	bitset<8> inputBits;

	while (input.get(inputChar)) {
		
		inputBits = inputChar;
		string inputString = inputBits.to_string();
		for (size_t i = 0; i < 8; ++i) {
			currentEncoding += inputString[i];
			if (ht->containsEncodingPattern(currentEncoding)) {
				desiredSymbol = ht->getCharFromPattern(currentEncoding);
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

void getFileName(string whichFile, string& fileName) {
	cout << "Enter file name for " << whichFile << ": " << endl;
	getline(cin, fileName);
}

void main() {
	////////////DEBUG STATEMENT////////////////////////
	enableDebug(true);
	
	string freqFileName;
	getFileName("frequency table", freqFileName);
	string inputAlphanumTextFileName;
	getFileName("input alphanumeric text", inputAlphanumTextFileName);
	string convBinName;
	getFileName("converted binary text", convBinName);
	string alphanumOutputName;
	getFileName("alphanumeric output", alphanumOutputName);

	//read in frequency table and build leaf nodes
	unique_ptr<XMLReader> reader(new XMLReader());
	ifstream freqTableFile;
	freqTableFile.open(freqFileName);
	reader->readFileIntoLeafNodes(freqTableFile);

	//build tree from the leaf nodes and encode all the symbols
	multiset<HuffmanNode*, NodeLess> leafNodes = reader->getLeafNodes();
	unique_ptr<HuffmanTree> ht (new HuffmanTree());
	ht->buildTreeFromLeafNodes(leafNodes);
	ht->encodeAllSymbols();

	//set up input alphanumeric text file and output binary file
	ifstream inputAlphanumTextFile;
	inputAlphanumTextFile.open(inputAlphanumTextFileName);
	ofstream convertedBinaryFile;
	convertedBinaryFile.open(convBinName, ios_base::out | ios_base::binary);

	//create text encryptor to convert alphanumeric text to binary text
	unique_ptr<TextEncryptor> encryptor ( new TextEncryptor(ht.get(), convertedBinaryFile) );
	encryptor->processInputStream(inputAlphanumTextFile);
	convertedBinaryFile.close();
	inputAlphanumTextFile.close();

	//set up binary input file and alphanumeric output file
	ifstream binaryInput;
	binaryInput.open(convBinName, ios_base::in | ios_base::binary);
	ofstream alphanumericOutput;
	alphanumericOutput.open(alphanumOutputName);

	//create binary decryptor to convert binary text to alphanumeric text
	unique_ptr<BinaryDecryptor> decryptor ( new BinaryDecryptor(ht.get(), alphanumericOutput) );
	if (binaryInput.is_open()){
		decryptor->processInputStream(binaryInput);
		binaryInput.close();
	}
	alphanumericOutput.close();

	cout << endl;
	cout << "Frequency table located in " << freqFileName << endl;
	cout << "Input alphanumeric text located in " << inputAlphanumTextFileName << endl;
	cout << "Converted binary text located in" << convBinName << endl;
	cout << "Final converted alphanumeric text located in " << alphanumOutputName << endl;

	system("pause");
}