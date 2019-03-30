/*Matching Engine */
#ifndef MATCHING_ENGINE_H
#define MATCHING_ENGINE_H
#include "orderbook.h"

namespace Matching {
	#define BUYSTR "BUY"
	#define TRADER "Poonam"
	#define NCOL 6


	class MatchingEngine {
	private:
		OrderBook* orderBook;
	public:
		MatchingEngine();
		virtual ~MatchingEngine() { clean(); }
		const OrderBook* getOrderBook() const { return orderBook; }

		void init(const vector<string>& names);
		void clean() { delete orderBook;}
		int run(const string& inFile);
		void processOrder(Order* order);
	};
}

#endif