#include "list.h"

#include "utestpp/utestpp.h"

#include <iterator>
#include <algorithm>
#include <utility>
#include <vector>

void testEmpty() {
	cntr::list<int> l;
	UTESTPP_ASSERT_TRUE(l.empty());
	UTESTPP_ASSERT_EQUAL(l.size(), 0);
}

void testConstructors() {
	auto l1 = cntr::list<int>{1, 2, 3};
	UTESTPP_ASSERT_TRUE(!l1.empty());
	UTESTPP_ASSERT_EQUAL(l1.size(), 3);
	UTESTPP_ASSERT_EQUAL(l1.back(), 3);
	
	auto count_ctr = cntr::list<int>(3);
	UTESTPP_ASSERT_EQUAL(count_ctr.size(), 3);
	
	auto count_ctr_with_value = cntr::list<int>(3, 5);
	UTESTPP_ASSERT_EQUAL(count_ctr_with_value.size(), 3);
	UTESTPP_ASSERT_EQUAL(count_ctr_with_value.front(), 5);
	UTESTPP_ASSERT_EQUAL(count_ctr_with_value.back(), 5);
	
	auto v = std::vector<int>{1, 2, 3};
	cntr::list<int> iterator_ctr(std::begin(v), std::end(v));
	UTESTPP_ASSERT_EQUAL(iterator_ctr.size(), 3);
	UTESTPP_ASSERT_EQUAL(iterator_ctr.back(), 3);
	
	int array[] = {1, 2, 3};
	
	cntr::list<int> iterator_cnt_array(array, array + 3);
	UTESTPP_ASSERT_EQUAL(iterator_cnt_array.size(), 3);
	UTESTPP_ASSERT_EQUAL(iterator_cnt_array.back(), 3);
	
	cntr::list<int> copy_of_iterator_cnt_array = iterator_cnt_array;
	UTESTPP_ASSERT_EQUAL(iterator_cnt_array.size(), copy_of_iterator_cnt_array.size());
	UTESTPP_ASSERT_EQUAL(iterator_cnt_array.back(), copy_of_iterator_cnt_array.back());
	
	// Make sure copies are not linked under the covers
	iterator_cnt_array.front() = 19;
	copy_of_iterator_cnt_array.front() = 92;
	UTESTPP_ASSERT_EQUAL(iterator_cnt_array.front(), 19);
	UTESTPP_ASSERT_EQUAL(copy_of_iterator_cnt_array.front(), 92);
	
	// TODO: Figure out a better way to test this. Moving doesn't require that
	//empty() return true. This depends on UB.
	auto moved = cntr::list<int>(std::move(iterator_cnt_array));
	UTESTPP_ASSERT_TRUE(!moved.empty());
	UTESTPP_ASSERT_TRUE(iterator_cnt_array.empty());
}

void testPush() {
	cntr::list<int> l;
	l.push_back(1);
	l.push_back(2);
	l.push_back(3);
	UTESTPP_ASSERT_EQUAL(l.size(), 3);
	UTESTPP_ASSERT_TRUE(!l.empty());
}

void testPopBack() {
	cntr::list<int> l{1, 2, 3};
	l.pop_back();
	UTESTPP_ASSERT_EQUAL(l.size(), 2);
	UTESTPP_ASSERT_EQUAL(l.back(), 2);
}

void testPopFront() {
	cntr::list<int> l{1, 2, 3};
	l.pop_front();
	UTESTPP_ASSERT_EQUAL(l.size(), 2);
	UTESTPP_ASSERT_EQUAL(l.front(), 2);
}

void testForwardIteratorsForward() {
	cntr::list<int> l{1, 2, 3};
	int i = 1;
	for (auto beg = l.begin(); beg != l.end(); ++beg, ++i) {
		UTESTPP_ASSERT_EQUAL(*beg, i);
	}
	i = 1;
	for (auto beg = l.begin(); beg != l.end(); beg = std::next(beg), ++i) {
		UTESTPP_ASSERT_EQUAL(*beg, i);
	}
	i = 1;
	for (auto beg = l.cbegin(); beg != l.cend(); beg = std::next(beg), ++i) {
		UTESTPP_ASSERT_EQUAL(*beg, i);
	}
	auto it = l.begin();
	auto preIncrement = ++it;
	auto postIncrement = it++;
	UTESTPP_ASSERT_EQUAL(preIncrement, postIncrement);
	UTESTPP_ASSERT_EQUAL(*preIncrement, *postIncrement);
	UTESTPP_ASSERT_EQUAL(*it, 3);
	
	int sum = 0;
	for (const auto& i : l) {
		sum += i;
	}
	UTESTPP_ASSERT_EQUAL(sum, 1 + 2 + 3);
	
}

void testForwardIteratorsBackward() {
	cntr::list<int> l{1, 2, 3};
	int i = 3;
	for (auto it = l.end(); it != l.begin(); --i) {
		UTESTPP_ASSERT_EQUAL(*--it, i);
	}
}

void testReverseIteratorsForward() {
	cntr::list<int> l{1, 2, 3};
	int i = 3;
	for (auto it = l.rbegin(); it != l.rend(); ++it, --i) {
		UTESTPP_ASSERT_EQUAL(*it, i);
	}
}

void testReverseIteratorsBackward() {
	cntr::list<int> l{1, 2, 3};
	int i = 1;
	for (auto it = l.rend(); it != l.rbegin(); ++i) {
		UTESTPP_ASSERT_EQUAL(*--it, i);
	}
}

void testMutationViaIterator() {
	cntr::list<int> l{1, 2, 3};
	std::vector<int> v{1, 2, 3};
	
	UTESTPP_ASSERT_TRUE(std::equal(std::begin(l), std::end(l), std::begin(v)));
	
	for (auto& i : v) { i *= 3; }
	for (auto& i : l) { i *= 3; }
	
	UTESTPP_ASSERT_TRUE(std::equal(std::begin(l), std::end(l), std::begin(v)));
}

void testIteratorToConstIteratorConversion() {
	using iterator = cntr::list<int>::iterator;
	using const_iterator = cntr::list<int>::const_iterator;
	
	auto l = cntr::list<int>{1, 2, 3};
	
	iterator it = l.begin();
	const_iterator const_it = it;
	const_iterator const_it_assign;
	const_it_assign = it;

	UTESTPP_ASSERT_EQUAL(*it, *const_it);
	UTESTPP_ASSERT_EQUAL(*const_it, *const_it_assign);
	UTESTPP_ASSERT_EQUAL(const_it, const_it_assign);
}

int main() {
	UTESTPP_INIT();
	
	testEmpty();
	
	testConstructors();
	
	testPush();
	
	testPopBack();
	testPopFront();
	
	testForwardIteratorsForward();
	testForwardIteratorsBackward();
	
	testReverseIteratorsForward();
	testReverseIteratorsBackward();
	
	testMutationViaIterator();
	
	testIteratorToConstIteratorConversion();
	
	return !UTESTPP_FINISH();
}
