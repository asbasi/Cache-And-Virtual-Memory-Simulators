#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

#define RAM_SIZE 65536
#define CACHE_SIZE 512
#define BYTES_PER_LINE 8

int main(int argc, char* argv[])
{
    unsigned char ram[RAM_SIZE] = {0};
    unsigned char cache[CACHE_SIZE] = {0};
    unsigned short stored[CACHE_SIZE / BYTES_PER_LINE] = {0};
    int dirtyBit[CACHE_SIZE / BYTES_PER_LINE] = {0};

    unsigned short address;     // Address being accessed.
    unsigned short operation;  // Operation being performed.
    unsigned short data;       // Data being used.

    unsigned int tag;
    unsigned int lineNum;
    unsigned int offset;

    unsigned short ramAddress;

    int hit;
    int dirty;

    if(argc != 2)
    {
        cout << "usage: " << argv[0] << " <input_file>" << endl;
    }
    else
    {
        ifstream inFile(argv[1]);       // The input file.
        ofstream outFile("dm-out.txt"); // The output file.

        if(inFile.is_open())
        {
            while(!inFile.eof() && inFile >> hex >> address >> operation >> data)
            {
                hit = 0;
                dirty = 0;

                // Parse the address.
                tag = address >> 9;
                lineNum = (address >> 3) & 0x3F;
                offset = address & 0x7;

                if((stored[lineNum] >> 9) == tag) // Hit.
                {
                    hit = 1;
                    dirty = dirtyBit[lineNum];
                }
                else // Miss.
                {
                    // Current line is dirty so we need to write it back to ram.
                    if(dirtyBit[lineNum] == 1)
                    {
                        ramAddress = stored[lineNum] >> 3;
                        for(int i = 0; i < 8; i++)
                        {
                            ram[(ramAddress * 8) + i] = cache[(lineNum * 8) + i];
                        }
                        dirty = 1;
                        dirtyBit[lineNum] = 0;
                    }

                    // Read the necessary line from the ram into cache.
                    ramAddress = address >> 3;
                    for(int i = 0; i < 8; i++)
                    {
                        cache[(lineNum * 8) + i] = ram[(ramAddress * 8) + i];
                    }
                    stored[lineNum] = address;
                }

                // Write operation.
                if(operation == 0xFF)
                {
                    cache[(lineNum * 8) + 7 - offset] = data;
                    dirtyBit[lineNum] = 1;
                }
                // Read Operation.
                else
                {
                    outFile << hex << uppercase << setfill('0') << setw(4) << address << " ";
                    for(int i = 0; i < 8; i++)
                    {
                        outFile << setw(2) << (unsigned short)cache[(lineNum * 8)+ i];
                    }
                    outFile << " " << hit << " " << dirty << endl;
                }
            }
            inFile.close();
            outFile.close();
        }

    }
    return 0;
}
