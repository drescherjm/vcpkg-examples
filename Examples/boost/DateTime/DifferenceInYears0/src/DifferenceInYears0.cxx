#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/date_time/gregorian/gregorian.hpp>

int main()
{
	boost::gregorian::date d1( 2000, 1, 2 );
	boost::gregorian::date d2( 2001, 1, 1 );

	int _yearsCount = abs(d1.year() - d2.year());

	// I want to have d1 date earlier than d2
	if (d2 < d1) {
		boost::gregorian::date temp(d1);
		d1 = boost::gregorian::date(d2);
		d2 = temp;
	}

	// I assume now the d1 and d2 has the same year
	// (the later one), 2007-04-01 and 2007-03-1
	boost::gregorian::date d1_temp(d2.year(), d1.month(), d1.day());
	if (d2 < d1_temp)
		--_yearsCount;

	std::cout << _yearsCount;
	
// 	if (d1 > d2)
//     std::swap(d1, d2); // guarantee that d2 >= d1
// 
// 	boost::date_time::partial_date pd1(d1.day(), d1.month());
// 	boost::date_time::partial_date pd2(d2.day(), d2.month());
// 
// 	int fullYearsInBetween = d2.year() - d1.year();
// 	if (pd1 > pd2)
// 		fullYearsInBetween -= 1;
}