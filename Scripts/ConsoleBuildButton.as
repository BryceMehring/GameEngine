
void BuildScript(bool b)
{
	//console.
	console.Build();
}

void main()
{
	CheckBoxData@ myCheckData = CheckBoxData();
	myCheckData.checked = false;
	myCheckData.func = as.GetFunc(@BuildScript);
	myCheckData.name = "Build Script";
	
	myCheckData.rect.left = 650;
	myCheckData.rect.top = 250;
	myCheckData.checked = false;

	UINode@ node = ui.GetCurrentNode();
	IUIElement@ element = ui.CreateElement("CheckBox",@myCheckData);
	node.AddElement(element);

}