/* Order.h - basic structure of a order to be placed */
#ifndef ORDER_H
#define ORDER_H
#include <string>
#include <iostream>
using namespace std;

//create a new namespace for Matching
namespace Matching {
	/*Order type is ID,NAME,PRICE,QUANTITY,TIME,BUY/SELL*/

	typedef struct Order {
		int id;
		int price;
		int quantity;
		int time; //just to ensure FIFO.
		bool isBuy;
		string name;

		//initialisation
		Order(int _id,string _name,int _price, int _quantity, int _time, bool _is_buy) :
		id(_id), name(_name), price(_price), quantity(_quantity),
		time(_time), isBuy(_is_buy) {}
	} Order;


	//operator overloading to print our typical Order
	inline ostream& operator<<(ostream& os, const Order& order) {
		os << order.name << "_" << order.price << "_" << order.quantity << "_" << order.time << "_" << order.isBuy;
		return os;
	}

	//comparing 2 orders by overloading ==
	inline bool operator==(const Order& lhs, const Order& rhs) {
		return lhs.id == rhs.id &&
		lhs.name == rhs.name &&
		lhs.price == rhs.price &&
		lhs.quantity == rhs.quantity &&
		lhs.time == rhs.time &&
		lhs.isBuy == rhs.isBuy;
	}

	//returning inequality - just check for == 
	inline bool operator!=(const Order& lhs, const Order& rhs) {
		return !(lhs == rhs);
	}
}

#endif /*ORDER_H*/