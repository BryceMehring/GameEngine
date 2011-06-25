
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
	myCheckData.func = as.GetFunc(@OpenFile);

	myCheckData.name = "Open Script";
	
	myCheckData.rect.left = 0;
	myCheckData.rect.top = 0;

	uint id = ui.AddElement("CheckBox",@myCheckData);
}

  