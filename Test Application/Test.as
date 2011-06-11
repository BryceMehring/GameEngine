
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
	
	myCheckData.pos.x = 0;
	myCheckData.pos.y = 0;

	myCheckData.func = as.GetFunc(@OpenFile);

	uint id = ui.AddElement("CheckBox",@myCheckData);
}

  