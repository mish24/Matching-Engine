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

int main(int argc, char** argv)
{
	
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
    Matching::MatchingEngine me;
    me.run(infile);
    OrderBook* orderBook = const_cast<OrderBook*>(me.getOrderBook());
    cout << *orderBook << endl;

    /*
    MatchingEngine me;
    string n1 = "Poonam", n2 = "Srajan", n3 = "Tom";
    me.init( {n1, n2, n3 } );

    Order* b1 = new Order( 70000001, n1, 200, 50, 100001, true );
    Order* b2 = new Order( 70000002, n2, 100, 100, 100002, true );
    Order* s1 = new Order( 70000003, n3, 50,  1500, 100003, false );
    me.processOrder( b1 );
    me.processOrder( b2 );
    me.processOrder( s1 );

    OrderBook* orderBook = const_cast< OrderBook* >( me.getOrderBook() );
    //b2->quantity -= 50;
    //OrderBook* ob1 = const_cast<OrderBook*>(engine.getOrderBook());
    cout<<*orderBook<<endl;
    
    //cout << ob1->getTraderExposure(n1) << endl;
    */

}
