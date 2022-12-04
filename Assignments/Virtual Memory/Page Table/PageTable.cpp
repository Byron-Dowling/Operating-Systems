/*
    Name:       Byron Dowling
    Course:     5143 Advanced Operating Systems
    Date:       10/11/2022

    Assignment: FIFO, LRU, and Second Chance Page Replacement Algorithms


        Example Run with actual results:

            Running FIFO with line size: 11
            Operation completed in: 428 milliseconds
            Cycles: 1136700
            Page Faults: 4607

            Running FIFO with line size: 12
            Operation completed in: 436 milliseconds
            Cycles: 4719500
            Page Faults: 16840

            Running LRU with line size: 11
            Operation completed in: 579 milliseconds
            Cycles: 666800
            Page Faults: 3208

            Running LRU with line size: 12
            Operation completed in: 431 milliseconds
            Cycles: 2659700
            Page Faults: 11317

            Running Second Chance with line size: 11
            Operation completed in: 559 milliseconds
            Cycles: 613500
            Page Faults: 4205

            Running Second Chance with line size: 12
            Operation completed in: 550 milliseconds
            Cycles: 4332400
            Page Faults: 24799
*/

#include<iostream>
#include<fstream>
#include<memory>
#include<chrono>
#include<vector>
#include<deque>
#include<cmath>
#include<map>


// Typedefs for easier shorter syntax
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long long int ulli;


/*
    Struct Object to hold our data in a convienent way

        - Page_Num is derived from doing a >> of page size of the Hex_Address

        - Command is used to reference if a write command is read

        - Write is a bool status that can be updated should a read command be
          read while the page is in the table. This is where the +500 hit occurs

*/

struct PageEntry
{
    ushort Command;
    int Page_Num;
    bool write;
    bool referenced;

    PageEntry() {
        write = referenced = false;
        }
};


/*
    Class: Page Table

    Since we will be running multiple instances of the experiment, it makes sense
    to use a class to handle an instance of the Page Table experiment. 

    This class will implement both the LRU and FIFO page replacement algorithms
    and will accept various page/line sizes. The class utilizes two separate maps
    to prevent unnecessary traversals of the deque container that is the actual 
    page table. The page entry's are held in the deque while the maps are
    updated and used to query the page number to see if it's in the table and to
    fetch the entire Page Entry object in case we need to update the write status.

        - Cycles are counted for page faults and overhead for evicting pages with 
          write commands.

        - Smart pointers are used here, specifically shared pointers, so that memory
          is cleaned up automatically when the pointer goes out of scope as well 
          as allowing for multiple pointers to temporarily point to the same object.

        - Two separate insert functions are defined for each algorithm with a generic
          insert method used to call the appropriate one based on what the replacement
          algorithm is.
*/

class PageTable
{
    private:

        std::map<int, bool> inTable;
        std::map<int, bool> pageLookup;     // Write
        std::map<int, bool> pageReference;  // Reference
        std::deque<int> pageTable;
        uint pageTableSize;
        uint pageFaults;
        ulli cycles;
        int currentSize;
        bool fullTable;
        bool FIFO;
        bool LRU;
        bool SecondChance;

    public:

        PageTable(uint pts, std::string alg) {
            pageTableSize = pts;
            currentSize = 0;
            pageFaults = 0;
            fullTable = false;
            cycles =  0;
            
            if (alg == "FIFO") {
                FIFO = true;
                LRU = false;
            }

            else if (alg == "LRU") {
                FIFO = false;
                LRU = true;
                SecondChance = false;
            }

            else 
            {
                FIFO = false;
                LRU = false;
                SecondChance = true;
            }
        }

        void insert(std::shared_ptr<PageEntry> Page);
        void insertFIFO(std::shared_ptr<PageEntry> Page);
        void insertLRU(std::shared_ptr<PageEntry> Page);
        void insertSecondChance(std::shared_ptr<PageEntry> Page);
        ulli getCycles() {return cycles;}
        ulli getFaults() {return pageFaults;}
        void visualizeTable();
};


int main()
{
    std::string prAlgorithms[] = {"FIFO", "LRU", "Second Chance"};   // Page Replacement Algorithms
    int lineSizes[] = {11, 12};                                      // Page Size of 4096 <=> 2^12 or 2048 <=> 2^11
    uint mmSize = pow(2,15);                                         // 32 KB Physical Memory

    std::ofstream Fout;
    Fout.open("Results.txt");

    /*
        The user is prompted for how many runs they would like to do for the experiment.

        One run = FIFO: Size 12 line size,       FIFO: Size 11 line size, 
                  LRU: Size 12 line size,        LRU: Size 11 line size
                  2nd Chance: Size 12 line size, 2nd Chance: Size 11 line size
                  
                  Time in milliseonds for each individual result.
    */

    
    for (auto algorithm : prAlgorithms)
    {
        for (auto lineSize : lineSizes)
        {
            std::ifstream Fin;
            Fin.open("spice.txt");

            uint Hex_Address;
            uint PageTableSize = mmSize/pow(2,lineSize);

            std::unique_ptr<PageTable> PT(new PageTable(PageTableSize, algorithm));

            // Start timer
            auto start = std::chrono::high_resolution_clock::now();
        
            while (!Fin.eof())
            {
                // Smart pointer object for our page entry
                // Should run faster this way
                std::shared_ptr<PageEntry> P1 (new PageEntry());

                Fin >> P1->Command;

                if (P1->Command == 1) {
                    P1->write = true;
                }

                Fin >> std::hex >> Hex_Address;
                P1->Page_Num = (Hex_Address>>lineSize);

                // Insert Page into the table after deriving its data
                PT->insert(P1);
            }

            // Stop timer
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast< std::chrono::milliseconds >(stop - start);

            // Visualize results
            std::cout << "\nRunning " << algorithm << " with line size: " << lineSize << "\n";
            std::cout << "Operation completed in: " << duration.count() << " milliseconds\n";
            std::cout << "Cycles: " << PT->getCycles() << "\n";
            std::cout << "Page Faults: " << PT->getFaults() << "\n";

            Fout << "Running " << algorithm << " with line size: " << lineSize << "\n";
            Fout << "Operation completed in: " << duration.count() << " milliseconds\n";
            Fout << "Cycles: " << PT->getCycles() << "\n\n";

            Fin.close();

            PT->visualizeTable();
        }
    }

    Fout.close();
}


// Sanity Check to see what the page table looks like
void PageTable::visualizeTable() {
    std::cout << "\nVisualizing Table Results:\n";
    for (auto item : this->pageTable) {
        std::cout << item << "<->";
    }

    std:: cout <<"NULL\n\n";
}



/*
    Purpose is to forward the page to the correct insert function
    based on if the current replacement algorithm being used is
    FIFO or LRU.

    This is set on the constructor when the current algorithm is
    passed in and the bool variables for the sorts are updated.
*/ 
void PageTable::insert(std::shared_ptr<PageEntry> Page)
{
    if(FIFO) {
        insertFIFO(Page);
    }

    else if (LRU) {
        insertLRU(Page);
    }

    else {
        insertSecondChance(Page);
    }
}


/*
    
    ██╗     ███████╗ █████╗ ███████╗████████╗                        
    ██║     ██╔════╝██╔══██╗██╔════╝╚══██╔══╝                        
    ██║     █████╗  ███████║███████╗   ██║                           
    ██║     ██╔══╝  ██╔══██║╚════██║   ██║                           
    ███████╗███████╗██║  ██║███████║   ██║                           
    ╚══════╝╚══════╝╚═╝  ╚═╝╚══════╝   ╚═╝                           
                                                                    
    ██████╗ ███████╗ ██████╗███████╗███╗   ██╗████████╗██╗  ██╗   ██╗
    ██╔══██╗██╔════╝██╔════╝██╔════╝████╗  ██║╚══██╔══╝██║  ╚██╗ ██╔╝
    ██████╔╝█████╗  ██║     █████╗  ██╔██╗ ██║   ██║   ██║   ╚████╔╝ 
    ██╔══██╗██╔══╝  ██║     ██╔══╝  ██║╚██╗██║   ██║   ██║    ╚██╔╝  
    ██║  ██║███████╗╚██████╗███████╗██║ ╚████║   ██║   ███████╗██║   
    ╚═╝  ╚═╝╚══════╝ ╚═════╝╚══════╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝╚═╝   
                                                                    
    ██╗   ██╗███████╗███████╗██████╗                                 
    ██║   ██║██╔════╝██╔════╝██╔══██╗                                
    ██║   ██║███████╗█████╗  ██║  ██║                                
    ██║   ██║╚════██║██╔══╝  ██║  ██║                                
    ╚██████╔╝███████║███████╗██████╔╝                                
    ╚═════╝ ╚══════╝╚══════╝╚═════╝                                 


    A deque (double ended queue) is used as the primary container for the
    page table. pages are sorted initially in a FIFO-style order. However
    if we get a hit on a page, we go to the page index, and we copy the page
    and erase the index. Then we re-add the index to the back of the queue.

    For example:

        [1]<->[2]<->[3]<->[4]<->[5]

            *hit on page 3

        [1]<->[2]<->[]<->[4]<->[5]

            * removed and resized

        [1]<->[2]<->[4]<->[5]

            * moving page 3 to the back

        [1]<->[2]<->[4]<->[5]<->[3]


    When it is time to evict a page, we run a pop front command since the
    oldest page/the least recently used page should remain at the front of
    the container. 
*/

void PageTable::insertLRU(std::shared_ptr<PageEntry> Page)
{
    // If the table is not full
    if (!fullTable)
    {
        // If the table isn't full and the page is not in our table
        // This would be a page fault and is +100 cycles
        if (!inTable[Page->Page_Num])
        {
            inTable[Page->Page_Num] = true;
            pageLookup[Page->Page_Num] = Page->write;
            currentSize++;
            cycles += 100;
            pageFaults++;

            pageTable.push_back(Page->Page_Num);

            if (currentSize == pageTableSize) {
                fullTable = true;
            }
        }

        // Hit on the page, need to shuffle order, but not increment capacity
        else
        {
            auto it = pageTable.begin();

            // Loop till we find the page and then erase it
            for (short i = 0; i < pageTable.size(); i++) {
                if (pageTable[i] == Page->Page_Num) {
                    pageTable.erase(it + i);
                    break;
                }
            }


            // Update write if necessary
            if (Page->write) {
                pageLookup[Page->Page_Num] = true;
            }

            // Add the page to the back of the queue
            pageTable.push_back(Page->Page_Num);
        }
    }

    // Table is full
    else
    {
        // Page isn't in table and we need to evict
        if (!inTable[Page->Page_Num])
        {
            auto P = pageTable.front();
            inTable[P] = false;

            // If write is true, add 500 cylces
            // in addition to 100 cycles for page fault
            if (pageLookup[P]) {
                cycles += 500;
            }

            pageTable.pop_front();

            pageTable.push_back(Page->Page_Num);
            pageLookup[Page->Page_Num] = Page->write;
            inTable[Page->Page_Num] = true;
            cycles += 100;
            pageFaults++;
        }

        // Page IS in table, need to shuffle
        else
        {
            auto P = pageLookup[Page->Page_Num];
            auto it = pageTable.begin();

            // Loop till we find the page and then erase it
            for (short i = 0; i < pageTable.size(); i++) {
                if (pageTable[i] == Page->Page_Num) {
                    pageTable.erase(it + i);
                    break;
                }
            }

            // Update write if necessary
            if (Page->write) {
                pageLookup[Page->Page_Num] = true;
            }

            // Add to the back of the queue
            pageTable.push_back(Page->Page_Num);
        }
    }
}


/*
    
    ███████╗██╗██████╗ ███████╗████████╗    ██╗███╗   ██╗              
    ██╔════╝██║██╔══██╗██╔════╝╚══██╔══╝    ██║████╗  ██║              
    █████╗  ██║██████╔╝███████╗   ██║       ██║██╔██╗ ██║              
    ██╔══╝  ██║██╔══██╗╚════██║   ██║       ██║██║╚██╗██║              
    ██║     ██║██║  ██║███████║   ██║       ██║██║ ╚████║              
    ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝   ╚═╝       ╚═╝╚═╝  ╚═══╝              
                                                                        
    ███████╗██╗██████╗ ███████╗████████╗     ██████╗ ██╗   ██╗████████╗
    ██╔════╝██║██╔══██╗██╔════╝╚══██╔══╝    ██╔═══██╗██║   ██║╚══██╔══╝
    █████╗  ██║██████╔╝███████╗   ██║       ██║   ██║██║   ██║   ██║   
    ██╔══╝  ██║██╔══██╗╚════██║   ██║       ██║   ██║██║   ██║   ██║   
    ██║     ██║██║  ██║███████║   ██║       ╚██████╔╝╚██████╔╝   ██║   
    ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝   ╚═╝        ╚═════╝  ╚═════╝    ╚═╝   

    Utilizing the features of the deque we simply add to the page table
    until it is full. When it becomes full, we pop front to remove the
    oldest element and push back to add the newest item to the back of
    the queue.

    Besides this change, the method largely works exactly the same as 
    LRU's insert method.

        For example:

        [1]<->[2]<->[3]<->[4]<->[5]

            *insert 6 but table is full*

        [1]<->[2]<->[3]<->[4]<->[5]
         ^                          ^   
      pop_front                 push_back 

            *New Table*
        
        [2]<->[3]<->[4]<->[5]<->[6]
*/

void PageTable::insertFIFO(std::shared_ptr<PageEntry> Page) 
{
    if (pageTable.empty())
    {
        currentSize++;
        pageTable.push_back(Page->Page_Num);
        cycles += 100;
        pageFaults++;
        pageLookup[Page->Page_Num] = Page->write;
        inTable[Page->Page_Num] = true;
    }

    else if (!fullTable)
    {
        if (!inTable[Page->Page_Num])
        {
            currentSize++;
            pageTable.push_back(Page->Page_Num);
            cycles += 100;
            pageFaults++;
            pageLookup[Page->Page_Num] = Page->write;
            inTable[Page->Page_Num] = true;
        }

        else
        {
            if (Page->write)
            {
                pageLookup[Page->Page_Num] = true;
            }
        }
    }

    else
    {
        if (inTable[Page->Page_Num])
        {
            if (Page->write)
            {
                pageLookup[Page->Page_Num] = true;
            }
        }

        else
        {
            auto P = pageTable.front();
            inTable[P] = false;

            if (pageLookup[P]) {
                cycles += 500;
            }

            pageTable.pop_front();

            pageTable.push_back(Page->Page_Num);
            pageLookup[Page->Page_Num] = Page->write;
            inTable[Page->Page_Num] = true;
            cycles += 100;
            pageFaults++;
        }
    }

    if (currentSize == pageTableSize){fullTable = true;}
}


/*

    ███████╗███████╗ ██████╗ ██████╗ ███╗   ██╗██████╗ 
    ██╔════╝██╔════╝██╔════╝██╔═══██╗████╗  ██║██╔══██╗
    ███████╗█████╗  ██║     ██║   ██║██╔██╗ ██║██║  ██║
    ╚════██║██╔══╝  ██║     ██║   ██║██║╚██╗██║██║  ██║
    ███████║███████╗╚██████╗╚██████╔╝██║ ╚████║██████╔╝
    ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═════╝ 
                                                        
     ██████╗██╗  ██╗ █████╗ ███╗   ██╗ ██████╗███████╗ 
    ██╔════╝██║  ██║██╔══██╗████╗  ██║██╔════╝██╔════╝ 
    ██║     ███████║███████║██╔██╗ ██║██║     █████╗   
    ██║     ██╔══██║██╔══██║██║╚██╗██║██║     ██╔══╝   
    ╚██████╗██║  ██║██║  ██║██║ ╚████║╚██████╗███████╗ 
     ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝╚══════╝ 

    Second chance runs F.I.F.O except that when there is a hit on the
    page it is considered referenced and a reference bit is toggled.

    When it is time to evict a page, if that page has its reference bit
    set to true, we flip it to false and then move on to the next candidate
    to be evicted.
*/

void PageTable::insertSecondChance(std::shared_ptr<PageEntry> Page)
{
    if (pageTable.empty())
    {
        currentSize++;
        pageTable.push_back(Page->Page_Num);
        cycles += 100;
        pageFaults++;
        pageLookup[Page->Page_Num] = Page->write;
        pageReference[Page->Page_Num] = false;
        inTable[Page->Page_Num] = true;
    }

    else if (!fullTable)
    {
        if (!inTable[Page->Page_Num])
        {
            currentSize++;
            pageTable.push_back(Page->Page_Num);
            cycles += 100;
            pageFaults++;
            pageLookup[Page->Page_Num] = Page->write;
            pageReference[Page->Page_Num] = false;
            inTable[Page->Page_Num] = true;
        }

        else
        {
            // Hit on page so we toggle the reference bit
            pageReference[Page->Page_Num] = true;

            if (Page->write) {
                pageLookup[Page->Page_Num] = true;
            }
        }
    }

    else
    {
        if (inTable[Page->Page_Num])
        {
            // Hit on page so we toggle the reference bit
            pageReference[Page->Page_Num] = true;

            if (Page->write) {
                pageLookup[Page->Page_Num] = true;
            }
        }

        else
        {
            // visualizeTable();
            auto it = pageTable.begin();
            bool evicted = false;

            for (short i = 0; i < pageTable.size(); i++) 
            {
                if (!pageReference[pageTable[i]]) 
                {
                    if (pageLookup[pageTable[i]]) {
                        cycles += 500;
                    }

                    // std::cout << "Evicting Page# " << pageTable[i] << "\n";
                    pageTable.erase(it + i);
                    inTable[pageTable[i]] = false;
                    evicted = true;
                    break;
                }

                else
                {
                    // std::cout << "\n" << pageTable[i] << " will get a second chance!\n";
                    pageLookup[pageTable[i]] = false;
                }
            }

            if (!evicted) 
            {
                // std::cout << "All elements had their references reset.\n";
                auto P = pageTable.front();
                inTable[P] = false;
    
                if (pageLookup[P]) {
                    cycles += 500;
                }
    
                pageTable.pop_front();
            }

            pageTable.push_back(Page->Page_Num);
            pageLookup[Page->Page_Num] = Page->write;
            pageReference[Page->Page_Num] = false;
            inTable[Page->Page_Num] = true;
            cycles += 100;
            pageFaults++;

            // std::cout << "\nNew Table:";
            // visualizeTable();
        }
    }

    if (currentSize == pageTableSize){fullTable = true;}
}
