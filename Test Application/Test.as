class Int
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
}

void main()
{
	Int myInt1(10);

	//myInt1.Display();

	Print(Test(10));

	//print(i++);
}
  