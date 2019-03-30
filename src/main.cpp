#include <iostream>
#include <unistd.h>
#include "matchingEngine.h"
using namespace std;
using namespace Matching;

void usage()
{
    cout << "Matching Engine\n" << endl;
    cout << "Usage: matching [-i inputFile]\n" << endl;
    cout << "Options: " << endl;
    cout << "  -i, input file order.csv path. If not specify, default to ../data/orders.csv" << endl;
    cout << endl;
}

int main()
{
	/*
    string infile = "../data/orders.csv";
    int opt;
    while ((opt = getopt(argc, argv, "i:")) != -1) {
        switch(opt) {
        case 'i':
            infile = optarg;
            break;
        default:
            usage ();
            return -1;
        }
    }
    */

    Matching::MatchingEngine engine;
    //return engine.run( infile );
    string n1 = "Poonam";
    Order* b1 = new Order(700001,n1,73.21,100,100001,true);
    Order* b2 = new Order(700002,n1,800,100,100002,false);
    Order* b3 = new Order(700003,"Srajan",900,1000,100002,true);
    Order* b4 = new Order(700004,"Pooja",74,200,100003,false);
    Order* b5 = new Order(700009,"Tree",800,133,20390,false);
    Order* b6 = new Order(700009,"Tree1",120,133,20290,false);
    cout << *b1 << endl;
    cout << *b2 << endl;
    OrderBook* ob = new OrderBook();
    ob->add(b1);
    ob->add(b2);
    ob->add(b3);
    ob->add(b4);
    ob->add(b5);
    ob->add(b6);
    cout << *ob << endl;
}
