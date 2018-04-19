/* WordLadder: Part 1 of homework 2
 * This is a collaborative work between Wenyan Hua (wenyan28@stanford.edu)
 * and Shaw Lu (xiaolu@stanford.edu). The user interface is summarized as
 * (1) User enters a dictionary.
 * (2) User enters two words.
 * (3) The program outputs a sequence of words, transforming word 2 to word 1
 * changing one letter at a time. */

#include <iostream>
#include "console.h"
#include "simpio.h"
#include <fstream>
#include "timer.h"
#include "Set.h"
#include <filelib.h>
#include <queue.h>
#include <vector.h>
#include <string>

using namespace std;
const string WELCOME_TEXT1 = "Welcome to CS 106B/X Word Ladder!";
const string WELCOME_TEXT2 = "Give me two English words, and I will change the first";
const string WELCOME_TEXT3 = "into the second by changing one letter at a time.";
const string WORDS_UNFOUND = "The two words must be found in the dictionary.";
const string WORDS_LENGTH  = "The two words must be the same length.";
const string WORDS_SAME    = "The two words must be different.";
const string DICT_PROMPT   = "Dictionary file name:";
const string WORD1_PROMPT  = "Word 1 (or Enter to quit):";
const string WORD2_PROMPT  = "Word 2 (or Enter to quit):";

bool islegalWord(Set<string>& dictionary, string& word);
void getLadder(string &word1, string& word2, Set<string>& dictionary);
bool isWordPairValid(string &word1, string &word2, Set<string> &dictionary);


/* Check if the a word is in dictionary.
 * Returns true if the word is valid.
 */
bool islegalWord(Set<string>& dictionary, string& word){
    if (dictionary.contains(word)){
        return true;
    }
    return false;
}

/* Generate the shortest word ladder between two words,
 * by changing one letter each time and keeping the new word a valid word.
 * Use a queue of stacks to store possible paths.
 * Returns the last stack if word2 is found.
 * Returns nothing if there is no possible path between the input two words.
 */
void getLadder(string &word1, string& word2, Set<string>& dictionary){
    Queue<Stack<string> > queuePartialLadder;
    Stack<string> stackPartialLadder;
    stackPartialLadder.push(word1);
    queuePartialLadder.enqueue(stackPartialLadder);

    bool foundLadder = false;
    Stack <string> shortest;
    Set<string> usedWords;
    usedWords.add(word1);

    // Stop loop if all the stacks are dequeued,
    // which means there is no path between given words.
    while(!foundLadder && queuePartialLadder.size() > 0){
        Stack<string> firststack = queuePartialLadder.dequeue();
        string lastWord = firststack.peek();
        for (int i = 0; i < lastWord.length(); i++){
            string newWord = lastWord;
            for (char ch = 'a'; ch <= 'z'; ch++){
                newWord = newWord.replace(i, 1, charToString(ch));
                if (islegalWord(dictionary, newWord)
                        && newWord!=lastWord
                        && !usedWords.contains(newWord)){
                    // newWord is a neighbor
                    Stack<string> newStack = firststack;
                    newStack.push(newWord);
                    usedWords.add(newWord);
                    queuePartialLadder.enqueue(newStack);
                    // found the shortes path and print it out in reverse order
                    if (newWord == word2){
                        foundLadder = true;
                        shortest = newStack;
                        cout << "A ladder from data back to " << word1 << ":" << endl;
                        int size = shortest.size();
                        for (int i = 0; i < size; i ++) {
                            cout << shortest.pop() << " ";
                        }
                        cout << endl << endl;
                        break;
                    }
                }
            }
        }
        if (queuePartialLadder.size() == 0) {
            cout << "No word ladder found from " << word1 << " back to " << word2 << endl << endl;
        }
    }
    return;
}

/* Check if words from users are both valid */
bool isWordPairValid(string &word1, string &word2, Set<string> &dictionary) {
    if (word1 == "" || word2 == "") {
        return true;
    }
    if (!islegalWord(dictionary, word1) || !islegalWord(dictionary, word2)) {
        cout << WORDS_UNFOUND << endl;
        return false;
    }
    if (word1.length() != word2.length()) {
        cout << WORDS_LENGTH << endl;
        return false;
    }
    if (word1 == word2) {
        cout << WORDS_SAME << endl;
        return false;
    }
    return true;
}

int main() {
    cout << WELCOME_TEXT1 << endl;
    cout << WELCOME_TEXT2 << endl;
    cout << WELCOME_TEXT3 << endl;
    cout << "" << endl;

    ifstream filename;
    promptUserForFile(filename, DICT_PROMPT);
    cout << "" << endl;

    Set<string> dictionary;
    string element;
    // keep a dictionary of all English words
    while(getline(filename, element)){
        dictionary.add(element);
    }

    // User interaction
    bool shouldExit = false;
    while (!shouldExit) {
        string word1 = toLowerCase(getLine(WORD1_PROMPT));
        if (word1 == "") break;
        string word2 = toLowerCase(getLine(WORD2_PROMPT));
        if (word2 == "") break;

        while (!isWordPairValid(word1, word2, dictionary)) {
            cout << "" << endl;
            word1 = getLine(WORD1_PROMPT);
            if (word1 == "") {
                shouldExit = true;
                break;
            }
            word2 = getLine(WORD2_PROMPT);
            if (word2 == "") {
                shouldExit = true;
                break;
            }
        }
        if (shouldExit) break;
        getLadder(word1, word2, dictionary);
    }

    cout << "Have a nice day." << endl;
    return 0;
}
