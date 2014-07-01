#include "gtest/gtest.h"
#include "generator_common.h"

TEST(MetaPathTest, Zero) {
	gen::volatile_path_t volatile_path;

	volatile_path.add_absolute("root");
	volatile_path.add_absolute("id1");

	EXPECT_FALSE(volatile_path.is_relative());

	gen::volatile_path_t::iterator_t iter = volatile_path.iterator();
	EXPECT_EQ(myvi::string_t("root"), iter.next());
	EXPECT_EQ(myvi::string_t("id1"), iter.next());
	EXPECT_TRUE(iter.next().is_empty());

	EXPECT_EXIT( {
		gen::meta_path_t meta_path(".root.id1"); // passed
		for (gen::meta_path_t::iterator_t iter = meta_path.iterator(); !iter.next().is_empty(); );
		exit(0);
	}, ::testing::ExitedWithCode(0), ".*");
	
	EXPECT_DEATH( {
		gen::meta_path_t meta_path1("..root.id1"); // fail
		for (gen::meta_path_t::iterator_t iter = meta_path1.iterator(); !iter.next().is_empty(); );
	}, ".*");

	EXPECT_DEATH( {
	gen::meta_path_t meta_path2("root..id1"); // fail
	for (gen::meta_path_t::iterator_t iter = meta_path2.iterator(); !iter.next().is_empty(); );
	}, ".*");

	EXPECT_DEATH( {
	gen::meta_path_t meta_path3("root.id1."); // fail
	for (gen::meta_path_t::iterator_t iter = meta_path3.iterator(); !iter.next().is_empty(); );
	}, ".*");
}
