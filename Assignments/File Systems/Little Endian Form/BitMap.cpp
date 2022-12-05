#include<iostream>
#include<fstream>
#include<vector>
#include<bitset>

typedef unsigned int uint;
typedef std::vector<uint> Vect_uint;
#define BitSize 32


class BitMap
{
    private:
        uint x;
        std::bitset<BitSize> BT;
        std::vector<Vect_uint> LittleEndian;
        Vect_uint FileSystem;
        int Skips;
        int BlockSize;

    public:

    BitMap()
    {
        x = rand();
        x = x % 1300000;
        x = x + 70000;

        Skips = 0;
        BlockSize = 4;
    }

    void load_Bitmap();
    void little_endian();
    void Skip_Counter();
    void printResults();
    int getSkips(){return Skips;}
    int getBlockSize(){return BlockSize;}
    int getDecimal(){return x;}
};


int main()
{
    double RT = 0;
    double average = 0.0;


    /*
        Since we are doing 10 experiments, we will loop 10 times and at each loop
        execution, we create a temp struct of our Bit Map schematic which should
        initialize it with a random number in the desired range. Then we call the
        necessary methods to build the Bit Map and count the skips and then write
        the result to the outfile destination. 
    */


    for (int i = 0; i < 10; i++)
    {
        BitMap temp;
        
        temp.load_Bitmap();
        temp.little_endian();
        temp.Skip_Counter();
        temp.printResults();
        RT += temp.getSkips();
    }

    average = RT / 10;

    std::cout << "The average number of skips for this experiment is: " << average << "\n";
}


void BitMap::load_Bitmap()
{
    std::bitset<BitSize> BT(x);


    // Since BT holds the binary string backwards, this puts it in correct order
    for (int i = 0; i < BT.size(); i++)
    {
        FileSystem.push_back(BT[i]);
    }


    /* 
        Splitting the binary string into 4 strings to be used for the little endian format.
        The for loop takes the current index and uses some math to make sure that it will
        only copy over 8 bits at a time. Values are hard coded as we know we are working
        with 32 bits.

        The actual values that are copied are put in a temp vector of unsigned ints and
        those are then pushed to a matrix which is a vector of vector of unsigned ints.
    */

    for (int k = 0; k < 4; k++)
    {
        Vect_uint temp;

        for (int l = (k * 32) / 4; l < (k + 1) * (32 / 4); l++)
        {
            temp.push_back(FileSystem[l]);
        }

        LittleEndian.push_back(temp);
    }
}



/*
    This method will tally up the binary values of the 4 substrings and determine what
    order they need to be "assembled" in to construct the little endian bit map string.
    The result of each string is calculated and placed in a temp vector Assembly. We 
    then loop through the results and search in order of highest to lowest and construct
    the Bit Map string called File_System.
*/

void BitMap::little_endian()
{
    Vect_uint Assembly;


    for (int i = 0; i < LittleEndian.size(); i++)
    {
        uint RT = 0;
        uint BPT = 128;

        for (int j = 0; j < LittleEndian[i].size(); j++)
        {
            uint temp = LittleEndian[i][j];
            RT += (temp * BPT);
            BPT = BPT / 2;
        }

        Assembly.push_back(RT);
    }

    FileSystem.clear();

    for (int k = 0; k < Assembly.size(); k++)
    {
        uint highest = 0;
        int index = 0;

        for (int l = 0; l < Assembly.size(); l++)
        {
            if (Assembly[l] > highest)
            {
                highest = Assembly[l];
                index = l;
            }
        }

        Assembly[index] = 0;

        for (int m = 0; m < LittleEndian[index].size(); m++)
        {
            FileSystem.push_back(LittleEndian[index][m]);
        }
    }
}



/*
    With our File System constructed in proper Little Endian form, we just need to
    loop through the results and determine if the empty blocks have enough room to
    write our file or if we need to skip. A skip counter keeps track of this only
    counting the free blocks that aren't big enough for our file as a skip. Once
    we have a contiguous block big enough for our file, we break.
*/

void BitMap::Skip_Counter()
{
    int consec_zero_count = 0;
    bool Free_Space = false;

    for (int i = 0; i < FileSystem.size(); i++)
    {
        if (FileSystem[i] == 1 && Free_Space == true)
        {
            Free_Space = false;
            consec_zero_count = 0;
            Skips++;
        }

        if (FileSystem[i] == 0)
        {
            Free_Space = true;
            consec_zero_count++;
        }

        if(consec_zero_count >= BlockSize)
        {
            break;
        }
    }
}



void BitMap::printResults()
{
    std::cout << "Decimal Number: " << x << "\n";
    std::cout << "Little Endian Bit Map: ";

    for (int z = 0; z < FileSystem.size(); z++)
    {
        std::cout << FileSystem[z] << " ";
    }

    std::cout << "\n";

    std::cout << "Skips: " << Skips << "\n\n";
}
