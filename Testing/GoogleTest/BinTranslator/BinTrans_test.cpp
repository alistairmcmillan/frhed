#include <gtest/gtest.h>
#include <windows.h>
#include "Simparr.h"
#include "BinTrans.h"

namespace
{
	// The fixture for testing paths functions.
	class SimpleArrayTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		SimpleArrayTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~SimpleArrayTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
		}

		// If	the	constructor	and	destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:

		virtual void SetUp()
		{
			// Code	here will be called	immediately	after the constructor (right
			// before each test).
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
		}

		// Objects declared here can be used by all tests in the test case for Foo.
	};

	TEST_F(SimpleArrayTest, Construct1)
	{
		Text2BinTranslator tr("a");
		EXPECT_FALSE(tr.IsEmpty());
		EXPECT_EQ(1, tr.GetLength());
	}

}  // namespace
