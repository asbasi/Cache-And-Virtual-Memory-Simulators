#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

int main(int argc, char* argv[])
{
    unsigned int ram[4] = {0};
    unsigned int pageTable[16] = {0};
    bool full[4] = {0};
    bool useBit[4] = {0};
    unsigned int clock = 0; // Keeps track of which frame we're at.
    unsigned int data;
    unsigned int address;
    bool exists;
    bool fault;
    bool found;

    if(argc != 2)
    {
        cout << "usage: " << argv[0] << " <input_file>" << endl;
    }
    else
    {
        ifstream inFile(argv[1]);
        ofstream outFile("vm-out.txt");

        if(inFile.is_open())
        {
            // Read in the first 16 lines to page table.
            for(int i = 0; i < 16; i++)
            {
                inFile >> hex >> data;
                pageTable[i] = data;
            }

            // Start the actual page access.
            while(!inFile.eof() && inFile >> hex >> data)
            {
                fault = true;
                found = false;
                exists = false;

                // Zero out the last 12 bits.
                address = data & 0xfffff000;

                for(int i = 0; i < 4; i++)
                {
                    if(ram[i] == address)
                    {
                        useBit[i] = true;
                        fault = false;
                        full[i] = true;
                        break;
                    }
                }

                // It wasn't found in RAM.
                if(fault)
                {
                    // Check that the page actually exists in the
                    // page table.
                    for(int i = 0; i < 16; i++)
                    {
                        if(pageTable[i] == address)
                        {
                            exists = true;
                        }
                    }

                    if(exists)
                    {
                        while(!found)
                        {
                            // Looped through once, so reset clock.
                            if(clock == 4)
                            {
                                clock = 0;
                            }

                            if(useBit[clock] == false)
                            {
                                found = true;
                                ram[clock] = address;
                                full[clock] = true;
                                useBit[clock] = true;
                            }
                            else
                            {
                                useBit[clock] = false;
                            }
                            clock++;
                        }
                    }
                    else
                    {
                        cout << "The page \"" << hex << address
                             << "\" doesn't exist in page table "
                             << "so it will be ignored." << endl;
                    }
                }
                // Print out the virtual addressses of
                // the 4 pages currently in RAM.
                if(full[0])
                {
                    outFile << hex << ram[0];
                }
                if(full[1])
                {
                    outFile << " " << hex << ram[1];
                }
                if(full[2])
                {
                    outFile << " " << hex << ram[2];
                }
                if(full[3])
                {
                    outFile << " " << hex << ram[3];
                }
                outFile << endl;
            }
            inFile.close();
            outFile.close();
        }
    }
    return 0;
}
