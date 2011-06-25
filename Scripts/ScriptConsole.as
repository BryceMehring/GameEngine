
void OpenConsoleButton(bool b)
{
	console.open();
}
void CloseConsoleButton(bool b)
{
	console.close();
}

void main()
{
	CheckBoxData@ data = CheckBoxData();
	data.checked = false;
	data.name = "Open Console";
	
	data.rect.left = 0;
	data.rect.top = 300;
	data.func = as.GetFunc(@OpenConsoleButton);

	ui.AddElement("CheckBox",@data);

	CheckBoxData@ data2 = CheckBoxData();
	data2.checked = false;
	data2.name = "Close Console";
	
	data2.rect.left = 150;
	data2.rect.top = 300;
	data2.func = as.GetFunc(@CloseConsoleButton);

	ui.AddElement("CheckBox",@data2);
}
