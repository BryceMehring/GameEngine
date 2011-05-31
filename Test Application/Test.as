
// Response
void CheckResponse(bool checked)
{
	if(checked)
	{
		print("Checked\n");
	}
	else
	{
		print("Not Checked\n");
	}
}

void OpenFile(bool b)
{
	string fileName = engine.OpenFileName();
	uint i = as.BuildScriptFromFile(fileName);
	as.ExecuteScript(i);
}


void main()
{
	CheckBoxData@ myCheckData = CheckBoxData();
	myCheckData.checked = false;
	myCheckData.name = "Open Script";
	
	myCheckData.pos.x = 100;
	myCheckData.pos.y = 100;

	myCheckData.func = GetId(@OpenFile);
	myCheckData.scriptIndex = scriptId;

	uint id = ui.AddElement("CheckBox",@myCheckData);

	/*CheckBoxData@ myCheckData2 = CheckBoxData();
	myCheckData2.checked = false;
	myCheckData2.name = "Start Game \n";
	
	myCheckData2.pos.x = 200;
	myCheckData2.pos.y = 30;

	myCheckData2.func = GetId(@CheckResponse);
	myCheckData2.scriptIndex = scriptId;

	id = ui.AddElement("CheckBox",@myCheckData2);*/
}

  