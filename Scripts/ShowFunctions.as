
void ListFunctionsButton(bool b)
{
	as.ListFunctions();
}

void ListVarButton(bool b)
{
	as.ListVariables();
}
void ListObjButton(bool b)
{
	as.ListObjects();
}


void main()
{

	CheckBoxData@ data = CheckBoxData();
	data.checked = false;
	data.name = "List Functions";
	
	data.rect.left = 0;
	data.rect.top = 50;
	data.func = as.GetFunc(@ListFunctionsButton);

	ui.AddElement("CheckBox",@data);

	CheckBoxData@ data2 = CheckBoxData();
	data2.checked = false;
	data2.name = "List Variables";
	
	data2.rect.left = 150;
	data2.rect.top = 50;
	data2.func = as.GetFunc(@ListVarButton);

	ui.AddElement("CheckBox",@data2);

	CheckBoxData@ data3 = CheckBoxData();
	data3.checked = false;
	data3.name = "List Objects";
	
	data3.rect.left = 250;
	data3.rect.top = 150;
	data3.func = as.GetFunc(@ListObjButton);

	ui.AddElement("CheckBox",@data3);
}
