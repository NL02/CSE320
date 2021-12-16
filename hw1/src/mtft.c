/*
 * MTF: Encode/decode using "move-to-front" transform and Fibonacci encoding.
 */

#include <stdlib.h>
#include <stdio.h>

#include "mtft.h"
#include "debug.h"
int outputSize;
int num_Symbol; 
int currentDepth;

void printTree(MTF_NODE * node, int size)
{
    if (node == NULL)
    {
        return;
    }
    else
    {
        for (int i=0; i< size; i++)
        {
            printf("  ");
        }
        printf("CURR %p, leftC %p rightC %p PARENT %p, SYM %d, Leftcount %d RightCount %d \n", 
        node, node->left_child, node->right_child, node->parent, node->symbol, node->left_count, node->right_count);
        fflush(stdout);
        if(node == node->parent)
        {
            return;;
        }
        if (node->left_child != NULL)
        {
            printTree(node->left_child, size + 1);
        }
        if (node->right_child != NULL)
        {
            printTree(node->right_child, size + 1);
        }
    }
}

/**
 * Function to find the log base 2 of a number incremented by 1
 */
int logB2(long n)
{
    int depth = 0;
    while(n != 0 )
    {
        n = n >> 1;
        depth += 1;
    }
    return depth;
}

MTF_NODE* getNode()
{
    if (recycled_node_list != NULL)
    {
        MTF_NODE* node;
        node = recycled_node_list;
        recycled_node_list = recycled_node_list->left_child;
        return node;
    }
    // Recyle node list empty need to take from node pool
    else 
    {
        MTF_NODE* node = node_pool + first_unused_node_index;
        first_unused_node_index += 1;
        return node;
    }
}

void incrementCount(MTF_NODE *node)
{
    // if we reach the peak of the tree break
    if (node->parent == NULL)
    {
        return;
    }
    else
    {
        MTF_NODE* parent = node->parent;
        if (parent->left_child == node)
        {
            parent->left_count += 1;
        }
        else if (parent->right_child == node)
        {
            parent->right_count += 1;
        }
        incrementCount(parent);
    }
}

void addSymbol(MTF_NODE* parent, long offset, SYMBOL symbol, int nodeDepth)
{   
    int LR = offset >> (currentDepth - nodeDepth) & 1;
    if (nodeDepth  >= currentDepth)
    {
        MTF_NODE *symNode = getNode();
        symNode->left_child = NULL;
        symNode->left_count = 0;
        symNode->right_child = NULL;
        symNode->right_count = 0;
        symNode->parent = parent;
        symNode->symbol = symbol;
        if (LR == 1)
        {
            parent->right_child = symNode;
            incrementCount(symNode);
            return;
        }
        else
        {
            parent->left_child = symNode;
            incrementCount(symNode);
            return;
        }
    }
    // Not leaf node
    else 
    { 
        //Get direction 
        if (LR == 1)
        {
            MTF_NODE* rightNode;
            // If node doesn't exist create it 
            if(parent->right_child == NULL)
            {
                rightNode = getNode();
                
                rightNode->left_child = NULL;
                rightNode->left_count = 0;
                rightNode->right_child = NULL;
                rightNode->right_count = 0;
                rightNode->parent = parent;
                rightNode->symbol = NO_OFFSET;
                parent->right_child = rightNode;
            }
            rightNode = parent->right_child;
            addSymbol(rightNode, current_offset, symbol, nodeDepth + 1);
        }
        else
        {
            MTF_NODE* leftNode;
            if(parent->left_child == NULL)
            {
                leftNode = getNode();
                
                leftNode->left_child = NULL;
                leftNode->left_count = 0;
                leftNode->right_child = NULL;
                leftNode->right_count = 0;
                leftNode->parent = parent;
                leftNode->symbol = NO_OFFSET;
                parent->left_child = leftNode;
            }
            leftNode = parent->left_child;
            addSymbol(leftNode, current_offset, symbol, nodeDepth + 1);
        }
    }
}

long removeParents(MTF_NODE *currNode, long sum)
{
    int leafFlag = 0;
    // If we reach a node that doesn't have children
    if (currNode->left_child == NULL && currNode->right_child == NULL)
    {
        currNode->left_child = recycled_node_list;
        recycled_node_list = currNode;
        leafFlag = 1;
    }
    MTF_NODE* parent = currNode->parent;
    // Reach the head of mtf_node
    if (parent == NULL)
    {
        return sum;
    }
    // Else keep ascending and accumulating the value of sum
    else
    {
        if (parent->left_child == currNode)
        {
            if(leafFlag == 1)
            {
                parent->left_child = NULL;
            }
            parent->left_count -= 1;
            sum += parent->right_count;
        }
        else if(parent->right_child == currNode)
        {
            if(leafFlag == 1)
            {
                parent->right_child = NULL;
            }
            parent->right_count -= 1;
        }
        return removeParents(parent, sum);
    }
}
long removeSymbol(MTF_NODE *currNode, long offset, int nodeDepth)
{
    int LR = offset >> (currentDepth - nodeDepth - 1) & 1;
    if(nodeDepth >= currentDepth)
    {
        return removeParents(currNode, 0);
    }
    if (LR == 1)
    {
        return removeSymbol(currNode->right_child, offset, nodeDepth + 1);
    }
    else
    {
        return removeSymbol(currNode->left_child, offset, nodeDepth + 1);
    }
    
}

SYMBOL removeSymbolDecode(MTF_NODE *currNode, long rank, int nodeDepth, int max)
{
    
    if(nodeDepth >= currentDepth)
    {
        SYMBOL sym = currNode->symbol;
        removeParents(currNode, 0);
        return sym;
    }
    if (rank <= max)
    {
        return removeSymbolDecode(currNode->right_child, rank, nodeDepth + 1, max - currNode->right_child->left_count);
    }
    else
    {
        return removeSymbolDecode(currNode->left_child, rank, nodeDepth + 1, max + currNode->left_child->right_count);
    }
    
}

void newRoot()
{
    MTF_NODE *node = getNode();
    
    node->left_child = mtf_map;
    if (current_offset == 1)
    {
        node->left_count = node->left_child->left_count + node->left_child->right_count + 1;
    }
    else
    {
        node->left_count = node->left_child->left_count + node->left_child->right_count;
    }
    node->right_child = NULL;
    node->right_count = 0;
    node->parent = NULL;
    node->symbol = NO_OFFSET;
    mtf_map->parent = node;
    mtf_map = node;
}

/**
 * @brief  Given a symbol value, determine its encoding (i.e. its current rank).
 * @details  Given a symbol value, determine its encoding (i.e. its current rank)
 * according to the "move-to-front" transform described in the assignment
 * handout, and update the state of the "move-to-front" data structures
 * appropriately.
 *
 * @param sym  An integer value, which is assumed to be in the range
 * [0, 255], [0, 65535], depending on whether the
 * value of the BYTES field in the global_options variable is 1 or 2.
 *
 * @return  An integer value in the range [0, 511] or [0, 131071]
 * (depending on the value of the BYTES field in the global_options variable),
 * which is the rank currently assigned to the argument symbol.
 * The first time a symbol is encountered, it is assigned
 * a default rank computed by adding 512 or 131072 to its value.
 * A symbol that has already been encountered is assigned a rank in the
 * range [0, 255], [0, 65535], according to how recently it has occurred
 * in the input compared to other symbols that have already been seen.
 * For example, if this function is called twice in succession
 * with the same value for the sym parameter, then the second time the
 * function is called the value returned will be 0 because the symbol will
 * have been "moved to the front" on the previous call.
 *
 * @modifies  The state of the "move-to-front" data structures.
 */
CODE mtf_map_encode(SYMBOL sym) {
    num_Symbol = global_options & 0x0000000F;
    // Case for invalid symbols
    if (num_Symbol == 1 && (sym > 255 || sym < 0))
    {
        return -1;
    }
    if (num_Symbol == 2 && (sym > 65535 || sym < 0))
    {
        return -1;
    }
    OFFSET *offsetArr = last_offset;
    long offset = *(offsetArr + sym);

    // Encountering symbol for the first time
    if (offset == NO_OFFSET)
    {
        *(offsetArr + sym) = current_offset;
        
        if(mtf_map == NULL){
            mtf_map = node_pool;
            
            mtf_map->left_child = NULL;
            mtf_map->left_count = 0;
            mtf_map->right_child = NULL;
            mtf_map->right_count = 0;
            mtf_map->parent = NULL;
            mtf_map->symbol = sym;

            first_unused_node_index += 1;
            currentDepth = 0;
        
        }
        else if (logB2(current_offset) > currentDepth)
        {
            currentDepth += 1;
            newRoot();
            addSymbol(mtf_map, current_offset, sym, 1);

        }
        else if(mtf_map != NULL)
        {
            addSymbol(mtf_map,current_offset, sym, 1);
        }
        current_offset += 1;
        if(num_Symbol == 1)
        {
            
            return sym + 256;
        }
        else
        {
            return sym + 65536;
        }
    }
    // Exists in the tree 
    else 
    {
        // Case where consecutive first two characters 
        if (mtf_map && mtf_map->symbol == sym )
        {
            
            return 1;
        }
        else
        {
            if (logB2(current_offset) > currentDepth)
            {
                currentDepth += 1;
                newRoot();
            }
            // if (currentDepth > 1)
            // {
                 long rank = removeSymbol(mtf_map, offset, 0);
                addSymbol(mtf_map, current_offset, sym, 1);
                *(offsetArr + sym) = current_offset;
                current_offset += 1;
                //printf("normal\n");
                //printTree(mtf_map,0);
                return rank;
            // }
        }

        // if (logB2(current_offset) > currentDepth)
        // {
        //     currentDepth += 1;
        //     newRoot();
        // }
        // long rank = removeSymbol(mtf_map, offset, 0);
        // addSymbol(mtf_map, current_offset, sym, 1);
        // *(offsetArr + sym) = current_offset;
        // current_offset += 1;
        // printTree(mtf_map,0);
        // return rank;
    }
    
    
    // // Case for valid symbols
    // if (num_Symbol == 1)
    // {
    //     sym += 256;
    // }
    // else 
    // {
    //     sym += 65536;
    // }
    // // Pointer to the rank array
    // int* rank = symbol_rank;
    // // Empty rank array case
    // if (*(rank) == 0)
    // {
    //     *(rank) = sym;
    //     return sym;
    // }

    // // Check if it already exists in array 
    // for (int i = 0; i < (sizeof(symbol_rank)/sizeof(CODE)); i++)
    // { 
    //     // If the symbol exists in the ranks return the index 
    //     if (*(rank + i) == sym)
    //     {
    //         // Need to move the rank back to the front
    //         for (int x = i; x > 0; x--) 
    //         {
    //             *(rank + x) = *(rank + x - 1);
    //         }
    //         // Place symbol at front 
    //         *(rank) = sym;
    //         return i;
    //     }
    // }
    
    // // If the symbol passes the check then we know that the symbol hasn't been encountered yet
    // // Shift the whole array over one space 
    // for(int j = (sizeof(symbol_rank)/sizeof(CODE)); j > 0; j--)
    // {
    //     *(rank + j) = *(rank + j - 1);
    // }
    // *(rank) = sym;
    // return sym;
    
    // // TO BE IMPLEMENTED.
    // return NO_SYMBOL;
}

/**
 * @brief Given an integer code, return the symbol currently having that code.
 * @details  Given an integer code, interpret the code as a rank, find the symbol
 * currently having that rank according to the "move-to-front" transform
 * described in the assignment handout, and update the state of the
 * "move-to-front" data structures appropriately.
 *
 * @param code  An integer value, which is assumed to be in the range
 * [0, 511] or [0, 131071], depending on the value of the BYTES field in
 * the global_options variable.
 *
 * @return  An integer value in the range [0, 255] or [0, 65535]
 * (depending on value of the BYTES field in the global_options variable),
 * which is the symbol having the specified argument value as its current rank.
 * Argument values in the upper halves of the respective input ranges will be
 * regarded as the default ranks of symbols that have not yet been encountered,
 * and the corresponding symbol value will be determined by subtracting 256 or
 * 65536, respectively.  Argument values in the lower halves of the respective
 * input ranges will be regarded as the current ranks of symbols that
 * have already been seen, and the corresponding symbol value will be
 * determined in accordance with the move-to-front transform.
 *
 * @modifies  The state of the "move-to-front" data structures.
 */
SYMBOL mtf_map_decode(CODE code) {
    num_Symbol = global_options & 0x0000000F;
    // Case for invalid symbols
    if (num_Symbol == 1 && (code > 511 || code < 0))
    {
        return -1;
    }
    if (num_Symbol == 2 && (code > 131071 || code < 0))
    {
        return -1;
    }
    // SYMBOL *rank = symbol_rank;
    if (num_Symbol == 1)
    {
        code -= 256;
    }
    else 
    {
        code -= 65536;
    }
    // Code exists
    if (code < 0)
    {
        if(num_Symbol == 1)
        {
            code += 256;
        }
        else
        {
            code += 65536;
        }
        if (mtf_map->symbol != -1)
        {
            
            return mtf_map->symbol;
        }
        else 
        {
            if (logB2(current_offset) > currentDepth)
            {
                currentDepth += 1;
                newRoot();
            }
            int max = mtf_map->right_count;
            SYMBOL sym = removeSymbolDecode(mtf_map, code+1, 0, max);
            addSymbol(mtf_map, current_offset, sym, 1);
            current_offset += 1;
            //printTree(mtf_map,0);
            return sym;
        }
    }
    else
    {
        if(mtf_map == NULL)
        {
            mtf_map = node_pool;
            
            mtf_map->left_child = NULL;
            mtf_map->left_count = 0;
            mtf_map->right_child = NULL;
            mtf_map->right_count = 0;
            mtf_map->parent = NULL;
            mtf_map->symbol = code;

            first_unused_node_index += 1;
            currentDepth = 0;
        }
        else if(logB2(current_offset) > currentDepth)
        {
            currentDepth += 1;
            newRoot();
            addSymbol(mtf_map, current_offset, code, 1);
        }
        else if(mtf_map != NULL)
        {
            addSymbol(mtf_map, current_offset, code, 1);
        }
        current_offset += 1;
        //printTree(mtf_map,0);
        return code; 
    }
    
    // // If empty set the first index to the code 
    // if (*(rank) == 0)
    // {
    //     *(rank) = code;
    //     return code;
    // }
    // else 
    // {
    //     // If code less than 0 then we know it exists in the array
    //     if (code < 0)
    //     {
    //         if(num_Symbol == 1)
    //         {
    //             code += 256;
    //         }
    //         else
    //         {
    //             code += 65536;
    //         }
    //         // Checking the index to see 
    //         SYMBOL symNum = *(rank + code);
    //         // Shift the elements to the right
    //         for(int i = code; i > 0; i--)
    //         {
    //             *(rank + i) = *(rank + i - 1);
    //         }
    //         // Put the value back at the front of the array
    //         *(rank) = symNum;
    //         return symNum;
    //     }
    //     else
    //     {
    //         for(int i = (sizeof(symbol_rank)/sizeof(SYMBOL)) ; i > 0; i--)
    //         {
    //             *(rank + i) = *(rank + i - 1);
    //         }
    //         *(rank) = code;
    //         return code;
    //     }
    // }

    // // TO BE IMPLEMENTED.
    // return NO_SYMBOL;
}

// returning the counter 
int getFib(int index)
{
    if (index <= 1)
    {
        return index;
    }
    return getFib(index - 1) + getFib(index-2);
}

long expTwo(long base, int exp)
{
    if (exp == 0)
    {
        return 1;
    }
    while (exp > 1)
    {
        base *= 2;
        exp -= 1;
    }
    return base;
}


long zecondorfAlgorithm(CODE code)
{
    outputSize = 0;
    int temp = 0;
    // Increment code by 1 since zecondorf only encodes positive 
    code += 1;
    int cpy = code;
    long output =  0x0000000000000000; 
    int start;
    if (num_Symbol == 1)
    {
        start = 14;
    }
    else 
    {
        start = 26;
    }
    int flag = 0;
    // Create the encoded sequence of numbers using zecondorf's algorithm 
    //  starting from the number closest to code 
    for (int i = start; i >= 2 ; i--)
    {
        int fibNum = getFib(i);
        // Get the closest fibNum to the current value of code 
        if(fibNum <= code)
        {
            output = output | 0x1000000000000000;
            if(flag == 0)
            {
                flag = 1;
            }
            code -= fibNum;
            if (code != 0)
            {
                output = output >> 1;
            }
        } 
        else 
        { 
            output = output >> 1;
            
        }
        if (flag == 1)
        {
            outputSize += 1;
        }
    }
    // Get rid of all the 0's to the right of the encoded sequence
    output = output >> (61-outputSize);
    // Set the consecutive 1 
    output = (output << 1) | 0x1;
    outputSize += 1;
    return output;
}

/**
 * @brief  Perform data compression.
 * @details  Read uncompressed data from stdin and write the corresponding
 * compressed data to stdout, using the "move-to-front" transform and
 * Fibonacci coding, as described in the assignment handout.
 *
 * Data is read byte-by-byte from stdin, and each group of one or two
 * bytes is interpreted as a single "symbol" (according to whether
 * the BYTES field of the global_options variable is 1 or 2).
 * Multi-byte symbols are constructed according to "big-endian" byte order:
 * the first byte read is used as the most-significant byte of the symbol
 * and the last byte becomes the least-significant byte.  The "move-to-front"
 * transform is used to map each symbol read to its current rank.
 * As described in the assignment handout, the range of possible ranks is
 * twice the size of the input alphabet.  For example, 1-byte input symbols
 * have values in the range [0, 255] and their ranks have values in the
 * range [0, 511].  Ranks in the lower range are used for symbols that have
 * already been encountered in the input, and ranks in the upper range
 * serve as default ranks for symbols that have not yet been seen.
 *
 * Once a symbol has been mapped to a rank r, Fibonacci coding is applied
 * to the value r+1 (which will therefore always be a positive integer)
 * to obtain a corresponding code word (conceptually, a sequence of bits).
 * The successive code words obtained as each symbol is read are concatenated
 * and the resulting bit string is blocked into 8-bit bytes (according to
 * "big-endian" bit order).  These 8-bit bytes are written successively to
 * stdout.
 *
 * When the end of input is reached, any partial output byte is "padded"
 * with 0 bits in the least-signficant positions to reach a full 8-bit
 * byte, which is emitted as the final byte in the output.
 *
 * Note that the transformation performed by this function is to be done
 * in an "online" fashion: output is produced incrementally as input is read,
 * without having to read the entire input first.
 * Note also that this function does *not* attempt to "close" its input
 * or output streams.
 *
 * @return 0 if the operation completes without error, -1 otherwise.
 */
int mtf_encode() {
    SYMBOL input;
    // Get the number of symbols to be read
    num_Symbol = global_options & 0x0000000F;
    CODE output;
    // Variable for printing bits and appending zecondorf sequences
    unsigned long concat = 0;
    // Varialbe used to get the 1's from the zecondorf sequence
    unsigned long result = 0;
    // Variable used to store zecondorf sequence
    unsigned long outAlgo = 0;
    // Varialbe used to obtain 8 or 16 bits from concat to print
    unsigned long print = 0;
    // Length of concat
    int sequenceLength = 0; 
    // Beginning of part 4 
    // Initialzie all values in last offset arr to -1    
    OFFSET *offsetArr = last_offset;

    for (int i = 0; i < 65536; i++)
    {
        *(offsetArr + i) = NO_OFFSET;
    }
    SYMBOL secondSym;
    while ((input = getchar()) != EOF)
    {
        // Case for single symbols
        if (num_Symbol == 2)
        {
            secondSym = getchar();
            if(secondSym == EOF)
            {
                return -1;
            }
            input = (input << 8) | secondSym;
        }
        output = mtf_map_encode(input);
        // Get the encoded number in zecondorf form
        outAlgo = zecondorfAlgorithm(output);

        // Increment the length of the overall sequence
        sequenceLength += outputSize;

        for (int i = 0; i < outputSize; i++)
        {
            // Obtain the 1's from the encoded number 
            result = (outAlgo >> (outputSize - i - 1) & 0x1);
            // Concatenate it to the overall sequence of words
            concat = concat << 1; 
            concat = concat | result;
        }
        // Putchar the output if it has a size larger than 8
        while (sequenceLength >= 8)
        {
            // Take the first 8 bits of concat 
            print = concat >> (sequenceLength - 8);
            // Putchar the character
            putchar(print);
            // Remove the 8 bits we just read by shifting concat all the way to the left 
            concat = concat << (63 - sequenceLength + 8);
            concat = concat << 1;
            // Shift over one bit as to make the msb a 0 or 1 
            concat = concat >> 1;
            // Make the msb 0 from 0 or 1 
            concat &= 0x7FFFFFFFFFFFFFFF;
            // Restore the state of concat without the first 8 bits 
            concat = concat >> (63 - sequenceLength + 8);
            print = 0;
            sequenceLength -= 8;
        }    
    }

    // Once we leave the loop we might have characters left to read 
    if (sequenceLength >= 1)
    {
        concat = concat << (8 - sequenceLength);
        putchar(concat);
        return 0;
    }
    return -1;
}
CODE deZecondorfAlgorithm(long input, int inputSize)
{
    CODE sum = -1;
    int index = 2;
    for( int i = inputSize-1; i >= 0; i--)
    {
        int inputCpy = (input >> i) & 1;
        if(inputCpy == 1)
        {
            sum += getFib(index);
        }
        index += 1;
    }
    return sum;
}
/**
 * @brief  Perform data decompression, inverting the transformation performed
 * by mtf_encode().
 * @details Read compressed data from stdin and write the corresponding
 * uncompressed data to stdout, inverting the transformation performed by
 * mtf_encode().
 *
 * Data is read byte-by-byte from stdin and is parsed into individual
 * Fibonacci code words, using the fact that two consecutive '1' bits can
 * occur only at the end of a code word.  The terminating '1' bits are
 * discarded and the remaining bits are interpreted as describing the set
 * of terms in the Zeckendorf sum representing a positive integer.
 * The value of the sum is computed, and one is subtracted from it to
 * recover a rank.  Ranks in the upper half of the range of possible values
 * are interpreted as the default ranks of symbols that have not yet been
 * seen, and ranks in the lower half of the range are interpreted as ranks
 * of symbols that have previously been seen.  Using this interpretation,
 * together with the ranking information maintained by the "move-to-front"
 * heuristic, the rank is decoded to obtain a symbol value.  Each symbol
 * value is output as a sequence of one or two bytes (using "big-endian" byte
 * order), according to the value of the BYTES field in the global_options
 * variable.
 *
 * Any 0 bits that occur as padding after the last code word are discarded
 * and do not contribute to the output.
 * 
 * Note that (as for mtf_encode()) the transformation performed by this
 * function is to be done in an "online" fashion: the entire input need not
 * (and should not) be read before output is produced.
 * Note also that this function does *not* attempt to "close" its input
 * or output streams.
 *
 * @return 0 if the operation completes without error, -1 otherwise.
 */
int mtf_decode() {
    num_Symbol = global_options & 0x0000000F;
    int input;
    int sequenceLength = 0;
    unsigned long concat = 0;
    SYMBOL *rank = symbol_rank;

    while ((input = getchar()) != EOF)
    {
        concat = (concat << 8) | input;
        sequenceLength += 8;
        int prev = (concat >> (sequenceLength - 1)) & 1;
        int curr = 0;
        
        for( int i = sequenceLength - 2; i >= 0; i--)
        {
            curr = (concat >> i) & 1;
            int flag = 0;
            if(curr == 1 && prev == 1)
            {
                // Copy of concat and everything including and after the consecutive 1
                unsigned long leftAlign = concat >> (i + 1);
                // Size of the zecondorf array
                int inputSize = sequenceLength - i - 1;
                CODE output = deZecondorfAlgorithm(leftAlign, inputSize);
                SYMBOL print = mtf_map_decode(output);
                if (num_Symbol == 2)
                {
                    int first = print >> 8;
                    int second = print << (32-8);
                    second = second >> (32-8);
                    putchar(first);
                    if (second == EOF)
                    {
                        return -1;
                    }
                    putchar(second);
                }
                else
                {
                    putchar(print);
                }

                sequenceLength = sequenceLength - inputSize - 1;
                concat = concat << (63 - i);
                concat = concat << 1;
                concat = concat >> (64 - i);
                flag = 1;
            }
            if(flag == 1)
            {
                prev = 0;
                flag = 1;
            }
            else 
            {
                prev = curr;
            }       
        }
    }
    return 0;
}

/**
 * @brief String comparator
 * @details This function will compare two strings to see if they're equal
 * 
 * @param str1 The first string
 * @param str2 The second string
 * @return 0 if the two strings are equal and a a non-zero if the two 
 * strings arn't equaivalent
 */
int strCmp(char* str1, char* str2)
{
    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

/**
 * @brief String to int converter
 * @details This function will convert a string to an int while getting rid of leading zeros
 * 
 * @param str The string to convert
 * @return the int version of str
 */
int strToInt(char* str)
{
    int leadingZeros = 0;
    int convert = 0;

    for(int i = 0; *(str + i) != '\0'; i++)
    {
        // Need to get number after leading zeros
        if(*(str + i) != '0' || leadingZeros > 0)
        {
            leadingZeros = 1;
            convert = convert * 10 + *(str +i) - '0';
        }
    }
    return convert;
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the various options that were specified will be
 * encoded in the global variable 'global_options', where it will be
 * accessible elsewhere in the program.  For details of the required
 * encoding, see the assignment handout.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * @modifies global variable "global_options" to contain an encoded representation
 * of the selected program options.
 */

int validargs(int argc, char **argv) {
    int encodingFlag = 0;
    int decodingFlag = 0;
    int byteFlag = 0;
    for (int idx = 1 ; idx < argc; idx++)
    {
        char * flag = *(argv + idx);
        // Looking for the H flag 
        if (strCmp(flag, "-h") == 0)
        {
            global_options = HELP_OPTION;
            return 0;
        }
        // Case for encoding
        else if (strCmp(flag, "-e") == 0)
        {
            // Cannot have decode and encode 
            // Cannot have encoding flag after a byte flag 
            if (encodingFlag > 0 || decodingFlag > 0 || byteFlag > 0)
            {
                return -1;
            }
            else 
            {
                global_options = ENCODE_OPTION;
                encodingFlag = 1;
            }
        }
        // Case for decoding
        else if (strCmp(flag, "-d") == 0)
        {
            // Cannot have encode and decode
            // Cannot have decoding flag after byte flag
            if (decodingFlag > 0 || encodingFlag > 0 || byteFlag > 0)
            {
                return -1;
            }
            else 
            {
                global_options = DECODE_OPTION;
                decodingFlag = 1;
            }
        }
        // Handling the byte flag
        else if (strCmp(flag, "-b") == 0)
        {
            // Need to have encoding or decoding flag
            // Byte flag can't be raised
            if ((encodingFlag > 0 || decodingFlag > 0) && byteFlag == 0)
            {
                // Check the for null after the symbol size to 
                //  make sure there isn't extra flags or arguments 
                char* nullFlag = *(argv + idx + 2);
                if (nullFlag != 0)
                {
                    return -1;
                }
                // Can check the value of the symbol size
                char* symbolSize = *(argv + idx + 1);
                // Convert string to int
                int converted = strToInt(symbolSize);
                if (converted == 1)
                {
                    global_options = global_options | 0x00000001;
                    byteFlag = 1;
                    return 0;
                }
                else if (converted == 2)
                {
                    global_options = global_options | 0x00000002;
                    byteFlag = 1;
                    return 0;
                }
                // Covers for invalid characters and input 
                else
                {
                    return -1;
                }
            }
        }
        // Case where anything other than -h, -e, -d, and -b is read
        else
        {
            return -1;
        }
    }
    
    if (encodingFlag == 0 && decodingFlag == 0 && byteFlag == 0)
    {
        return -1;

    }
    // case for default byte symbol size
    else if ((encodingFlag == 1 || decodingFlag == 1) && byteFlag == 0)
    {
        global_options = global_options | 0x00000001;
        return 0;
    }
    else
    {
        return -1;
    }
}
