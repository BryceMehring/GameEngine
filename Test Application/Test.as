
void Function(bool b)
{
	// do something interesting!
}

void Nothing()
{
	print("The Function");
}

void CheckBoxResponce()
{

}

void main(uint scriptId)
{
	CheckBoxData myCheckData;
	myCheckData.checked = false;
	myCheckData.name = "Call AngelScript Function \n";
	
	myCheckData.rect.left = 8;
	myCheckData.rect.top = 30;
	myCheckData.rect.right = 150;
	myCheckData.rect.bottom = 80;

	myCheckData.func = GetId(@Nothing);
	myCheckData.scriptIndex = scriptId;

	ui.AddElement("CheckBox",myCheckData);

	//CheckBox@ myCheckBox=CheckBox(myCheckData);

	//ui.AddCheckBox(myCheckData);

	print(myCheckData.name);

	/*uint id = as.BuildScriptFromFile("C:\\Users\\Bryce\\Desktop\\TestScript.as");
	as.ExecuteScript(id);
	//print("\nHello World\n\n");*/
}

  