/* This is the 6th collaborative assignment between
 * Shaw Lu and Wenyan Hua due on Monday Nov 27. It
 * implements the Huffman encoding algorithm which
 * uses a priority queue to build an encoding tree.
 *
 * We used PriorityQueue class from Stanford C++
 * libraries to save a queue of Huffman node, which
 * may or may not contain a character value. Parent
 * count is always the sum of the two childdren's count.
 *
 * This program may accepts a string or file as input
 * and encode it using Huffman algorithm, or decode
 * a huffman code file containing a header and body. */

#include "encoding.h"
#include "filelib.h"
#include "pqueue.h"
#include "HuffmanNode.h"

bool dev = true;

/* Build a frequency table counting occurence of each character
 * @param input user input string/file
 * @return Map<int, int> maps key (character) to count */
Map<int, int> buildFrequencyTable(istream& input) {
    Map<int, int> freqTable;
    int ch = input.get();
    // loop until reaching end-of-file (ch = -1)
    while (ch != -1) {
        freqTable[ch] += 1;
        ch = input.get();
    }
    // The end-of-file is -1, but its ASCII code if 256
    freqTable[256] = 1;
    return freqTable;
}

/* Transform the frequency table into a Huffman encoding tree
 * each parent has a count value as the sum of its two children's
 * count values.
 * @param freqTable frequency table built from previous method.
 * @return HuffmanNode root node of the encoding tree. */
HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    PriorityQueue<HuffmanNode*> q;
    // create a priority queue of HuffmanNodes
    for (int ch : freqTable.keys()) {
        HuffmanNode *node = new HuffmanNode(ch, freqTable[ch], NULL, NULL);
        q.enqueue(node, node->count);
    }
    // construct the encoding tree
    while (q.size() != 1) {
        HuffmanNode *node1 = q.dequeue();
        HuffmanNode *node2 = q.dequeue();
        HuffmanNode *parent= new HuffmanNode(NOT_A_CHAR, node1->count+node2->count, node1, node2);
        q.enqueue(parent, parent->count);
    }
    // return the root node (the only element in queue)
    return q.dequeue();
}

/* Recursively build an encoding map by traversing the encoding the
 * tree and storing the code value to a map every time a node with
 * a character value is reached.
 * @param encodingMap maps character to binary Huffman code value
 * @param node current HuffmanNode
 * @param code the binary code that leads to current node */
void traverseTree(Map<int, string> &encodingMap, HuffmanNode* node, string code) {
    if (node->character != NOT_A_CHAR) {
        encodingMap[node->character] = code;
    }
    // implicit base case: reaching the end of the tree
    if (node->zero != NULL) traverseTree(encodingMap, node->zero, code+"0");
    if (node->one != NULL) traverseTree(encodingMap, node->one, code+"1");
}

/* Build an encoding map by calling the recursive traverse the encoding tree
 * @param encodingTree built from earlier function
 * @return Map<int, string> maps character to Huffman code value */
Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    traverseTree(encodingMap, encodingTree, "");
    return encodingMap;
}

/* A utility function that writes a Huffman code (string)
 * bit by bit into an output obitstream. */
void encodeToStrem(string code, obitstream& output) {
    while (code != "") {
        string bit = code.substr(0, 1);
        output.writeBit(stringToInteger(bit));
        code = code.substr(1);
    }
}

/* Encode the input by looking up the Huffman code for each
 * character and write it to output stream by calling the
 * utility function above.
 * @param input the input message (string or file)
 * @param encodingMap maps character to Huffman code
 * @param output an empty output message to be filled bit by bit */
void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    int ch = input.get();
    // loop until the end-of-file (ch = -1)
    while (ch != -1) {
        string code = encodingMap[ch];
        encodeToStrem(code, output);
        ch = input.get();
    }
    // encode end-of-file, whose key is 256
    string code = encodingMap[256];
    encodeToStrem(code, output);
}

/* Recursively decode a message by traversing the encoding tree
 * add one character to output every time a node with a character
 * is reached.
 * @param input the input message (string or file)
 * @param encodingTree first node to the encoding tree
 * @param output an empty output message to be filled bit by bit
 * @return true if end of input stream is reached */
bool decodeDataHelper(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    if (encodingTree->character == PSEUDO_EOF) {
        // finished encoding
        return true;
    } else if (encodingTree->character != NOT_A_CHAR) {
        // continue encoding, starting over from the root node
        output.put(encodingTree->character);
        return false;
    }
    int bit = input.readBit();
    if (bit == 0) {
        // report back to earlier call
        return decodeDataHelper(input, encodingTree->zero, output);
    } else {
        // report back to earlier call
        return decodeDataHelper(input, encodingTree->one, output);
    }
}

/* Decode the input stream by calling a recursive helper function
 * @param input the input message (string or file)
 * @param encodingTree first node to the encoding tree
 * @param output an empty output message to be filled bit by bit */
void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    bool finishedDecoding = false;
    while (!finishedDecoding) {
        finishedDecoding = decodeDataHelper(input, encodingTree, output);
    }
}

/* Compress a file into .huf format
 * @param input a file stream
 * @param output encoded stream */
void compress(istream& input, obitstream& output) {
    Map<int, int> freqTable = buildFrequencyTable(input);

    // save frequency table as header
    output << freqTable;

    // must unwind to the beginning
    rewindStream(input);
    HuffmanNode *root = buildEncodingTree(freqTable);
    Map<int, string> encodingMap = buildEncodingMap(root);
    encodeData(input, encodingMap, output);
}

/* Decompress a file from .huf format
 * @param input an encoded stream including a header
 * @param output write to the uncompressed format */
void decompress(ibitstream& input, ostream& output) {
    Map<int, int> freqTable;
    // read header
    input >> freqTable;
    HuffmanNode *root = buildEncodingTree(freqTable);
    decodeData(input, root, output);
}

/* recursively delete node, starting from the end(bottom) of the tree
 * @param node current node */
void freeTreeHelper(HuffmanNode* node) {
    // implicit base case: no more child node
    if (node->zero) freeTreeHelper(node->zero);
    if (node->one) freeTreeHelper(node->one);
    if (dev) cout << "delete node with char: " << node->character << " count:" << node->count << endl;
    delete node;
}

// Free tree memory by calling the recursive helper function above
void freeTree(HuffmanNode* node) {
    freeTreeHelper(node);
}
