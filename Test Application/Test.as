/*class Int
{

	Int(int a)
	{
		m_data = a;
	}

	void Display()
	{
		print(m_data);
	}

	private int m_data;

};

int Test(int data)
{
	int iReturn = 1;

	if(data > 0)
	{
		iReturn = data * Test(data - 1);
	}

	return iReturn;
}*/

class TestObject
{
	TestObject(int a, int b)
	{
		m_total = a * b;
	}

	int GetTotal() { return m_total; }

	int m_total;
	
}



void main()
{
	//Int myInt1(10);

	//myInt1.Display();
	for(int i = 0; i < 5000; ++i)
	{
		TestObject a1(i,i+1);
		Print(a1.GetTotal());

	}


	//print(i++);
}

  