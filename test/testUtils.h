/*test utility function */
#ifndef TEST_TESTUTILS_H
#define TEST_TESTUTILS_H

#include "../src/orderbook.h"
using namespace std;

namespace Matching
{
	/*Test bid in descending order
	Test ask in ascending order
	*/
	inline bool priceTreeEquals(PriceTree* priceTree, vector<Order*>& orders) {
		//orders is taken by reference to remove cc invocation, no need for pointers while using the function though
		unsigned int num = 0;
		for(PriceTreeIt it = priceTree->begin(); it != priceTree->end(); ++it) {
			OrderTree* orderTree = it->second->getOrderTree();
			for(OrderTreeIt itOrder = orderTree->begin(); itOrder != orderTree->end(); ++itOrder) {
				if(num >= orders.size() || *orders[num] != *(*itOrder))
					return false;
				++num;
			}
		}
		return num == orders.size();
	}


	/*test if orderBook equals expected */
	inline bool orderBookEquals(OrderBook* book, vector<Order*> bids, vector<Order*> asks) {
		//comparing address only?
		return priceTreeEquals(book->getBidTree(), bids) &&
		priceTreeEquals(book->getAskTree(), asks);
	}
}
#endif