
void BuildScript()
{
	//console.
	console.build();
}

void Next()
{

}

void main()
{
	CheckBoxData@ myCheckData = CheckBoxData();
	myCheckData.checked = false;
	myCheckData.func = as.GetFunc(@BuildScript);
	myCheckData.name = "Build Script";
	
	myCheckData.rect.left = 650;
	myCheckData.rect.top = 150;
	myCheckData.checked = false;

	UINode@ node = ui.GetCurrentNode();
	IUIElement@ element = ui.CreateElement("CheckBox",@myCheckData);
	node.AddElement(element);
}