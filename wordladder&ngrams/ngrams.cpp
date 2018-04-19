/* WordLadder: Part 2 of homework 2
 * This is a collaborative work between Wenyan Hua (wenyan28@stanford.edu)
 * and Shaw Lu (xiaolu@stanford.edu). The user interface is summarized as
 * (1) User enters a file.
 * (2) User enters N.
 * (3) User enters Length for the generated text, or 0 to exit.
 * (4) Program outputs the generated text.
 * (5) User enters Length for another generated text, or 0 to exit. */

#include <iostream>
#include <fstream>
#include <filelib.h>
#include "console.h"
#include "map.h"
#include "vector.h"
#include "strlib.h"
#include "simpio.h"

using namespace std;
Vector<string> text;
const string WELCOME_1      = "Welcome to CS 106B/X Random Writer ('N-Grams')!";
const string WELCOME_2      = "This program generates random text based on a document.";
const string WELCOME_3      = "Give me an input file and an 'N' value for groups";
const string WELCOME_4      = "of words, and I'll create random text for you.";
const string FILE_PROMPT    = "Input file name?";
const string N_PROMPT       = "Value of N?";
const string L_PROMPT       = "# of random words to generate (0 to quit)?";
const string EXIT           = "Exiting.";
const string N_RANGE        = "N must be 2 or greater.";
const string INT_FORMAT     = "Illegal integer format. Try again.";
const string ERROR          = "N is larger than the number of words in the file.\nUnable to compute N-grams.";

Map<Vector<string>, Vector<string>> getWordMap(int &N);
Vector<string> generateText(Map<Vector<string>, Vector<string>> &wordMap, int length, int N);
int getValidN();
int getValidLength(int N);

/* Get an N-grams map of words from prefixes to suffixes,
 * which connects a collection of N-1 words
 * with another collection of all possible suffixes.
 */
Map<Vector<string>, Vector<string>> getWordMap(int &N) {
    Map<Vector<string>, Vector<string> > wordMap;
    int length = text.size();
    // wrap around the map
    for (int i = 0; i < N-1; i++) {
        text.add(text[i]);
    }
    for (int i = 0; i <= length - 1; i++) {
        Vector<string> key = text.subList(i, N-1);
        string suffix = text[i + N - 1];
        Vector<string> val = wordMap.get(key);
        val.add(suffix);
        wordMap.put(key, val);
    }
    return wordMap;
}

/* Generate random text using wordmap
 * by randomly choosing a starting key
 * and randomly choosing a corresponding value with weighted probability,
 * Look up a new key by discarding the prefix and appending the new suffix,
 * and get a new corresponding value.
 * Returns generated text if the length is equal to input number from users.
 */
Vector<string> generateText(Map<Vector<string>, Vector<string>> &wordMap, int length, int N) {
    Vector<Vector<string>> allKeys = wordMap.keys();
    Vector<string> text;
    int randKeyIndex = randomInteger(0, allKeys.size()-1);
    Vector<string> window = allKeys[randKeyIndex];

    cout << "... ";
    for (int i = 0; i < N - 1; i ++) {
        cout << window[i] << " ";
    }
    for (int j = 0; j < length - ( N - 1 ); j ++) {
        Vector<string> vals = wordMap.get(window);
        int randValIndex = randomInteger(0, vals.size()-1);
        string next = vals[randValIndex];
        cout << next << " ";
        // slide current window
        window.remove(0);
        window.add(next);
    }
    cout << "..." << endl << endl;
    return text;
}

/* Get the value of N from user.
 * Re-prompt if the value is not an integer, or less than 2.
 */
int getValidN(){
    int N;
    while (true) {
        N = getInteger(N_PROMPT, INT_FORMAT);
        if (N < 2) {
            cout << N_RANGE << endl;
        } else {
            break;
        }
    }
    return N;
}

/* Get the number of words from user.
 * Re-prompt if the number is not an integer, or less than N.
 */
int getValidLength(int N) {
    int length;
    while (true) {
        length = getInteger(L_PROMPT, INT_FORMAT);
        if (length < N && length != 0) {
            cout << "Must be at least " << N << " words." << endl << endl;
        } else {
            break;
        }
    }
    return length;
}

int main() {
    cout << WELCOME_1 << endl;
    cout << WELCOME_2 << endl;
    cout << WELCOME_3 << endl;
    cout << WELCOME_4 << endl;
    cout << endl;

    ifstream filename;
    promptUserForFile(filename, FILE_PROMPT);
    string word;

    // read the file one word at a time and store words in Vector 'text'
    while (filename >> word) {
        text.add(word);
    }

    int N = getValidN();
    // Exit if N is greater than the number of words in file
    if (N > text.size()){
        cout << ERROR << endl;
    } else {
        cout << endl;

        bool shouldExit = false;
        while (!shouldExit) {
            int length = getValidLength(N);
            // Exit if user type 0
            if (length == 0) {
                shouldExit = true;
            } else{
                Map<Vector<string>, Vector<string>> wordMap = getWordMap(N);
                Vector<string> generatedText = generateText(wordMap, length, N);
            }
        }
    }

    cout << EXIT << endl;
    return 0;
}
