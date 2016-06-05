/*
 Ronen Burd
 Maryna Kapurova
 Lab 5: Huffman Encoding
 04 June 2016
 main.cpp
 Purpose: Use a variety of STL structure to use Huffman Encoding
 */

// #ifndef Lab5_H_
// #define Lab5_H_
// #include "Lab5.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stack>
#include <queue>
#include <regex>
#include <list>
#include <iterator>

using namespace std;

// #endif

// programmed by
void signature()
{
    cout << " " << endl;
    cout << "Programmed by: " << endl;
    cout << "Ronen Burd" << endl;
    cout << "Maryna Kapurova" << endl;
    cout << " " << endl;
}

/*
 // Memory Leak Check: Visual Studio
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
 */

int main() {
    ifstream fin;
    string line;
    
    // Memory Leak Check: Visual Studio
    // enableDebug(true);
    
    // Open and read file
    fin.open("Frequencies.txt");
    if (fin.fail())
    {
        cout << "Input file failed to open!\n";
        exit(-1);
    }
    
    // Process File
    while (!fin.eof())
    {
        getline(fin, line);
        cout << line;
    }
    
    // Close input file
    fin.close();
    cout << " " << endl;
    cout << "File processing completed." << endl;
    cout << " " << endl;
    
    // signature(); // Pogrammed by
    
    return 0;
} // main

/* Testing:
 <7>1<\7><M>1<\M><D>1<\D><2>1<\2><6>1<\6><P>1<\P><J>2<\J><B>2<\B><;>2<\;><O>2<\O><F>3<\F><C>3<\C><R>4<\R><4>4<\4><8>4<\8><K>5<\K><3>5<\3><G>5<\G><9>5<\9><1>6<\1><z>6<\z><5>6<\5><->6<\-><?>8<\?><q>8<\q><'>9<\'><N>10<\N><x>11<\x><0>12<\0><E>16<\E><j>19<\j><A>20<\A><:>21<\:><U>23<\U><W>28<\W><S>35<\S><I>45<\I><k>51<\k><T>69<\T><v>120<\v><,>146<\,><.>147<\.><b>148<\b><g>192<\g><y>209<\y><w>249<\w><m>269<\m><u>330<\u><p>338<\p><c>345<\c><f>391<\f><l>483<\l><d>486<\d><h>633<\h><r>806<\r><s>848<\s><i>861<\i><n>953<\n><a>968<\a><o>1118<\o><t>1254<\t><e>1643<\e>< >2891<\ >
 File processing completed.
 
 
 Programmed by:
 Ronen Burd
 Maryna Kapurova
 
 Program ended with exit code: 0
 ...
*/
