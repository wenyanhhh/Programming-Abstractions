/* This file grammarsolver.cpp was a collaboratiev work between Wenyan Hua
 * and Shaw Lu on Oct 25, 2017 as homework 3 for the class CS106B Programming
 * Absraction.
 *
 * User interface:
 * (1) User enters a file delineating grammar rules;
 * (2) User enters a symbol (non-terminal word);
 * (3) User enters number of sentences to generate;
 * (4) Program randomly generates specified number of sentences;
 * (5) repeat (1) or quit program. */

#include "grammarsolver.h"
#include "map.h"

using namespace std;

bool isNonTerminal(Vector<string> nonTerminals, string symbol);
void generateSentence(Map<string, Vector<Vector<string>>> const language, string const symbol, string &sentence);
Vector<string> grammarGenerate(istream& input, string symbol, int times);

/* accepts a vector of non-terminal words and a symbol.
 * check if the symbol is a non-terminal word.
 * @param nonTerminals a vector of non-terminal words
 * @param symbol a string
 * @return true if the string is in the vector. */
bool isNonTerminal(Vector<string> nonTerminals, string symbol) {
    for (int i = 0; i < nonTerminals.size(); i++) {
        if (nonTerminals[i] == symbol) return true;
    }
    return false;
}

/* accepts a map detailing language rules and a symbol (terminal
 * or non-terminal), generate a sentence based on the rules.
 * @param language map containing all rules of a language.
 * @param symbol a terminal word or non-terminal word.
 * @param &sentence a string to store randomly generated terminal words. */
void generateSentence(Map<string, Vector<Vector<string>>> const language,
                      string const symbol, string &sentence) {
    Vector<string> nonTerminals = language.keys();
    if (!isNonTerminal(nonTerminals, symbol)) {
        sentence = sentence + " " +symbol;
        return;
    }
    Vector<Vector<string>> allRules = language[symbol];
    int rand = randomInteger(0, allRules.size()-1);
    Vector<string> rule = allRules[rand];
    for (int i = 0; i < rule.size(); i++) {
        generateSentence(language, rule[i], sentence);
    }
}

/* accepts an input stream file and a symbol (terminal or non terminal)
 * randomly generate a sentence multiple times.
 * @param input grammar rules to be segmented and stored in a map.
 * @param symbol non-terminal symbol to be resursively expanded.
 * @param times number of sentences to generate.
 * @returns a vector containning all synthesized sentences. */
Vector<string> grammarGenerate(istream& input, string symbol, int times) {
    Map<string, Vector<Vector<string>>> language;
    string line;
    while (getline(input, line)) {
        Vector<string> keySplit = stringSplit(line, "::=");
        Vector<string> ruleSplit = stringSplit(keySplit[1], "|");
        Vector<Vector<string>> rules;
        if (language.containsKey(keySplit[0])) {
            throw keySplit[0];
        }
        for (int i = 0; i < ruleSplit.size(); i++) {
            Vector<string> words = stringSplit(ruleSplit[i], " ");
            rules.add(words);
        }
        language[keySplit[0]] = rules;
    }
    Vector<string> v;
    if (symbol == "\"\"") {
        throw symbol;
        return v;
    }
    for (int i = 0; i < times; i++) {
        string sentence = "";
        generateSentence(language, symbol, sentence);
        v.add(trim(sentence));
    }
    return v;
}
