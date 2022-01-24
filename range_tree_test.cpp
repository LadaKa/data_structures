// Modified code from :
// https://gitlab.kam.mff.cuni.cz/datovky/assignments/-/blob/master/12-range_tree/cpp/range_tree_test.cpp

#include <algorithm>
#include <functional>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// If the condition is not true, report an error and halt.
#define EXPECT(condition, message) do { if (!(condition)) expect_failed(message); } while (0)
void expect_failed(const string& message);

#include "range_tree.h"

void create_test_tree(int64_t size, bool ascending, Tree& tree) {
	vector<int64_t> sequence = { 7 };
	for (int64_t i = 2; i < size; i++)
		sequence.push_back((sequence.back() * sequence.front()) % size);
	if (ascending)
		sort(sequence.begin(), sequence.end());

	for (int64_t element : sequence)
		tree.insert(element, element);
}

void test_missing(int64_t size, bool ascending) {
	Tree tree;
	create_test_tree(size, ascending, tree);

	int64_t values = 0;
	for (int64_t i = 0; i < size; i++)
		values += tree.range_sum(-size, 0) + tree.range_sum(size, 2 * size);
	EXPECT(values == 0, "Expected no values in an empty range");
}

void test_suffixes(int64_t size, bool ascending) {
	Tree tree;
	create_test_tree(size, ascending, tree);

	for (int64_t left = 1; left < size; left++) {
		int64_t values = tree.range_sum(left, size - 1);
		int64_t expected = size * (size - 1) / 2 - left * (left - 1) / 2;
		EXPECT(values == expected,
			"Expected " + to_string(expected) + " for range [" + to_string(left) +
			", " + to_string(size - 1) + "], got " + to_string(values));
	}
}

void test_updates(int64_t size, bool ascending) {
	Tree tree;
	create_test_tree(size, ascending, tree);

	for (int64_t left = 1; left < size; left++) {
		tree.remove(left);
		tree.insert(left + size - 1, left + size - 1);
		int64_t values = tree.range_sum(left + 1, size + left);
		int64_t expected = (size + left) * (size + left - 1) / 2 - (left + 1) * left / 2;
		EXPECT(values == expected,
			"Expected " + to_string(expected) + " for range [" + to_string(left + 1) +
			", " + to_string(size + left) + "], got " + to_string(values));
	}
}

void test_range_update(int64_t size, bool ascending)
{

	int64_t left;
	int64_t right;
	int64_t delta = 10;

	for (int64_t i = -size; i <= size + 1; i++) {
		for (int64_t j = -size; j <= size + 1; j++) {

			cout << i << " " << j << endl;
			Tree tree;
			create_test_tree(size, ascending, tree);

			tree.remove(i);
			tree.insert(i, 0);

			int result = tree.test_range_update(i, j, delta);
				
			EXPECT(result == 0, "Error in range update.");
		}

	}
}


vector<pair<string, function<void()>>> tests = {
	{"random_missing", [] { test_missing(13, false); }},
	{"random_suffixes", [] { test_suffixes(13, false); }},
	{"random_updates", [] { test_updates(13, false); }},

	{"path_missing", [] { test_missing(13, true); }},
	{"path_suffixes", [] { test_suffixes(13, true); }},
	{"path_updates", [] { test_updates(13, true); }},

	{"random_missing_big", [] { test_missing(199999, false); }},
	{"random_suffixes_big", [] { test_suffixes(199999, false); }},
	{"random_updates_big", [] { test_updates(199999, false); }},

	{"path_missing_big", [] { test_missing(199999, true); }},
	{"path_suffixes_big", [] { test_suffixes(199999, true); }},
	{"path_updates_big", [] { test_updates(199999, true); }},
	
	{"range_update", [] { test_range_update(13, true); }},
};
