
funcdef void CALLBACK(bool);

void Function(bool b)
{
	// do something interesting!
}

void main()
{
	CALLBACK@ funct = @Function;

	CheckBoxData myCheckData;
	myCheckData.m_checked = true;
	myCheckData.m_str = "Hello\n";
	myCheckData.m_Rect.left = 0;

	ui.AddCheckBox(myCheckData);

	print(myCheckData.m_str);

	uint id = as.BuildScriptFromFile("C:\\Users\\Bryce\\Desktop\\TestScript.as");
	as.ExecuteScript(id);
	//print("\nHello World\n\n");
}

  