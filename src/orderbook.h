/*Order book design */
/* OrderBook will be a binary sorted tree, and the nodes will
be bid or ask, they'll have a price and a set of Order pointer and its comparator (which
is mostly the same for all nodes). Each node will belong to a OrderTree (which is a set) */


#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <map>
#include <unordered_map>
#include <set>
#include <limits>
#include <vector>
#include <iostream>
#include "order.h"
using namespace std;

namespace Matching {
	#define INAN std::numeric_limits<int>::min()
	#define IS_VALID( x ) ( x != INAN )

	class PriceNode;

	/* Comparator - if size is bigger, return true
	in case of equal sizes, lesser timestamp one wins */
	struct OrderSizeTimeComparator {
		bool operator()(const Order* a, const Order* b) const {
			int qtya = a->quantity, qtyb = b->quantity;
			if(qtya != qtyb) return qtya > qtyb;
			else return a->time < b->time;
		}
	};

	//some typedefs for iterators and pointers
	typedef PriceNode* PriceNodePtr;
	typedef map<int, PriceNodePtr> PriceTree;
	//PriceTree will comprise of the price, and the respective pointer
	//or order corresponding to that price
	typedef map<int, PriceNodePtr>::iterator PriceTreeIt;
	typedef map<int,PriceNodePtr>::reverse_iterator PriceTreeRevIt;
	typedef unordered_map<int, PriceNodePtr> PriceToNodeMap;
	//a price to Order hash table like structure for all orders
	typedef unordered_map<int, PriceNodePtr>::iterator PriceToNodeMapIt;
	typedef set<Order*,OrderSizeTimeComparator> OrderTree;
	typedef set<Order*, OrderSizeTimeComparator>::iterator OrderTreeIt;
	typedef unordered_map<string, int> AccountMap;
	typedef unordered_map<string, int>::iterator AccountMapIt;

	/*each node will be a BUY or SELL entry in our orderbook. 
	This goes in a map(a self balancing BST) - the trees are separated into
	buy and sell */
	class PriceNode {
	private:
		int price;
		set<Order*, OrderSizeTimeComparator>* orderTree;
		/*pricenode has a price and its tree in a set, whose
		//value cant be modified once it is added, and there can be only
		//one Order corresponding to that PriceNode
		//a priceNode can have multiple nodes since at same price other things
		//can vary. all those are stored in the orderTree, to which there is a
		pointer in the code.*/
	public:
		PriceNode() : price(INAN), orderTree(NULL) {}
		PriceNode(int price_) : price(price_) { orderTree = new OrderTree(); }
		void clean(); //implemented later
		virtual ~PriceNode() { clean(); }

		//utility functions
		int getPrice() const { return price; } //dont modify the price
		//getOrderTree() gets a reference to a pointer since it may alter its value
		//this may happen if this is called for a priceNode which is being initialised
		//here, a new OrderTree will be made as per the constructor
		//only works when PriceNode is not accessed directly. Used in OrderBook where the PriceNode is accessed by an iterator. 
		OrderTree*& getOrderTree() { return orderTree; } //returns a reference to the pointer of the OrderTree
		//OrderTree* getOrderTreeTest() { return orderTree; }
		void insertOrder(Order* order) { orderTree->emplace(order);}

		//operator overloading
		friend ostream& operator<<(ostream& os, const PriceNode& priceNode);

	};
	/* end of PriceNode class */

	//friend function
	inline ostream& operator<<(ostream& os, const PriceNode& priceNode) {
		os<<"[ " << priceNode.getPrice() << ", <";
		for( auto& node : *priceNode.orderTree)
			os << *node << ",";
		os << "> ]";
		return os;
	}


	/*
	Order book
	*/
	class OrderBook {
	private:
		/*
		binary sorted tree (map) for indexing bid and ask
		within order book
		/* Advantages : add order - logN,
		Modify : logN to 2logN (remove and add again)
		Remove : logN
		Trade : 2logN ~ 2 0(N) if all orders are of the same price, map will
		degrade to a list
		*/
		map<int, PriceNodePtr>* bidTree; //<price, LimitPriceNode>
		map<int, PriceNodePtr>* askTree;

		/*for booking a trade
		add and remove complexity is O(N) */
		unordered_map<string, int>* account;
		//hashmap to speed up add() in order book to O(1) if the price already exists
		unordered_map<int, PriceNodePtr>* bidMap;
		unordered_map<int, PriceNodePtr>* askMap;

	public:
		OrderBook();
		virtual ~OrderBook();

		void add(Order* order);
		void match(const Order* order, int& qtyToMatch);
		void match(OrderTreeIt it, const Order* order, int& qtyToMatch, OrderTree* quotes, int bestPrice);

		PriceTree*& getBidTree() { return bidTree;}
		PriceTree*& getAskTree() { return askTree;}

		PriceToNodeMap*& getAskMap() { return askMap;}
		PriceToNodeMap*& getBidMap() { return bidMap;}

		/*marketable orders remove liquidity, the bid must be above the current ask
		or the asks must be below the current bid.
		*/
		bool isMarketable(const Order* order, int bestPrice, bool isBuy);

		void bookTrade(int execQty, const string& buyer, const string& seller);
		void bookTradeForTrader(const vector<string>& names);
		int getTraderExposure(const string& name);

		friend ostream& operator<<(ostream& os, const OrderBook& book);
	};
	/* end of orderBook class */

	/*operator overload for OrderBook printing*/
	inline ostream& operator<<(ostream& os, const OrderBook& book) {
		os << "\n------Ask------\n";
		for(PriceTreeRevIt it = book.askTree->rbegin(); it != book.askTree->rend(); ++it) {
			os << "Price: " << it->first << " [ ";
			for(const auto& order: *(it->second->getOrderTree()))
				os << *order << " ";
			os << " ]" << endl;
		}

		os << "\n------Bid------\n";
		for(PriceTreeRevIt it = book.bidTree->rbegin(); it!= book.bidTree->rend(); ++it) {
			os << "Price: " << it->first << " [ ";
			for( const auto& order: *(it->second->getOrderTree()))
				os << *order << " ";
			os << " ]" << endl;
		}
		return os;
	}

	/* PriceNode */
	inline void PriceNode::clean(){
		for(OrderTreeIt it = orderTree->begin(); it != orderTree->end(); ++it) {
			delete *it;
		}
		delete orderTree;
	}

	/* OrderBook */
	inline OrderBook::OrderBook() {
		bidTree = new PriceTree();
		askTree = new PriceTree();
		bidMap = new PriceToNodeMap();
		askMap = new PriceToNodeMap();
		account = new AccountMap();
	}

	inline OrderBook::~OrderBook() {
		for(PriceTreeIt it = bidTree->begin(); it != bidTree->end(); ++it)
			delete it->second;
		for(PriceTreeIt it = askTree->begin(); it != askTree->end(); ++it)
			delete it->second;

		delete bidTree;
		delete askTree;
		delete bidMap;
		delete askMap;
		delete account;
	}

	inline bool OrderBook::isMarketable(const Order* order, int bestPrice, bool isBuy) {
		if(isBuy)
			return order->price >= bestPrice;
		else
			return order->price <= bestPrice;
	}

	/*Marketable order handling:
	Remove liquidity to the other side of the book and order time: O(1) */
	inline void OrderBook::match(const Order* order, int& qtyToMatch) {
		bool isBuy = order->isBuy;
		//get the opposite side of the tree to match
		if(isBuy) {
			//check the askTree
			for(PriceTreeIt itBestPrice = askTree->begin();
				itBestPrice != askTree->end() && isMarketable(order,askTree->begin()->first,order->isBuy) &&
				qtyToMatch > 0; )
			{
				int bestPrice = itBestPrice->first;
				PriceNode* bestPriceNode = itBestPrice->second;
				OrderTree* quotes = bestPriceNode->getOrderTree();
				//for each order (in size>time) in this price level
				match(quotes->begin(), order, qtyToMatch, quotes, bestPrice);
				//order depletes current price level
				if(quotes->empty()) {
					delete bestPriceNode;
					//bestPriceNode = NULL;
					//deals with the nodes in the trees only for that price level. when the quantity is changed
					//for qtyToMatch, this is updated in the processOrder function, not here.
					askTree->erase(itBestPrice++);
					askMap->erase(bestPrice);
				}
				else
					++itBestPrice;
			}
		}
		else {
			for(PriceTreeRevIt itBestPrice = bidTree->rbegin();
				itBestPrice != bidTree->rend() && 
				isMarketable(order,bidTree->rbegin()->first,order->isBuy) &&
				qtyToMatch > 0; ) {
				int bestPrice = itBestPrice->first;
				PriceNode* bestPriceNode = itBestPrice->second;
				OrderTree* quotes = bestPriceNode->getOrderTree();

				//for each order for(size>time) in this price level
				match(quotes->begin(),order,qtyToMatch,quotes,bestPrice);

				//order depeltes current price level
				if(quotes->empty()) {
					delete bestPriceNode;
					bidTree->erase(std::next(itBestPrice).base()); //erase in reverse iterator
					bidMap->erase(bestPrice);
				}
				else {
					++itBestPrice;
				}
			}
		}
	}

		/*the overloaded match function which is called for each
		order of the price level. it satisfies full or a part of the requiremtn
		for buy/sell. the quantity of shares is also reduced here. the results 
		go up to match 1 and deletion or it moves to the next BestPrice
		*/

		inline void OrderBook::match(OrderTreeIt it, const Order* order, int& qtyToMatch, OrderTree* quotes, int bestPrice) {
			//it is the OrderTree which has many orders of the same price
			bool isBuy = order->isBuy;

			while(it != quotes->end() &&
				order != NULL &&
				isMarketable(order,bestPrice,isBuy) &&
				qtyToMatch > 0) {
				Order* quote = *it;
				//we're matching this quote, so remove this from the quotes (opposite)
				quotes->erase(it++);

				int curQty = quote->quantity;
				int execQty = min(curQty,qtyToMatch); //it is possible that the ask volume is greater than the one desired
				const string& buyer = isBuy ? order->name : quote->name;
				const string& seller = isBuy ? quote->name : order->name;
				bookTrade(execQty,buyer,seller);
				qtyToMatch -= execQty;

				//add residual back to order tree
				if(curQty > execQty) {
					quote->quantity = curQty - execQty;
					quotes->emplace(quote);
				}
				else
				{
					delete quote;
				}

			}

			if(qtyToMatch == 0) {
				delete order;
			}
		}

	



		/*Non marketable order handling :
		Add liquidity to the same side of the book and order
		time : O(1) if level exists, else O(logM) - M = avg number of quotes
		*/
		inline void OrderBook::add(Order* order) {
			bool isBuy = order->isBuy;
			int price = order->price;
			PriceTree* priceTree = isBuy ? bidTree : askTree;
			PriceToNodeMap* priceToNodeMap = isBuy ? bidMap : askMap;

			PriceToNodeMapIt it = priceToNodeMap->find(price);
			//for searching, the unordered_map helps, if the price exists
			//we find the PriceNode associated with that price and add this new
			//order to the OrderTree
			if(it != priceToNodeMap->end()) {
				OrderTree* quotes = it->second->getOrderTree();
				quotes->emplace(order);
			}
			else {
				PriceNode* priceNode = new PriceNode(price);
				priceNode->insertOrder(order);
				priceTree->emplace(price,priceNode);
				priceToNodeMap->emplace(price, priceNode);
			}
		}

		inline void OrderBook::bookTrade(int qty, const string& buyer, const string& seller) {
			AccountMapIt it = account->find(buyer);
			if(it != account->end())
				it->second += qty;
			it = account->find(seller);
			if(it != account->end())
				it->second -= qty;
		}

		inline void OrderBook::bookTradeForTrader(const vector<string>& names) {
			for(string name : names) {
				account->emplace(name,0);
			}
		}

		inline int OrderBook::getTraderExposure(const string& name) {
			AccountMapIt it = account->find(name);
			if(it != account->end())
				return it->second;
			else return 0;
		}

}

#endif