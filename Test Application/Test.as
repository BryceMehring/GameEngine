
void Function(bool b)
{
	// do something interesting!
}

void Nothing()
{
	print("Hello");
}

void CheckBoxResponce()
{

}

void main()
{
	CheckBoxData myCheckData;
	myCheckData.checked = true;
	myCheckData.name = "Hello\n";
	myCheckData.rect.left = 0;

	ui.AddCheckBox(myCheckData);

	TakeFuncion(@Nothing);

	print(myCheckData.name);

	uint id = as.BuildScriptFromFile("C:\\Users\\Bryce\\Desktop\\TestScript.as");
	as.ExecuteScript(id);
	//print("\nHello World\n\n");
}

  