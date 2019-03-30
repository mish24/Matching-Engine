/* Implementation of Matching Engine - 
The works of Matching Engine are : 
starting the engine by getting the order book
pricessing orders
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <cstring>
#include "matchingEngine.h"
#include "orderbook.h"

namespace Matching {
	MatchingEngine::MatchingEngine() {
		orderBook = new OrderBook();
		vector<string> names{TRADER};
		init(names);
	}

	void MatchingEngine::init(const vector<string>& names) {
		orderBook->bookTradeForTrader(names);
	}

	void MatchingEngine::processOrder(Order* order) {
		int qtyToMatch = order->quantity;
		orderBook->match(order,qtyToMatch);
		//post non marketable portion
		if(qtyToMatch > 0) {
			if(qtyToMatch != order->quantity)
				order->quantity = qtyToMatch;
			orderBook->add(order);
		}
	}

	int MatchingEngine::run(const string& inFile) {
		FILE* file = fopen(inFile.c_str(), "r");
		if(NULL == file) {
			fprintf(stderr, "Cannot open file at %s\n", inFile.c_str());
			return -1;
		}

		int id,quantity,time;
		char name[20],buySellStr[5];
		float price;
		while(!feof(file)) {
			int nItemsRead = fscanf(file,"%d,%[^,],%f,%d,%d,%s\n",
				&id,name,&price,&quantity,&time,buySellStr);
			if(NCOL != nItemsRead)
			{
				cout << "Bad line" << endl;
			}

			bool isBuy = strcmp(BUYSTR,buySellStr) == 0;
			int priceInt = price*100 + 0.5;
			Order* order = new Order(id,name,priceInt,quantity,time,isBuy);
			processOrder(order);
		}

		int exposure = orderBook->getTraderExposure(TRADER);
		string str = exposure >= 0 ? "L" : "S";
		cout << str << endl;
		cout << abs(exposure) << endl;
		return 0;
	}

}