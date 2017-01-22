#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

#define RAM_SIZE 65536
#define CACHE_SIZE 512
#define BYTES_PER_LINE 8

unsigned int LRU(unsigned int setNumber, unsigned int* counter);

int main(int argc, char* argv[])
{
    unsigned char ram[RAM_SIZE] = {0};
    unsigned char cache[CACHE_SIZE] = {0};
    unsigned short stored[CACHE_SIZE / BYTES_PER_LINE] = {0};
    unsigned int counter[CACHE_SIZE / BYTES_PER_LINE] = {0};
    int dirtyBit[CACHE_SIZE / BYTES_PER_LINE] = {0};

    unsigned short address;     // Address being accessed.
    unsigned short operation;   // Operation being performed.
    unsigned short data;        // Data being used.

    unsigned int tag;
    unsigned int setNum;
    unsigned int lineNum;
    unsigned int offset;

    unsigned short ramAddress;

    int hit;
    int dirty;

    bool found;

    if(argc != 2)
    {
        cout << "usage: " << argv[0] << " <input_file>" << endl;
    }
    else
    {
        ifstream inFile(argv[1]);       // The input file.
        ofstream outFile("sa-out.txt"); // The output file.

        if(inFile.is_open())
        {
            while(!inFile.eof() && inFile >> hex >> address >> operation >> data)
            {
                hit = 0;
                dirty = 0;
                found = false;
                lineNum = 0;

                // Parse the address.
                tag = address >> 7;
                setNum = (address >> 3) & 0xF;
                offset = address & 0x7;

                while(!found && lineNum < 4)
                {
                    if((stored[(setNum * 4) + lineNum] >> 7) == tag)
                    {
                        found = true;
                    }
                    else
                    {
                        lineNum++;
                    }
                }


                if(found) // Hit.
                {
                    hit = 1;
                    dirty = dirtyBit[(setNum * 4) + lineNum];
                }
                else // Miss.
                {
                    lineNum = LRU(setNum, counter);

                    // Current line is dirty so we need to write it back to ram.
                    if(dirtyBit[(setNum * 4) + lineNum] == 1)
                    {
                        ramAddress = stored[(setNum * 4) + lineNum] >> 3;
                        for(int i = 0; i < 8; i++)
                        {
                            ram[(ramAddress * 8) + i] = cache[(setNum * 32) + (lineNum * 8) + i];
                        }
                        dirty = 1;
                        dirtyBit[(setNum * 4 ) + lineNum] = 0;
                    }

                    // Read the necessary line from the ram into cache.
                    ramAddress = address >> 3;
                    for(int i = 0; i < 8; i++)
                    {
                        cache[(setNum * 32) + (lineNum * 8) + i] = ram[(ramAddress * 8) + i];
                    }
                    stored[(setNum * 4) + lineNum] = address;
                }

                // Write operation.
                if(operation == 0xFF)
                {
                    cache[(setNum * 32) + (lineNum * 8) + 7 - offset] = data;
                    dirtyBit[(setNum * 4) + lineNum] = 1;
                }
                // Read Operation.
                else
                {
                    outFile << hex << uppercase << setfill('0') << setw(4) << address << " ";
                    for(int i = 0; i < 8; i++)
                    {
                        outFile << setw(2) << (unsigned short)cache[(setNum * 32) + (lineNum * 8) + i];
                    }
                    outFile << " " << hit << " " << dirty << endl;
                }

                // Zero out the counter for the line we're using.
                counter[(setNum * 4) + lineNum] = 0;

                // Increment counter for the other lines.
                for(int i = 0; i < 4; i++)
                {
                    if(i != lineNum)
                        counter[(setNum * 4) + i] += 1;
                }
            }
            inFile.close();
            outFile.close();
        }

    }
    return 0;
}

unsigned int LRU(unsigned int setNumber, unsigned int* counter)
{
    unsigned int lru = 0;
    unsigned int max = counter[(setNumber * 4)];
    for(int i = 1; i < 4; i++)
    {
        if(counter[(setNumber * 4) + i] > max)
        {
            max = counter[(setNumber * 4) + i];
            lru = i;
        }
    }
    return lru;
}
