/*UNIT TESTS FOR THE CLASSES USING BOOST */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestMatch
#include <boost/test/unit_test.hpp>
#include "../src/matchingEngine.h"
#include "testUtils.h"
using namespace std;

/* Tests covered :
1. Add
2. Same/different tree
3. Ordering priceTree orderTree
4. Match
5. No match
6. Deplete a portion of the quote
7. Deplete multiple quote. Post and no post
8. Deplete one price level
9. Deplete multiple price level
10. Deplete entire tree
*/

BOOST_AUTO_TEST_SUITE( Matching )

BOOST_AUTO_TEST_CASE(TestNoMatch) {
	MatchingEngine me;
	string n1 = "Tree", n2 = "Plant", n3 = "Rabbit";
	me.init({n1,n2,n3});
	Order* b1 = new Order(1,n1,100,100,1,true);
	Order* b2 = new Order(2,n2,100,200,2,true);
	Order* s1 = new Order(3,n3,200,300,3,false);
	me.processOrder(b1);
	me.processOrder(b2);
	me.processOrder(s1);

	OrderBook* orderBook = const_cast<OrderBook*>(me.getOrderBook());
	BOOST_CHECK(orderBookEquals(orderBook, {b2,b1},{s1}));
	BOOST_CHECK_EQUAL(orderBook->getTraderExposure(n1),0);
	BOOST_CHECK_EQUAL(orderBook->getTraderExposure(n2),0);
	BOOST_CHECK_EQUAL(orderBook->getTraderExposure(n3),0);

}

BOOST_AUTO_TEST_CASE(TestOneQuotePartialMatch) {
	MatchingEngine me;
	string n1 = "Tree", n2 = "Plant", n3 = "Rabbit";
	me.init({n1,n2,n3});
	Order* b1 = new Order(1,n1,100,100,1,true);
	Order* b2 = new Order(2,n2,100,200,2,true);
	Order* s1 = new Order(3,n3,80,50,3,false);
	me.processOrder(b1);
	me.processOrder(b2);
	me.processOrder(s1);
	OrderBook* orderBook = const_cast<OrderBook*>(me.getOrderBook());
	BOOST_CHECK(orderBookEquals(orderBook,{b2,b1},{}));
	BOOST_CHECK_EQUAL(orderBook->getTraderExposure(n1),0);
	BOOST_CHECK_EQUAL(orderBook->getTraderExposure(n2),50);
	BOOST_CHECK_EQUAL(orderBook->getTraderExposure(n3),-50);
}

BOOST_AUTO_TEST_CASE(TestOneQuoteFullMatch) {
	MatchingEngine me;
	string n1 = "Tree", n2 = "Plant", n3 = "Rabbit";
	me.init({n1,n2,n3});
	Order* b1 = new Order(1,n1,100,100,1,true);
	Order* b2 = new Order(2,n2,100,200,2,true);
	Order* s1 = new Order(3,n3,80,200,3,false);
	me.processOrder(b1);
	me.processOrder(b2);
	me.processOrder(s1);
	OrderBook* orderBook = const_cast<OrderBook*>(me.getOrderBook());
	BOOST_CHECK(orderBookEquals(orderBook, {b1},{}));
	BOOST_CHECK_EQUAL(orderBook->getTraderExposure(n1),0);
	BOOST_CHECK_EQUAL(orderBook->getTraderExposure(n2),200);
	BOOST_CHECK_EQUAL(orderBook->getTraderExposure(n3),-200);
}

BOOST_AUTO_TEST_CASE(TestMultipleQuoteFullMatch) {
	MatchingEngine me;
	string n1 = "Tree", n2 = "Plant", n3 = "Rabbit", n4 = "Husky", n5 = "Lab";
	me.init({n1,n2,n3,n4,n5});
	Order* b1 = new Order(1,n1,100,100,1,true);
	Order* b2 = new Order(2,n2,100,200,2,true);
	Order* b3 = new Order(2,n3,100,300,3,true);
	Order* s1 = new Order(3,n4,80,500,4,false);
	Order* s2 = new Order(3,n5,500,500,4,false);
	me.processOrder( b1 );
    me.processOrder( b2 );
    me.processOrder( b3 );
    me.processOrder( s1 );
    me.processOrder( s2 );
    OrderBook* orderBook = const_cast< OrderBook* >( me.getOrderBook() );
    BOOST_CHECK( orderBookEquals( orderBook, { b1 }, { s2 } ) );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n1 ), 0 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n2 ), 200 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n3 ), 300 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n4 ), -500 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n5 ), 0 );
}

BOOST_AUTO_TEST_CASE(TestMultipleQuoteOutsideMatchWithResidual) {
	MatchingEngine me;
    string n1 = "Mal", n2 = "Kaylee", n3 = "Tom", n4 = "Kate", n5 = "Rob";
    me.init( {n1, n2, n3, n4, n5 } );

    Order* b1 = new Order( 70000001, n1, 7311, 100, 100001, true );
    Order* b2 = new Order( 70000002, n2, 7321, 200, 100002, true );
    Order* b3 = new Order( 70000003, n3, 7322, 300, 100003, true );
    Order* s1 = new Order( 70000004, n4, 7441, 200, 100004, false );
    Order* s2 = new Order( 70000005, n5, 7320, 600, 100005, false );
    me.processOrder( b1 );
    me.processOrder( b2 );
    me.processOrder( b3 );
    me.processOrder( s1 );
    me.processOrder( s2 );

    OrderBook* orderBook = const_cast< OrderBook* >( me.getOrderBook() );
    BOOST_CHECK( orderBookEquals( orderBook, { b1 }, { s2, s1 } ) );

    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n1 ), 0 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n2 ), 200 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n3 ), 300 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n4 ), 0 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n5 ), -500 );
}

BOOST_AUTO_TEST_CASE( TestOnePriceLevelMatch )
{
    MatchingEngine me;
    string n1 = "Mal", n2 = "Kaylee", n3 = "Tom", n4 = "Kate", n5 = "Rob";
    me.init( {n1, n2, n3, n4, n5 } );

    Order* b1 = new Order( 70000001, n1, 7311, 100, 100001, true );
    Order* b2 = new Order( 70000002, n2, 7322, 200, 100002, true );
    Order* b3 = new Order( 70000003, n3, 7322, 300, 100003, true );
    Order* s1 = new Order( 70000004, n4, 7441, 200, 100004, false );
    Order* s2 = new Order( 70000005, n5, 7320, 500, 100005, false );
    me.processOrder( b1 );
    me.processOrder( b2 );
    me.processOrder( b3 );
    me.processOrder( s1 );
    me.processOrder( s2 );

    OrderBook* orderBook = const_cast< OrderBook* >( me.getOrderBook() );
    cout << *orderBook << endl;
    BOOST_CHECK( orderBookEquals( orderBook, { b1 }, { s1 } ) );

    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n1 ), 0 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n2 ), 200 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n3 ), 300 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n4 ), 0 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n5 ), -500 );
}

BOOST_AUTO_TEST_CASE( TestMultiplePriceLevelMatch )
{
    MatchingEngine me;
    string n1 = "Mal", n2 = "Kaylee", n3 = "Tom", n4 = "Kate", n5 = "Rob", n6 = "Bill";
    me.init( {n1, n2, n3, n4, n5, n6 } );

    Order* b1 = new Order( 70000001, n1, 7311, 100, 100001, true );
    Order* b2 = new Order( 70000002, n2, 7322, 200, 100002, true );
    Order* b3 = new Order( 70000003, n3, 7322, 300, 100003, true );
    Order* b4 = new Order( 70000004, n4, 7323, 400, 100004, true );
    Order* s1 = new Order( 70000005, n5, 7441, 200, 100005, false );
    Order* s2 = new Order( 70000006, n6, 7320, 900, 100006, false );
    me.processOrder( b1 );
    me.processOrder( b2 );
    me.processOrder( b3 );
    me.processOrder( b4 );
    me.processOrder( s1 );
    me.processOrder( s2 );

    OrderBook* orderBook = const_cast< OrderBook* >( me.getOrderBook() );
    BOOST_CHECK( orderBookEquals( orderBook, { b1 }, { s1 } ) );

    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n1 ), 0 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n2 ), 200 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n3 ), 300 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n4 ), 400 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n5 ), 0 );
    BOOST_CHECK_EQUAL( orderBook->getTraderExposure( n6 ), -900 );
}

BOOST_AUTO_TEST_CASE(TestEntireTreeMatch) {
	MatchingEngine me;
	string n1 = "Mal", n2 = "Kaylee", n3 = "Tom", n4 = "Kate", n5 = "Rob", n6 = "Bill";
    me.init( {n1, n2, n3, n4, n5, n6 } );
    Order* b1 = new Order(1,n1,200,100,1,true);
    Order* b2 = new Order(2,n2,400,100,2,true);
    Order* b3 = new Order(3,n3,400,300,3,true);
    Order* b4 = new Order(4,n4,400,200,4,true);
    Order* s1 = new Order(5,n5,400,600,5,false);
    Order* s2 = new Order(6,n6,200,200,6,false);
    me.processOrder(b1);
    me.processOrder(b2);
    me.processOrder(b3);
    me.processOrder(b4);
    me.processOrder(s1);
    me.processOrder(s2);
    OrderBook* orderBook = const_cast< OrderBook* >( me.getOrderBook() );
    cout << *orderBook << endl;
    BOOST_CHECK( orderBookEquals( orderBook, {}, {s2} ) );
}

BOOST_AUTO_TEST_SUITE_END()