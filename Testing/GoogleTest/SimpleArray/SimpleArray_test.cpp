#include <gtest/gtest.h>
#include <windows.h>
#include "Simparr.h"

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
		SimpleArray<int> arr;
		EXPECT_TRUE(arr.blIsEmpty());
		EXPECT_EQ(0, arr.GetSize());
		EXPECT_EQ(0, arr.GetLength());
	}

	TEST_F(SimpleArrayTest, Construct2)
	{
		SimpleArray<int> arr(5, 1);
		EXPECT_TRUE(arr.blIsEmpty());
		EXPECT_EQ(5, arr.GetSize());
		EXPECT_EQ(0, arr.GetLength());
		EXPECT_EQ(1, arr.GetGrowBy());
	}

	TEST_F(SimpleArrayTest, Construct3)
	{
		SimpleArray<int> arr(7, 4);
		EXPECT_TRUE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(0, arr.GetLength());
		EXPECT_EQ(4, arr.GetGrowBy());
	}

/*	TEST_F(SimpleArrayTest, Construct4)
	{
		const int t[3] = {1, 3, 5};
		SimpleArray<int> arr(&t[0], 4, 2);
		EXPECT_TRUE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(0, arr.GetLength());
		EXPECT_EQ(4, arr.GetGrowBy());
	}
*/

	TEST_F(SimpleArrayTest, Construct5)
	{
		SimpleArray<int> arr2(7, 4);
		SimpleArray<int> arr(arr2);

		EXPECT_TRUE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(0, arr.GetLength());
		EXPECT_EQ(4, arr.GetGrowBy());
	}

	TEST_F(SimpleArrayTest, Append1)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(3);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(1, arr.GetLength());
		EXPECT_EQ(3, arr.GetAt(0));
	}

	TEST_F(SimpleArrayTest, Append2)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(3);
		arr.Append(5);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(2, arr.GetLength());
		EXPECT_EQ(3, arr.GetAt(0));
		EXPECT_EQ(5, arr.GetAt(1));
	}

	TEST_F(SimpleArrayTest, Append3)
	{
		SimpleArray<int> arr(1, 4);
		arr.Append(3);
		arr.Append(5);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(5, arr.GetSize());
		EXPECT_EQ(2, arr.GetLength());
		EXPECT_EQ(3, arr.GetAt(0));
		EXPECT_EQ(5, arr.GetAt(1));
	}

	TEST_F(SimpleArrayTest, Append4)
	{
		SimpleArray<int> arr(1, 2);
		arr.Append(3);
		arr.Append(5);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(3, arr.GetSize());
		EXPECT_EQ(2, arr.GetLength());
		EXPECT_EQ(3, arr.GetAt(0));
		EXPECT_EQ(5, arr.GetAt(1));
	}

	// THIS FAILS
	// Looks like InsertAt() does not work for empty array?
	TEST_F(SimpleArrayTest, Insert1)
	{
		SimpleArray<int> arr(7, 4);
		arr.InsertAt(0, 3);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(1, arr.GetLength());
		EXPECT_EQ(3, arr.GetAt(0));
	}

	TEST_F(SimpleArrayTest, Insert2)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(2);
		arr.InsertAt(0, 3);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(2, arr.GetLength());
		EXPECT_EQ(3, arr.GetAt(0));
		EXPECT_EQ(2, arr.GetAt(1));
	}
	
	// THIS FAILS
	TEST_F(SimpleArrayTest, Insert3)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(2);
		arr.InsertAt(1, 3);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(2, arr.GetLength());
		EXPECT_EQ(2, arr.GetAt(0));
		EXPECT_EQ(3, arr.GetAt(1));
	}

	TEST_F(SimpleArrayTest, Insert4)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(2);
		arr.InsertAt(0, 3);
		arr.InsertAt(0, 4);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(3, arr.GetLength());
		EXPECT_EQ(4, arr.GetAt(0));
		EXPECT_EQ(3, arr.GetAt(1));
		EXPECT_EQ(2, arr.GetAt(2));
	}

	TEST_F(SimpleArrayTest, SetAt1)
	{
		SimpleArray<int> arr(7, 4);
		arr.SetAt(0, 3);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(1, arr.GetLength());
		EXPECT_EQ(3, arr.GetAt(0));
	}

	TEST_F(SimpleArrayTest, SetAt2)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(2);
		arr.SetAt(0, 3);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(1, arr.GetLength());
		EXPECT_EQ(3, arr.GetAt(0));
	}

	TEST_F(SimpleArrayTest, ClearAll)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(2);
		arr.ClearAll();
		
		EXPECT_TRUE(arr.blIsEmpty());
		EXPECT_EQ(0, arr.GetSize());
		EXPECT_EQ(0, arr.GetLength());
	}

	TEST_F(SimpleArrayTest, RemoveAt1)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(2);
		arr.RemoveAt(0);
		
		EXPECT_TRUE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(0, arr.GetLength());
	}

	TEST_F(SimpleArrayTest, RemoveAt2)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(2);
		arr.Append(3);
		arr.RemoveAt(0);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(1, arr.GetLength());
		EXPECT_EQ(3, arr.GetAt(0));
	}

	TEST_F(SimpleArrayTest, RemoveAt3)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(2);
		arr.Append(3);
		arr.RemoveAt(1);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(1, arr.GetLength());
		EXPECT_EQ(2, arr.GetAt(0));
	}

	TEST_F(SimpleArrayTest, RemoveAt4)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(2);
		arr.Append(3);
		arr.Append(4);
		arr.RemoveAt(1);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(2, arr.GetLength());
		EXPECT_EQ(2, arr.GetAt(0));
		EXPECT_EQ(4, arr.GetAt(1));
	}

	TEST_F(SimpleArrayTest, Exchange1)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(2);
		arr.Append(3);
		arr.Exchange(0, 1);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(2, arr.GetLength());
		EXPECT_EQ(3, arr.GetAt(0));
		EXPECT_EQ(2, arr.GetAt(1));
	}
	TEST_F(SimpleArrayTest, Exchange2)
	{
		SimpleArray<int> arr(7, 4);
		arr.Append(2);
		arr.Append(3);
		arr.Append(4);
		arr.Exchange(0, 2);
		
		EXPECT_FALSE(arr.blIsEmpty());
		EXPECT_EQ(7, arr.GetSize());
		EXPECT_EQ(3, arr.GetLength());
		EXPECT_EQ(4, arr.GetAt(0));
		EXPECT_EQ(2, arr.GetAt(2));
	}
}  // namespace

