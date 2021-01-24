#include <iostream>

#include "Core.h"
#include "TrtEngine.h"
#include "SSDRes18Engine.h"

using namespace std;

void PrintHelp()
{
    cout << "SkiingRobot 0.2" << endl;
    cout << "Usage: SkiingRobot <engine_file>" << endl;
    cout << "    engine_file: path to model TensorRT engine file" <<endl;
    cout << endl;
}

int PrintBadArguments()
{
    cout << "Bad arguments!" << endl;
    PrintHelp();
    return 1;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        return PrintBadArguments();
    }

    // Demo usage
    SSDRes18Engine(argv[1], 224, 224);
    //TODO: implment YoloEngine
}
