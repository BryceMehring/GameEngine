
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

interface ITrigger
{
	bool IsTriggered();
};

class trigger : ITrigger
{
	trigger() {}

	bool IsTriggered()
	{
		return input.MouseClick(1);
	}

};

void OpenFile(bool b)
{
	string fileName = engine.OpenFileName();
	uint i = as.BuildScriptFromFile(fileName);
	as.ExecuteScript(i);
}

void CreateEdge(UINode@ node, UINode@ node2)
{
	uint id = graph.CreateEdge(node,node2);
	UIEdge@ edge = graph.GetEdge(id);
	UIEdge@ edge2 = graph.GetEdge(id + 1);
	edge.AddTrigger(theTrigger);
	edge2.AddTrigger(theTrigger);
}


void main()
{
	// node 1
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

	// node 2, link node 1 to node 2
	UINode@ node2 = graph.AddVertex();

	UITrigger@ theTrigger = UITrigger(element);

	CreateEdge();
}

  