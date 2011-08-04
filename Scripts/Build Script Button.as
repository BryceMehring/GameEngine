
// Response
/*void CheckResponse(bool checked)
{
	if(checked)
	{
		print("Checked\n");
	}
	else
	{
		print("Not Checked\n");
	}
}*/

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
	
	myCheckData.rect.left = 150;
	myCheckData.rect.top = 50;
	myCheckData.checked = false;

	UIGraph@ graph = ui.GetGraph();
	UINode@ node = graph.AddVertex();
	IUIElement@ element = ui.CreateElement("CheckBox",@myCheckData);

	node.AddElement(element);

	//ui.SetCurrentNode(node);
}

  