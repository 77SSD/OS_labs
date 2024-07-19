#include <iostream>
#include <bits/stdc++.h>
#include <cstdlib>
#include <time.h>
using namespace std;

int FIFO_page_faults = 0, LRU_page_faults = 0, Random_page_faults = 0;
int page_request;
deque<int> FIFOFrames;
deque<int> LRUFrames;
deque<int> RandomFrames;
deque<int> FIFOswapFrames;
deque<int> LRUswapFrames;
deque<int> RandomswapFrames;
void FIFO(int numFrames , int swapSize)
{
    deque<int>::iterator itr;
    itr = find(FIFOFrames.begin(), FIFOFrames.end(), page_request);
    if (itr == FIFOFrames.end())
    {
        FIFO_page_faults++;
        deque<int>::iterator itr1;
        if (FIFOFrames.size() == numFrames){
            //swapspacehandlestart
            int front_element = FIFOFrames.front();
            itr1 = find(FIFOswapFrames.begin(), FIFOswapFrames.end(), front_element);
            if (itr1 == FIFOswapFrames.end()){
                if (FIFOswapFrames.size() == swapSize){
                    FIFOswapFrames.pop_front();
                }
                FIFOswapFrames.push_back(front_element);
            }
            //swapspacehandleend
            FIFOFrames.pop_front();
        }
        FIFOFrames.push_back(page_request);
    }
}

void LRU(int numFrames , int swapSize)
{
    deque<int>::iterator itr;
    itr = find(LRUFrames.begin(), LRUFrames.end(), page_request);
    if (itr == LRUFrames.end())
    {
        LRU_page_faults++;
        deque<int>::iterator itr1;
        if (LRUFrames.size() == numFrames){
            //swapspacehandlestart
            int front_element = LRUFrames.front();
            itr1 = find(LRUswapFrames.begin(), LRUswapFrames.end(), front_element);
            if (itr1 == LRUswapFrames.end()){
                if (LRUswapFrames.size() == swapSize){
                    LRUswapFrames.pop_front();
                }
                LRUswapFrames.push_back(front_element);
            }
            //swapspacehandleend
            LRUFrames.pop_front();
        }
        LRUFrames.push_back(page_request);
    }
    else
    {
        LRUFrames.erase(itr);
        LRUFrames.push_back(page_request);
    }
}

void Random(int numFrames , int swapSize)
{
    deque<int>::iterator itr;
    itr = find(RandomFrames.begin(), RandomFrames.end(), page_request);
    if (itr == RandomFrames.end())
    {
        Random_page_faults++;
        if (RandomFrames.size() == numFrames)
        {
            srand(time(NULL));
            int random = 1 + (rand() % RandomFrames.size());
            itr = RandomFrames.begin() + random;
            //swapspacehandlestart
            deque<int>::iterator itr1;
            int front_element = RandomFrames[random] ;
            itr1 = find(RandomswapFrames.begin(), RandomswapFrames.end(), front_element);
            if (itr1 == RandomswapFrames.end()){
                if (RandomswapFrames.size() == swapSize){
                    RandomswapFrames.pop_front();
                }
                RandomswapFrames.push_back(front_element);
            }
            //swapspacehandleend
            RandomFrames.erase(itr);
            random =0;
        }
        RandomFrames.push_back(page_request);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        cout << "give input <num of pages> <number of frames> <number of disk blocks (swap space)> <input file>\n";
        return 1;
    }
    int numPages = atoi(argv[1]);
    int numFrames = atoi(argv[2]);
    int swapSize = atoi(argv[3]);
    if (swapSize < (numPages-numFrames))
    {
        cout << "Number of disk blocks should be greater than number of pages - number of frames atleast to store all data\n";
        return 0;
    }
    fstream file;
    string filename;
    filename = argv[4];
    file.open(filename.c_str());
    while (file >> page_request)
    {
        if (page_request > numPages){
            cout << "Requested page" << page_request << "is not valid" << endl;
            return 0;  
        }
        else{

            FIFO(numFrames , swapSize);
            LRU(numFrames , swapSize);
            Random(numFrames , swapSize);
        }
    }
    cout << "FIFO Page Faults : " << FIFO_page_faults << endl;
    cout << "LRU Page Faults : " << LRU_page_faults << endl;
    cout << "Random Page Faults : " << Random_page_faults << endl;
    return 0;
}
